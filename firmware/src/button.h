#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <nvs_flash.h>

#define GPIO_SOFT_RESET 25

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));

            if (io_num ==  GPIO_SOFT_RESET) {
                nvs_flash_erase();
                esp_restart();
            }
        }
    }
}

void gpio_setup(void)
{
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

    gpio_set_direction(GPIO_SOFT_RESET, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_SOFT_RESET, GPIO_PULLUP_ONLY);

    //install gpio isr service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_SOFT_RESET, gpio_isr_handler, (void*) GPIO_SOFT_RESET);

    gpio_wakeup_enable(GPIO_SOFT_RESET, GPIO_INTR_LOW_LEVEL);
    gpio_set_intr_type(GPIO_SOFT_RESET, GPIO_INTR_LOW_LEVEL);
    gpio_intr_enable(GPIO_SOFT_RESET);
}

