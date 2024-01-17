#ifndef __PWM___H
#define __PWM___H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

// Log
#include "esp_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    uint32_t period; //ms
    uint8_t duty_cycle; // 0 - 100 percent

    void *param;

    void (*on_func)(void *param);
    void (*off_func)(void *param);

} pwm_data_t;

esp_err_t pwm_init(pwm_data_t *pwm);

#ifdef __cplusplus
}
#endif 

#endif /* __PWM___H */