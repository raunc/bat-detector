#include <SD.h>
#include <SPI.h>
#define bufSize 4097
//FILE
File myFile;
//----------------------SPI SPEED------------------------------------
byte chipSelect=4; //selects the SS pin on board

//----------------------SD Info--------------------------------------

//----------------------BUFFERs FOR ANALOG DATA----------------------
byte buf00[bufSize]; // buffer array 1
byte buf01[bufSize]; // buffer array 2
word recByteCount = 0; // control byte to send certain buffer data

//---------------------TEST COMMAND----------------------------------
byte Test = 0b00000001;
//----------------------PROTOTYPES----------------------
void dataSend(void); //to send the data
void SerialPrepare(void);
void BDTest(byte);
void testLoadBuffer(void);
void SDPrepare(void);

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
  int t;
  int t2;
  t = micros();
  dataSend();
  t = micros()-t;
  myFile.close();
  t2 = micros()-t;
   SerialUSB.print("dataSend time2: ");
    SerialUSB.println(t);
    SerialUSB.print("myFile.close time: ");
    SerialUSB.println(t2);
  SerialUSB.println("Finished");
  while(1); //stop here
  delay(1000);
}

void SerialPrepare(void){
  SerialUSB.begin(9600);
  while (!SerialUSB) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
   SerialUSB.println("SerialUSB launched");
}

void SDPrepare(void){
  SerialUSB.print("Preparing SPI..");
  SerialUSB.println("Ready");
  SerialUSB.print("Initializing SD card..."); //Initialization message
  if (!SD.begin(chipSelect))
  { //IF ERROR IS ENCOUNTERED
    digitalWrite(13, HIGH); //LED GO ON
    SerialUSB.println("Card failed, or not present"); //UNSUCCESSFUL MESSAGE
    return;
  }
  SerialUSB.println("Card initialized.");//SUCCESSFUL MESSAGE
  if (SD.exists("example.txt")) {
    SerialUSB.println("example.txt exists."); //CHECK IF FILE EXISTS
    } 
  else {
    SerialUSB.println("example.txt doesn't exist.");
  }
  //----------------------CREATE AND OPEN A FILE ON SD CARD----------------------
  myFile = SD.open("example.txt", FILE_WRITE);
  //myFile.close(); //USE THIS TO CHECK IF EMPTY FILE CREATED
}

void dataSend(void){
   int t;
  int t2;
  SerialUSB.print("Saving to SD...");
  if (recByteCount % 2*bufSize == bufSize) { 
    t = micros();
      myFile.write(buf00,bufSize-1); // save buf01 to card
        t = micros()-t;
        SerialUSB.print("dataSend time: ");
    SerialUSB.println(t);
      //recByteSaved+= 4095; //Count how many bytes were saved
      } 
  if (recByteCount % 2*bufSize == 0) { 
      t = micros();
      myFile.write(buf01,bufSize-1); // save buf01 to card
        t = micros()-t;
        SerialUSB.print("dataSend time: ");
    SerialUSB.println(t);
      //recByteSaved+= 4095; //Count how many bytes were saved
      }
  SerialUSB.println("Saved");
}

void BDTest(byte Test){
  if((Test && 0b00000001)==1){
    SerialUSB.println("Starting to generate data");
    testLoadBuffer();
    SerialUSB.println("Finished generating");
  }
}

void testLoadBuffer(void){
  word i;
  for(i=0; i<bufSize-1; i++){
    buf01[i]=i;
    }
}
