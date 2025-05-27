/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#define DELAY (1000 / portTICK_PERIOD_MS)
#define BUF_SIZE (1024 * 2)

const uart_port_t uart_num = UART_NUM_0;

void commandEcho(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    printf("%s\n", argv[i]);
  }
}
void uart(void *arg) {
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
  while (1) {
    char data[128];
    int length = 0;

    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t *)&length));
    length = uart_read_bytes(uart_num, data, length, 100);
    if (length > 0) {
      char *stuff[10];
      *stuff[0] = data[0];
      *stuff[1] = data[1];
      commandEcho(10, stuff);
    }
    length = uart_read_bytes(uart_num, data, length, 100); // Feeling useless might delete later
    vTaskDelay(DELAY / 10);
  }
}
void app_main(void) {

  xTaskCreate(uart, "uart", 4096, NULL, 1, NULL);
  vTaskDelay(DELAY * 2);

  fflush(stdout); //Feeling useless might delete later
}
