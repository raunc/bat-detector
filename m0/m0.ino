#include "adc-dma.h"

#define DAC_PIN A0
#define ADC_PIN A1


// ADC TODO list
// TODO: ADC should have two buffers
// TODO: ADC should have a callback function when finished. (currently polling adc_done variable)
// TODO: investigate why ADC readings are not from 0 TO MAX

// ADC buffer length
#define ADCBufferLength 4096

/* GLOBAL VARIABLE DEFINITIONS */
uint8_t ADCBuffer[ADCBufferLength];


void setup() {

  // Configuring SerialUSB only for debugging purposes
  // TODO: remove from final release or make it excludable with compiler arguments
  SerialUSB.begin(115200);
  while (!SerialUSB);

  adc_init(ADC_PIN);
  dma_init();

  delay(3000);
  SerialUSB.println(adc_done);
}

void loop() {

  /* TODO: The whole contents in loop() are for debbugging purposes. Should be changed for final application */

  uint32_t t;
  t = micros();

  adc_dma(ADCBuffer, ADCBufferLength);
  while(!adc_done);  // await DMA done isr

  t = micros() - t;

  SerialUSB.print(t);
  SerialUSB.println(" us: ");

  for (int i = 0; i < 10; ++i) {
    SerialUSB.print(ADCBuffer[i]);
    SerialUSB.print(' ');
  }
  SerialUSB.println("");

  delay(1000);
}
