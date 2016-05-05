#include <SD.h>
#include <SPI.h>

#include "adc-dma.h"
#include "sd.h"


#define ADC_PIN A1
#define CHIP_SELECT_SD 4
#define BUFFER_SIZE 4096

// TODO: investigate why ADC readings are not from 0 TO MAX // Depends on gain and reference settings // Check also calibration in datasheet

// Buffer Statuses
#define BUFFER_EMPTY 0
#define BUFFER_FULL 1
#define BUFFER_ADC_RUNNING 2

/* GLOBAL VARIABLE DEFINITIONS */
// ADC buffers
uint8_t adcBuffer0[BUFFER_SIZE];
uint8_t adcBuffer1[BUFFER_SIZE];

uint8_t adcBuffer0Status = BUFFER_EMPTY;
uint8_t adcBuffer1Status = BUFFER_EMPTY;

// File on SD card
File sdFile;


/* Function declarations */
void handleBufferFull(void);
void handleBufferWrittenToSDCard(uint8_t* buffer);


void setup() {

  // Configuring SerialUSB only for debugging purposes
  // TODO: remove from final release or make it excludable with compiler arguments
  SerialUSB.begin(115200);
  while (!SerialUSB);

  // Initializing ADC
  adc_init(ADC_PIN, 63);
  dma_init();
  setADCDoneHandler(&handleBufferFull);

  // Initializing SD card
  pinMode(13, OUTPUT);// For using LED to indicate SD card errors
  SDPrepare(sdFile, CHIP_SELECT_SD);

  // Fill one ADC buffer
  adcBuffer0Status = BUFFER_ADC_RUNNING;
  adc_dma(adcBuffer0, BUFFER_SIZE);
}

void loop() {
}

void handleBufferFull(void) {
//  SerialUSB.println("handleBufferFull");
  uint8_t* buffer;
  uint8_t* bufferStatus;
  uint8_t* otherBuffer;
  uint8_t* otherBufferStatus;

  if (adcBuffer0Status == BUFFER_ADC_RUNNING) {
    // Buffer 0 was running
    buffer = adcBuffer0;
    bufferStatus = &adcBuffer0Status;
    otherBuffer = adcBuffer1;
    otherBufferStatus = &adcBuffer1Status;
  } else if (adcBuffer1Status == BUFFER_ADC_RUNNING) {
    // Buffer 1 was running
    buffer = adcBuffer1;
    bufferStatus = &adcBuffer1Status;
    otherBuffer = adcBuffer0;
    otherBufferStatus = &adcBuffer0Status;
  } else {
    // Should never get here
    SerialUSB.println("Something went wrong with adc buffer statuses");
    return;
  }

//  SerialUSB.print("Buffer statuses ");
//  SerialUSB.print(adcBuffer0Status);
//  SerialUSB.println(adcBuffer1Status);
  (*bufferStatus) = BUFFER_FULL;

  // For debugging:
/*  for (uint32_t i = 0; i < 10; ++i) {
    SerialUSB.print(buffer[i]);
    SerialUSB.print(" ");
  }
  SerialUSB.println();*/


  if ((*otherBufferStatus) == BUFFER_EMPTY) {
    // Start filling other buffer and write this buffer to SD card
    (*otherBufferStatus) = BUFFER_ADC_RUNNING;
    adc_dma(otherBuffer, BUFFER_SIZE);

    writeBufferToSD(buffer, BUFFER_SIZE);
  }
}

void handleBufferWrittenToSDCard(uint8_t* buffer) {
//  SerialUSB.println("handleBufferWrittenToSDCard");
  uint8_t* otherBuffer;
  uint8_t* otherBufferStatus;
  uint8_t* bufferStatus;

  if (buffer == adcBuffer0) {
    bufferStatus = &adcBuffer0Status;
    otherBuffer = adcBuffer1;
    otherBufferStatus = &adcBuffer1Status;
  } else {// buffer == &adcBuffer1
    bufferStatus = &adcBuffer1Status;
    otherBuffer = adcBuffer0;
    otherBufferStatus = &adcBuffer0Status;
  }

  (*bufferStatus) = BUFFER_EMPTY;

  if ((*otherBufferStatus) == BUFFER_FULL) {
    // Start filling this buffer and save other buffer to SD card
    (*bufferStatus) = BUFFER_ADC_RUNNING;
    adc_dma(buffer, BUFFER_SIZE);

    writeBufferToSD(otherBuffer, BUFFER_SIZE);
  }
}

void writeBufferToSD(uint8_t* buffer, size_t size) {
//  SerialUSB.println("writeBufferToSD");
  uint32_t t2 = micros();

  uint32_t t1 = micros();
  uint16_t bytesWritten = sdFile.write(buffer, size);// Save buffer to card
  t1 = micros() - t1;
//  SerialUSB.println("bytesWritten, t1: ");
//  SerialUSB.println(bytesWritten);
//  SerialUSB.println(t1);// Print for debugging
  SDFlush(sdFile, t1);

  t2 = micros() - t2;
//  SDReCheck(sdFile, CHIP_SELECT_SD, t1, t1);

//  SerialUSB.println(t2);// Print for debugging

  // Buffer is now written to SD card. Check other buffer status
  handleBufferWrittenToSDCard(buffer);
}
