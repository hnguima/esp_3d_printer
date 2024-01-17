#ifndef __PID___H
#define __PID___H

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


#define LIMIT(value, max, min)                   \
    value = ((value) > (max) ? (max) : (value)); \
    value = ((value) < (min) ? (min) : (value))

typedef struct
{
    /* Controller gains */
    float kp, ki, kd;

    /* Derivative low-pass filter time constant */
    float tau;

    /* Output limits */
    float out_min, out_max;

    /* Integrator limits */
    float int_min, int_max;

    /* Sample time (in seconds) */
    float period;

    /* Controller "memory" */
    float prop_val;
    float int_val;
    float last_err; /* Required for integrator */
    float diff_val;
    float last_meas; /* Required for differentiator */

    float err, setpoint, meas;

    /* Controller output */
    float out;

    void *sensor;
    void *controller;

    float (*measure)(void *sensor);
    void (*control)(void *controller, float control);

} pid_ctrl_t;

void pid_init(pid_ctrl_t *pid);

#ifdef __cplusplus
}
#endif 


#endif /* __PID___H */