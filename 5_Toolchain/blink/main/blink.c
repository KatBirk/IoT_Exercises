#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>
#define DELAY (1000 / portTICK_PERIOD_MS)
#define PIN (23)

void TaskBlink(void *pvParameters) {
  uint8_t state = 1;
  while (1) {
    gpio_set_level(PIN, state);
    vTaskDelay(DELAY);
    state = !state;
  }
}
void app_main(void) {
  printf("Hello world!\n");

  gpio_reset_pin(PIN);
  gpio_set_direction(PIN, GPIO_MODE_OUTPUT);
  xTaskCreate(TaskBlink, "Blink", 4096, NULL, 1, NULL);

  vTaskDelay(DELAY * 2);

  fflush(stdout);
  esp_restart();
}
