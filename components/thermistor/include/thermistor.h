#ifndef __THERMISTOR___H
#define __THERMISTOR___H

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

// ADC
#include "adc.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define THERMISTOR_MAX_BUFFER_SIZE 128

#define RES_DIV 10000.0
#define VOLT_THERM 3240.0
#define R0_THERM 100000.0
#define T0_THERM (25 + 273.15)
#define BETA_THERM 3950.0

#define THERMISTOR_DEFAULT_CONFIG {\
    .adc_ch = 0,\
    .rd = RES_DIV,\
    .v0 = VOLT_THERM,\
    .r0 = R0_THERM,\
    .t0 = T0_THERM,\
    .beta = BETA_THERM\
}

typedef struct
{
    uint8_t adc_ch;

    double rd;
    double v0;
    double r0;
    double t0;
    double beta;

    double temp;
    double adc_val;

    double buffer[THERMISTOR_MAX_BUFFER_SIZE];
    int buf_head;
    int buf_tail;
    int buf_size;
} therm_t;

void therm_init(therm_t *therm);
double therm_convert(therm_t *therm);
double therm_get_temp(therm_t *therm);

#ifdef __cplusplus
}
#endif 


#endif /* __THERMISTOR___H */