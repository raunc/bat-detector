#ifndef ADC_DMA_H_
#define ADC_DMA_H_

// Arduino header for board (includes also ASF headers)
#include <Arduino.h>

extern volatile uint32_t adc_done;
typedef void (*fnPtr)(void);

void setADCDoneHandler(fnPtr);
void adc_init(const uint8_t ADC_PIN, const uint8_t samplingSpeed);// samplingSpeed - number [0, 63], lower is faster. Setting too low can cause problems
void dma_init();
void adc_dma(void *rxdata, size_t hwords);

#endif
