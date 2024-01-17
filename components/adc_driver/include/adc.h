#ifndef __ADC___H
#define __ADC___H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

// Log
#include "esp_log.h"

// ESP-IDF
#include "driver/adc.h"
#include "esp_adc_cal.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define ADC_UNIT ADC_UNIT_2
#define ADC_WIDTH ADC_WIDTH_BIT_12
#define ADC_ATTEN ADC_ATTEN_DB_11
#define ADC_SAMPLES 500

#define ADC_1_CH ADC2_CHANNEL_8
#define ADC_2_CH ADC2_CHANNEL_9
#define ADC_3_CH ADC2_CHANNEL_1
#define ADC_4_CH ADC2_CHANNEL_3

static const uint8_t ADC_CH[] = {ADC_1_CH, ADC_2_CH, ADC_3_CH, ADC_4_CH};

static uint16_t adc_mv[4];

void adc_init();
float adc_get(uint8_t channel);

#ifdef __cplusplus
}
#endif 
#endif /* __ADC___H */