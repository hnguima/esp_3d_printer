#include "pwm.h"

static const char *TAG = "pwm";

static void pwm_task(void *param)
{

    pwm_data_t *pwm = (pwm_data_t *)param;

    while (1)
    {
        if (pwm->duty_cycle > 0)
        {
            pwm->on_func(pwm->param);
            vTaskDelay(((pwm->duty_cycle / 100.0f) * pwm->period) / portTICK_PERIOD_MS);
        }

        if (pwm->duty_cycle < 100)
        {
            pwm->off_func(pwm->param);
            vTaskDelay((((100 - pwm->duty_cycle) / 100.0f) * pwm->period) / portTICK_PERIOD_MS);
        }
    }
}

esp_err_t pwm_init(pwm_data_t *pwm)
{
    if (pwm->duty_cycle > 100)
    {
        return ESP_FAIL;
    }

    if (pwm->on_func == NULL || pwm->off_func == NULL)
    {
        return ESP_FAIL;
    }

    xTaskCreate(pwm_task, "pwm_task", 2048, pwm, 5, NULL);

    return ESP_OK;
}

// uint16_t therm_convert(int adc_mv)
// {
//     int res_therm = (RES_DIV * (VOLT_THERM - out)) / out;

//     float temp = BETA_THERM / ((BETA_THERM / T0_THERM) + log(res_therm / R0_THERM));

//     ESP_LOGI(TAG, "temp %d: %.1f deg", channel, temp - 273.15);

//     return out;
// }