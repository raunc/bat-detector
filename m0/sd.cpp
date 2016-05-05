
#include <SD.h>
#include <SPI.h>
#include "sd.h"

#define NUMBER_OF_TRIES 3

//----------------------FILE INFO------------------------------------
const char *name = "F";
uint32_t filenum = 0;
const char *format = ".txt";

void SDPrepare(File &file, uint8_t chipSelect){
	bool Ready;
	uint8_t i;
  SerialUSB.println("12MHz SPI Ready");
  SerialUSB.print("Initializing SD card..."); //Initialization message
  for(i =0; (i<NUMBER_OF_TRIES)&&(!SD.begin(chipSelect));i++)
  //while (!SD.begin(chipSelect))
  { //IF ERROR IS ENCOUNTERED
    SerialUSB.println("Card failed, or not present"); //UNSUCCESSFUL MESSAGE
	delay(100);
  }
  if (i == NUMBER_OF_TRIES) {
	  while (1) {
		  digitalWrite(13, HIGH);
		  delay(500);
		  digitalWrite(13, LOW);
		  delay(500);
	  }
  }
  SerialUSB.println("Card initialized.");//SUCCESSFUL MESSAGE
  //----------------------CREATE AND OPEN A FILE ON SD CARD----------------------
  SerialUSB.println("Opening file.");
  //Ready = SDNewOpen();
  if (!SDNewOpen(file)) {
	  SerialUSB.println("Error opening new file");
	  return;
  }
  SerialUSB.println("File opened.");
}

/*Each time when the sending time is larger than ~350ms the file is reopened.
It is needed to confirm saved data which was sent before. Otherwise, if the controller stops working for some reason
the stored data will be lost.*/
bool SDFlush(File &myFile, uint32_t t)
{
	if (t > 150000) {
//		SerialUSB.println("SDFlush");
		myFile.flush();
			}
}

/*Planning to use this function to create another file when the data was recorded for ~1 hour*/
bool SDNewOpen(File &myFile) {
	SerialUSB.println("SDNewOpen");
	char buffer[30];
	filenum++;
	if (filenum != 1) {
		myFile.close();
	}
	sprintf(buffer, "%s%d%s", name, filenum, format);
	SerialUSB.println(buffer);
	while (SD.exists(buffer)) {
		filenum++;
		sprintf(buffer, "%s%d%s", name, filenum, format);
		SerialUSB.println(buffer);
	}
	if ((myFile = SD.open(buffer, FILE_WRITE)) == 0) {
			return 0;
	}
	return 1;
}

void SDReCheck(File &file, uint8_t chipSelect, uint32_t t, uint32_t t1) {
	//SerialUSB.println("SDReCheck");
	if ((t < 1000) || (t1 < 1000)) {
		SerialUSB.println("SDReCheck: prepare");
		SDPrepare(file, chipSelect);
	}
}
