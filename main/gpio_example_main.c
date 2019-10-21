#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

/**
 * Brief:
 * Configure gpios to toggle LED on button press using interrupts
 * TODO: Debounce button presses
 */

#define LED_GPIO_OUTPUT_IO_0    23
#define BUTTON_GPIO_INPUT_IO_0     4
#define GPIO_INPUT_PIN_SEL  ((1ULL<<BUTTON_GPIO_INPUT_IO_0))
#define ESP_INTR_FLAG_DEFAULT 0

static xQueueHandle gpio_evt_queue = NULL;

// initialize variable that determines led_level to 0
static uint32_t led_level = 0;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void button_to_led_task(void* arg)
{
    uint32_t io_num, button_level;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            button_level = gpio_get_level(io_num);
            // toggle LED level
            led_level = !led_level;
            gpio_set_level(LED_GPIO_OUTPUT_IO_0, led_level);
            printf("GPIO[%d] intr, val: %d      LED_level: %d\n", io_num, button_level, led_level);
        }
    }
}

void app_main()
{
    // configure button
    gpio_config_t io_conf;
    //interrupt of falling edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // configure LED
    gpio_pad_select_gpio(LED_GPIO_OUTPUT_IO_0);
    // Set the GPIO as a push/pull output
    gpio_set_direction(LED_GPIO_OUTPUT_IO_0, GPIO_MODE_OUTPUT);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(button_to_led_task, "button_to_led_task", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(BUTTON_GPIO_INPUT_IO_0, gpio_isr_handler, (void*) BUTTON_GPIO_INPUT_IO_0);

    // start out with LED off
    gpio_set_level(LED_GPIO_OUTPUT_IO_0, led_level);

    // int cnt = 0;
    while(1) {
        // wait for button press
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
