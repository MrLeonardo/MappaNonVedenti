//Librerie per SD
#include <SD.h>
#include <SPI.h>

#include <Wire.h>

//Libreria per text-to-speech
#include <TTS.h>

#include <string.h>
#include <avr/wdt.h>

#define BUFFER_SIZE_FILENAME 30

TTS text2speech;

const int sdPin = 10;

const int PCF_address_A0 = B0111000;
const int PCF_address_A1 = B0111001;
const int PCF_address_A2 = B0111010;
const int PCF_address_A3 = B0111011;
const int PCF_address_A4 = B0111100;

int ArrayOfPCF_address[] = {B0111000, B0111001, B0111010, B0111011, B0111100};

struct WIRE {
  int pin;    
  int address;
};

void setup() {
  Serial.begin(9600); 
  Wire.begin();
  openSD();  
  wdt_enable(WDTO_8S);
}

void loop() {  
  WIRE wire = readFromAddress(true);    
  if(wire.address == PCF_address_A4 && wire.pin == 128) {      
    //recordAudio();
  }
  else {
    if(wire.pin > 0) {
      String filename = String(wire.address) + "_" + String(wire.pin) + ".txt";
      Serial.println("play filename: " + filename);
      String text = readTextFromFile(filename);
      playText(text);
    }
  }   
  
  wdt_reset();
}

// -------------- setting --------------

void openSD() {
  Serial.print("Initializing SD card...");
  while (!SD.begin(sdPin)) {
    Serial.println(" failed!");
  }
  Serial.println(" done.");
}

// -------------------------------------

// ------------- function --------------
byte readPin(int PCF_address) {
  int n = 0;
  Wire.requestFrom(PCF_address, 1);
  while(Wire.available()) {    
    n += (Wire.read());
  }
  return ~n;
}

File readSDCard(char* namefile) {  
  return SD.open(namefile, FILE_READ);
}

String readTextFromFile(String namefile) { 
  String text = "";
  File myFile = SD.open(namefile, FILE_READ);
  if (myFile) {
    Serial.println("test.txt:");    
    while (myFile.available()) {
      text += myFile.read();
      Serial.println(text);
    }
    myFile.close();
  } else {
    Serial.println("error opening " + namefile);
  }

  return text;
}
   
struct WIRE readFromAddress(boolean all) {
  WIRE wire;
  int i = 0;
  
  wire.pin = 0;
  wire.address = 0;
  
  int numberOfAddress = (sizeof(ArrayOfPCF_address)/sizeof(int));
  
  if(!all)
    numberOfAddress -= 1;
    
  for(i = 0; i < numberOfAddress && wire.pin == 0; i++) {      
    wire.pin = readPin(ArrayOfPCF_address[i]);
    wire.address = ArrayOfPCF_address[i];
  }
  
  if(wire.pin == 0)
    wire.address = 0;
  
  return (wire);
}

void playText(String &text) {
  text2speech.setPitch(6);
  char* buf = (char*)text.c_str();
  text2speech.sayText(buf);
}

// ------------------------------------




