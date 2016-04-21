#include <SD.h>
#include <SPI.h>
#include "sd.h"

#define bufSize 4096
//FILE
File myFile;
//----------------------SPI SETTINGS---------------------------------
byte chipSelect=4; //selects the SS pin on board

//----------------------SD Info--------------------------------------

//----------------------BUFFERs FOR ANALOG DATA----------------------
byte buf00[bufSize]; // buffer array 1
byte buf01[bufSize]; // buffer array 2
word recByteCount = 0; // control byte to send certain buffer data
//byte recByteSends = 0;

//---------------------TEST COMMAND----------------------------------
byte Test = 0b00000001;
//----------------------PROTOTYPES----------------------
//void dataSend(void); //to send the data (REMOVED)
void BDTest(byte);
void testLoadBuffer(void);
void SerialPrepare(void);

void setup()
{
  //----------------------USE LED AS AN ERROR INFORMER----------------------
  pinMode(13, OUTPUT);
  SerialPrepare();
  SDPrepare(myFile, chipSelect);
  BDTest(Test);


}

void loop()
{
  uint32_t t=1000;
  uint32_t t1=1000;
  uint32_t t2;
  //uint32_t i;//732422 for 3Gigs
  //for (i = 0; i < 200000; i++) {
	  if (recByteCount % 2 * bufSize == bufSize) {
		  t = micros();
		  myFile.write(buf00, bufSize); // save buf01 to card
		  t = micros()-t;
	  }
	  SDFlush(myFile, t);
	  t2 = micros();
	  if (recByteCount % 2 * bufSize == 0) {
		  t1 = micros();
		  myFile.write(buf01, bufSize); // save buf01 to card
		  t1 = micros()-t1;
	  }
	  SDFlush(myFile, t1);
	  t2 = micros() - t2;
	  SDReCheck(myFile, chipSelect, t, t1);
	  //SerialUSB.print(i); //For tests
	  //SerialUSB.print(". ");
	  SerialUSB.println(t2);
  /*}
  myFile.close();
  SerialUSB.println("Finished");
  while(1); //stop here*/
}

void SerialPrepare(void){
  SerialUSB.begin(115200);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
   SerialUSB.println("SerialUSB launched");
}

void BDTest(byte Test){
  if((Test && 0b00000001)==1){
    //SerialUSB.println("Starting to generate data");
    testLoadBuffer();
    //SerialUSB.println("Finished generating");
  }
}

void testLoadBuffer(void){
  word i;
  for (i = 0; i<bufSize; i++) {
	  buf00[i] = i;
  }
  for(i=0; i<bufSize; i++){
    buf01[i]=i;
    }
}
