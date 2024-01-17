#include "pid.h"

static const char *TAG = "pid";

static void pid_task(void *param)
{

    pid_ctrl_t *pid = (pid_ctrl_t *)param;

    while (1)
    {

        pid->meas = pid->measure(pid->sensor);
        pid->err = pid->setpoint - pid->meas;

        /*
         * Proportional
         */
        pid->prop_val = pid->kp * pid->err;

        /*
         * Integral
         */
        pid->int_val = pid->int_val + 0.5f * pid->ki * pid->period * (pid->err + pid->last_err);

        /* Anti-wind-up via int_val clamping */
        LIMIT(pid->int_val, pid->int_max, pid->int_min);

        /*
         * Derivative (band-limited differentiator)
         */

        pid->diff_val = -(2.0f * pid->kd * (pid->meas - pid->last_meas) /* Note: derivative on measurement, therefore minus sign in front of equation! */
                          + (2.0f * pid->tau - pid->period) * pid->diff_val) /
                        (2.0f * pid->tau + pid->period);

        /*
         * Compute output and apply limits
         */
        pid->out = pid->prop_val + pid->int_val + pid->diff_val;

        LIMIT(pid->out, pid->out_max, pid->out_min);

        /* Store error and measurement for later use */
        pid->last_err = pid->err;
        pid->last_meas = pid->meas;

        /* Return controller output */
        // printf("%f, %f, %f\n", pid->meas, pid->setpoint, pid->out);
        pid->control(pid->controller, pid->out);

        vTaskDelay((pid->period * 1000) / portTICK_PERIOD_MS);
    }
}

void pid_init(pid_ctrl_t *pid)
{

    pid->int_val = 0.0f;
    pid->last_err = 0.0f;

    pid->diff_val = 0.0f;
    pid->last_meas = 0.0f;

    pid->err = 0.0f;
    pid->out = 0.0f;

    // create pid task
    xTaskCreate(pid_task, "pid_task", 2048, pid, 5, NULL);
}