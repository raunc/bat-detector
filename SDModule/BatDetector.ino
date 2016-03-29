#include <SD.h>
#include <SPI.h>
//----------------------SPI SPEED SETTINGS----------------------
SPISettings settingsA(2000000, MSBFIRST, SPI_MODE0); // 2 MHz clock, MSB first, mode 0
SPISettings settingsB(4000000, MSBFIRST, SPI_MODE0); // 4 MHz clock, MSB first, mode 0
SPISettings settingsC(8000000, MSBFIRST, SPI_MODE0); // 8 MHz clock, MSB first, mode 0
SPISettings settingsD(16000000, MSBFIRST, SPI_MODE0); // 16 MHz clock, MSB first, mode 0
SPISettings settingsE(20000000, MSBFIRST, SPI_MODE0); // 20 MHz clock, MSB first, mode 0 (SD!)
//FILE
File myFile;
//----------------------BUFFERs FOR ANALOG DATA----------------------
byte buf00[256]; // buffer array 1
byte buf01[256]; // buffer array 2
byte controlbuf0x = 0; // control byte to send certain buffer data
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
 
 


}

void loop()
{
  SPI.beginTransaction (settingsB);  // 20 MHz clock, MSB first, mode 0
  BDTest(Test);
  dataSend(controlbuf0x);
  ReadData();
  myFile.close();
  Serial.println("Finished");
  while(1); //stop here
}

void SerialPrepare(void){
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void SDPrepare(void){
  Serial.print("Initializing SD card..."); //Initialization message
  if (!SD.begin(4)) { //IF ERROR IS ENCOUNTERED
    digitalWrite(13, HIGH); //LED GO ON
    Serial.println("Card failed, or not present"); //UNSUCCESSFUL MESSAGE
    return;
  }
  Serial.println("Card initialized.");//SUCCESSFUL MESSAGE
  if (SD.exists("example.txt")) {
    Serial.println("example.txt exists."); //CHECK IF FILE EXISTS
    } 
  else {
    Serial.println("example.txt doesn't exist.");
  }
  //----------------------CREATE AND OPEN A FILE ON SD CARD----------------------
  myFile = SD.open("example.txt", FILE_WRITE);
  //myFile.close(); //USE THIS TO CHECK IF EMPTY FILE CREATED
}

void dataSend(byte controlbuf0x){
  Serial.println("Saving to SD...");
  if (myFile) {
    switch(controlbuf0x){
      case 0: 
      Serial.println("Saving buff00");
        myFile.write(buf00,0b11111111);
        break;
      case 1:
      Serial.println("Saving buff01"); 
        myFile.write(buf01,0b11111111);
        break;
      default:
      digitalWrite(13, HIGH);
      break;
      }
    //myFile.close();
    // print to the serial port too:
  }
  else{
    digitalWrite(13, HIGH);
    Serial.println("error opening example.txt");
  }
}

void BDTest(byte Test){
  if((Test && 0b00000001)==1){
    Serial.println("Starting to generate data");
    testLoadBuffer();
    Serial.println("Finished generating");
  }
  /*if((Test && 0b00000010)==1){
  }
  if((Test && 0b00000100)==1){
  }
  if((Test && 0b00001000)==1){
  }
  if((Test && 0b00010000)==1){
  }
  if((Test && 0b00100000)==1){
  }
  if((Test && 0b01000000)==1){
  }
  if((Test && 0b10000000)==1){
  }*/
}

void testLoadBuffer(void){
  byte i;
  for(i=0b00000000; i<0b11111111; i++){
    buf00[i]=i;
    Serial.print(i);
    Serial.println("");
    }
}

void ReadData(void){
if (myFile) {
    Serial.println("example.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
