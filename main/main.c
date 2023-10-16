#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/queue.h"

#define NORMAL_STATE 1
#define PRESSED_STATE 0

/* Disable watchdog */
#define CONFIG_ESP_TASK_WDT_INIT 0

int button_is_pressed = 0;
int button_is_held = 0;

int KeyReg0 = NORMAL_STATE;
int KeyReg1 = NORMAL_STATE;
int KeyReg2 = NORMAL_STATE;
int KeyReg3 = NORMAL_STATE;

int TimeOutForKeyPress = 100;

/*funtion anti-vibration handle */
void getKeyInput(){
    KeyReg2 = KeyReg1;
    KeyReg1 = KeyReg0;
    KeyReg0 = gpio_get_level(GPIO_NUM_17);
    if ((KeyReg1 == KeyReg0) && (KeyReg1 == KeyReg2)){
        if (KeyReg2 != KeyReg3){
            KeyReg3 = KeyReg2;
            if (KeyReg3 == PRESSED_STATE){
                TimeOutForKeyPress = 100;
            button_is_pressed = 1;
            }
        }
        else{
            TimeOutForKeyPress --;
            if (TimeOutForKeyPress == 0){
                if (KeyReg0 == NORMAL_STATE){
        		    KeyReg3 = KeyReg0;
        		    button_is_held = 0;
        	    }
                else
                    button_is_held = 1;
                TimeOutForKeyPress = 100;
            }
        }
    }
}

void button_call_back_func(void* param){
    getKeyInput();
}
void Cyclic_Task (void* parameter)
{
    
    while(1)
    {
        printf("ID: 2014160 - 2012775. Time: %lld s\n",esp_timer_get_time() / 1000000);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete( NULL) ;
}
void Uncyclic_Task (void* parameter)
{
    gpio_pad_select_gpio ( GPIO_NUM_17 ) ;
    gpio_set_direction ( GPIO_NUM_17 , GPIO_MODE_INPUT ) ;
    while(1)
    { 
        if (button_is_held == 1){
            printf("Button is being held\n");
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        else if (button_is_pressed == 1){
            button_is_pressed = 0;
            printf("ESP32\n");
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete ( NULL ) ;
}

void app_main(void)
{    
    const esp_timer_create_args_t my_timer_args = {
      .callback = &button_call_back_func,
      .name = "My Timer"};
    esp_timer_handle_t timer_handler;
    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 10000));
    xTaskCreate(Cyclic_Task, "Task 1", 2048, NULL, 5, NULL);
    xTaskCreate(Uncyclic_Task, "Task 2", 2048, NULL, 6, NULL);
}