#include <SD.h>
#include <SPI.h>

#include "adc-dma.h"
#include "sd.h"


#define ADC_PIN A1
#define CHIP_SELECT_SD 4
#define BUFFER_SIZE 4096

// ADC TODO list
// TODO: ADC should have two buffers
// TODO: ADC should have a callback function when finished. (currently polling adc_done variable)
// TODO: investigate why ADC readings are not from 0 TO MAX // Depends on gain and reference settings // Check also calibration in datasheet
// TODO: Make sampling frequency configurable // Use automatic trigger instead of freerun?


/* GLOBAL VARIABLE DEFINITIONS */
// ADC buffers
uint8_t adcBuffer0[BUFFER_SIZE];
uint8_t adcBuffer1[BUFFER_SIZE];

// File on SD card
File sdFile;

word recByteCount = 0;// control byte to send certain buffer data


void setup() {

  // Configuring SerialUSB only for debugging purposes
  // TODO: remove from final release or make it excludable with compiler arguments
  SerialUSB.begin(115200);
  while (!SerialUSB);

  // Initializing ADC
  adc_init(ADC_PIN);
  dma_init();

  // Initializing SD card
  pinMode(13, OUTPUT);// For using LED to indicate SD card errors
  SDPrepare(sdFile, CHIP_SELECT_SD);


  // Fill one buffer here. It just simplifies code in loop function
  adc_dma(adcBuffer1, BUFFER_SIZE);
  while(!adc_done);
}

void loop() {

  // Start filling buffer 0
  adc_dma(adcBuffer0, BUFFER_SIZE);

  // Save buffer 1 to SD card
  writeBufferToSD(adcBuffer1, BUFFER_SIZE);

  // Wait until buffer 0 is ready
  while (!adc_done);// TODO: Should be solved using interrupts

  // Start filling buffer 1
  adc_dma(adcBuffer1, BUFFER_SIZE);

  // Save buffer 0 to SD card
  writeBufferToSD(adcBuffer0, BUFFER_SIZE);

  // Wait until buffer 1 is ready
  while (!adc_done);
}

void writeBufferToSD(uint8_t* buffer, size_t size) {
  uint32_t t2 = micros();

  uint32_t t1 = micros();
  sdFile.write(buffer, size);// Save adcBuffer1 to card
  t1 = micros() - t1;
  SDFlush(sdFile, /*t1*/ 1e6);// Added 1e6 to flush every time for testing

  t2 = micros() - t2;
  SDReCheck(sdFile, CHIP_SELECT_SD, t1, t1);

  SerialUSB.println(t2);// Print for debugging
}
