#include <SD.h>
#include <SPI.h>
#define bufSize 4096
//FILE
File myFile;
//----------------------SPI SETTINGS---------------------------------
byte chipSelect=4; //selects the SS pin on board

//----------------------SD Info--------------------------------------
#define NUMBER_OF_TRIES 3
//----------------------FILE INFO------------------------------------
const char *name = "F";
uint32_t filenum = 0;
const char *format = ".txt";
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
void SDPrepare(void);
bool SDFlush(uint32_t);
bool SDNewOpen(void);
void SerialPrepare(void);

void setup()
{
  //----------------------USE LED AS AN ERROR INFORMER----------------------
  pinMode(13, OUTPUT);
  SerialPrepare();
  SDPrepare();
  BDTest(Test);


}

void loop()
{
  uint32_t t;
  uint32_t t1;
  uint32_t t2;
  //uint32_t i;//732422 for 3Gigs
  //for (i = 0; i < 200000; i++) {
	  if (recByteCount % 2 * bufSize == bufSize) {
		  t = micros();
		  myFile.write(buf00, bufSize); // save buf01 to card
		  t = micros()-t;
	  }
	  if (!SDFlush(t)) {
		  SerialUSB.println("Error reopening file");
		  return;
	  }
	  t2 = micros();
	  if (recByteCount % 2 * bufSize == 0) {
		  t1 = micros();
		  myFile.write(buf01, bufSize); // save buf01 to card
		  t1 = micros()-t1;
	  }
	  if (!SDFlush(t1)) {
		  SerialUSB.println("Error reopening file");
		  return;
	  }
	  t2 = micros() - t2;
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

void SDPrepare(void){
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
  if (!SDNewOpen()) {
	  SerialUSB.println("Error opening new file");
	  return;
  }
  SerialUSB.println("File opened.");
}

/*Each time when the sending time is larger than ~350ms the file is reopened. 
It is needed to confirm saved data which was sent before. Otherwise, if the controller stops working for some reason
the stored data will be lost.*/
bool SDFlush(uint32_t t)
{
	if (t > 350000) {
		SerialUSB.println("SDFlush");
		myFile.flush();
			}
	return 1;
}

/*Planning to use this function to create another file when the data was recorded for ~1 hour*/
bool SDNewOpen(void) {
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
