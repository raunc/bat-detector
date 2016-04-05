#ifndef ADC_DMA_H_
#define ADC_DMA_H_

// Arduino header for board (includes also ASF headers)
#include <Arduino.h>

extern volatile uint32_t adc_done;

void adc_init(const uint8_t ADC_PIN);
void dma_init();
void adc_dma(void *rxdata,  size_t hwords);

#endif
