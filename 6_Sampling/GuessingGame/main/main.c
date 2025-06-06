/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "driver/adc_types_legacy.h"
#include "driver/uart.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#define DELAY (1000 / portTICK_PERIOD_MS)
#define PIN (33)
#define ADC ADC_UNIT_1
#define ADC_CHANNEL ADC1_CHANNEL_5
#define BUF_SIZE (1024 * 2)
const uart_port_t uart_num = UART_NUM_0;
volatile int gameStarted = 0;
void configUart() {

  uart_config_t uart_config = {
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // Disable flow control
      .rx_flow_ctrl_thresh = 122,
  };

  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

  QueueHandle_t uart_queue;
  // Install UART driver using an event queue here
  ESP_ERROR_CHECK(
      uart_driver_install(uart_num, BUF_SIZE, BUF_SIZE, 10, &uart_queue, 0));
}
void handleUserInput(char *data) {
  while (1) {
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t *)&length));
    length = uart_read_bytes(uart_num, data, length, 100);
    if (length > 0) {
      break;
    }
    vTaskDelay(DELAY / 10);
  }
  fflush(stdout);
}

int sample() {
  // Config to use ADC1
  adc_oneshot_unit_handle_t adc1_handle;
  adc_oneshot_unit_init_cfg_t init_config1 = {
      .unit_id = ADC_UNIT_1,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH_DEFAULT,
      .atten = ADC_ATTEN_DB_11,
  };

  // Config for the channel 5 which is GPIO 33
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &config));

  int adc_raw;
  // Reading the ADC once
  ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw));
  vTaskDelay(DELAY);
  ESP_ERROR_CHECK(adc_oneshot_del_unit(
      adc1_handle)); // CLEAN UP, so other methods can use the channel

  return adc_raw;
}

void shallWeBeginPrompt() {
  while (1) {
    if (gameStarted == 0) {
      printf("Shall we begin? y/n\n");
      fflush(stdout);
    }
    vTaskDelay(DELAY * 5);
  }
}
void app_main(void) {
  configUart();
  xTaskCreate(shallWeBeginPrompt, "begin?", 4096, NULL, 1, NULL);
  char answer[BUF_SIZE];
  while (1) {
    handleUserInput(answer);
    if (answer[0] == 'y') {
      gameStarted = 1;
      int secret =
          sample() + 1; // I add one since nobody wants to included zero
      int attempts = (int)log2(4096);
      printf("Can you guess the number from 1-4096 in %d attempts\n",
             attempts); // This is a binary search so it can be guessed
      int done = 0;
      int count = 0;
      while (!done) {
        handleUserInput(answer);
        int guess = atoi(answer);
        count++;
        if (count >= attempts) {
          break;
        }
        if (guess == secret) {
          if (count == 1) {
            printf("You guessed correctly in just %d try\n", count);
          } else {

            printf("You guessed correctly in just %d tries\n", count);
          }
          done = 1;
          break;
        } else if (guess > secret) {
          printf("Too High\n");
        } else if (guess < secret) {
          printf("Too Low\n");
        }
        if (count == 1) {
          printf("You guessed %d time\n", count);
        } else {

          printf("You guessed %d times\n", count);
        }
      }
      if (!done) {
        printf("The only way of winning is to not to play\n");
      }
      fflush(stdout);
      gameStarted = 0;
    } else {
      esp_restart();
    }
  }
}
