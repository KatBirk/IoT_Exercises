/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "driver/adc_types_legacy.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>
#define DELAY (1000 / portTICK_PERIOD_MS)
#define PIN (33)
#define ADC ADC_UNIT_1
#define ADC_CHANNEL ADC1_CHANNEL_5

const static char *TAG = "Log Tag:";

void readADC(void *pvParameters) {
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

  int adc_raw[2][10]; // If only reading one ADC and channel it only need to be
                      // an int. This is prefered to be able to hold all
                      // permutations of ADC's and channels

  // Reading the ADC
  while (1) {

    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL, &adc_raw[0][0]));
    ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC + 1, ADC_CHANNEL,
             adc_raw[0][0]);
    vTaskDelay(DELAY);
  }
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
  ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC + 1, ADC_CHANNEL,
           adc_raw);
  vTaskDelay(DELAY);
   ESP_ERROR_CHECK(adc_oneshot_del_unit(adc1_handle)); // CLEAN UP, so other methods can use the channel

    return adc_raw;

}

void app_main(void) {
  printf("THE SAMPLE:%d\n",sample());
  fflush(stdout);
  xTaskCreate(readADC, "ADC", 4096, NULL, 1, NULL);
  vTaskDelay(DELAY * 2);


  //  esp_restart();
}
