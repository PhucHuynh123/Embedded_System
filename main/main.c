#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "sdkconfig.h"
#include "freertos/timers.h"
#include <time.h>

void vTimerCallback(TimerHandle_t xTimer)
{
    const uint32_t ulMaxExpiryCountBeforeStopping_timer1 = 10;
    const uint32_t ulMaxExpiryCountBeforeStopping_timer2 = 5;
    uint32_t ulCount;

    ulCount = (uint32_t)pvTimerGetTimerID(xTimer);
    ulCount++;

    if (ulCount / 100 == 1)
    {
        printf("ahihi: %d\n", (int)clock() / 1000);
        if (ulCount % 100 >= ulMaxExpiryCountBeforeStopping_timer1)
        {
            xTimerStop(xTimer, 0);
        }
    }
    else if (ulCount / 200 == 1)
    {
        printf ("ihaha: %d\n", (int)clock() / 1000);
        if (ulCount % 200 >= ulMaxExpiryCountBeforeStopping_timer2)
        {
            xTimerStop(xTimer, 0);
        }
    }
    vTimerSetTimerID(xTimer, (void*)ulCount);
}

void app_main(void)
{
    TimerHandle_t timer1s;
    TimerHandle_t timer2s;

    timer1s = xTimerCreate("Timer1", 200, pdTRUE, (void*)100, vTimerCallback);
    timer2s = xTimerCreate("Timer2", 300, pdTRUE, (void*)200, vTimerCallback);

    if (xTimerStart(timer1s, 0) != pdPASS || xTimerStart(timer2s, 0) != pdPASS)
    {
        printf("Timer start false");
    }
}