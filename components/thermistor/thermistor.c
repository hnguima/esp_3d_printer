#include "thermistor.h"

static const char *TAG = "therm";

void therm_task(void *param)
{
    therm_t *therm = (therm_t *)param;

    while (true)
    {
        therm->buffer[therm->buf_head++] = therm_convert(therm);
        therm->buf_head %= THERMISTOR_MAX_BUFFER_SIZE;

        therm->buf_size = (therm->buf_size + 1);
        if (therm->buf_size >= THERMISTOR_MAX_BUFFER_SIZE)
        {
            therm->buf_size = THERMISTOR_MAX_BUFFER_SIZE;
        }

        therm->buf_tail = ((therm->buf_head - therm->buf_size) + THERMISTOR_MAX_BUFFER_SIZE) % THERMISTOR_MAX_BUFFER_SIZE;

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void therm_init(therm_t *therm)
{
    therm->buf_head = 0;
    therm->buf_tail = 0;
    therm->buf_size = 0;

    xTaskCreate(therm_task, "therm_task", 2048, therm, 5, NULL);
}

double therm_convert(therm_t *therm)
{
    // printf("%f\n", adc_get(therm->adc_ch));
    double adc_volt = adc_get(therm->adc_ch);
    // printf("%d: %lf\n", therm->adc_ch, adc_volt);

    double res_therm = (therm->rd * (therm->v0 - adc_volt)) / adc_volt;
    double temperature = therm->beta / ((therm->beta / therm->t0) + log(res_therm / therm->r0)) - 273.15;
    
    // float res_therm = (therm->rd * (therm->v0 - therm->adc_val)) / therm->adc_val;
    // therm->temp = therm->beta / ((therm->beta / therm->t0) + log(res_therm / therm->r0)) - 273.15;
    
    return temperature;
}

double therm_get_temp(therm_t *therm)
{
    double out = 0;

    for (uint16_t i = 0; i < therm->buf_size; i++)
    {
        out += therm->buffer[(therm->buf_tail + i) % THERMISTOR_MAX_BUFFER_SIZE];
    }

    return out / therm->buf_size;
}