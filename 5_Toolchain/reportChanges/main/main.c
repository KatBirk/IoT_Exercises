/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>
#define DELAY (1000 / portTICK_PERIOD_MS)
#define PIN (32)

void TaskBlink(void *pvParameters) {
  while (1) {
    int val = gpio_get_level(PIN);
    printf("%d\n", val);
    vTaskDelay(DELAY);
  }
}
void app_main(void) {
  gpio_pulldown_en(PIN); // Pulldown is important
  gpio_set_direction(PIN, GPIO_MODE_INPUT);
  xTaskCreate(TaskBlink, "ReadPin", 4096, NULL, 1, NULL);
  vTaskDelay(DELAY * 2);
  fflush(stdout);
  esp_restart();
}
