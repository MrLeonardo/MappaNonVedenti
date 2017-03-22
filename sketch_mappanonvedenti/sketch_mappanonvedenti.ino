#include <pcmRF.h>
#include <pcmConfig.h>

#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include <string.h>

#include <avr/wdt.h>

#include <TMRpcm.h>

#define BUFFER_SIZE_FILENAME 30

const int speakerPin = 9;
const int setVolume = 6;

const int sdPin = 10;

const int PCF_address_A0 = B0111000;
const int PCF_address_A1 = B0111001;
const int PCF_address_A2 = B0111010;
const int PCF_address_A3 = B0111011;
const int PCF_address_A4 = B0111100;

int ArrayOfPCF_address[] = {B0111000, B0111001, B0111010, B0111011, B0111100};

TMRpcm tmrpcm;

Sd2Card card;
SdVolume volume;
SdFile root;

struct WIRE {
  int pin;    
  int address;
};

void setup() {
  wdt_enable(WDTO_8S);
  Serial.begin(9600);  
  Wire.begin();
  openSD();  
  setAudio();
  tmrpcm.CSPin = sdPin;
}

void loop() {  
  if(!tmrpcm.isPlaying()) {        
    WIRE wire = readFromAddress(true);    
    if(wire.address == PCF_address_A4 && wire.pin == 128) {      
      recordAudio();
    }
    else {
      if(wire.pin > 0) {
        Serial.println("play filename: " + String(wire.address) + "_" + String(wire.pin) + ".wav");
        String str = String(wire.address) + "_" + String(wire.pin) + ".wav";
        playAudio(str);        
      }
    }      
  }
  else {
    //Serial.println("play");
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

void setAudio() {
  tmrpcm.speakerPin = speakerPin;
  //tmrpcm.setVolume(setVolume);
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
  return SD.open(namefile, FILE_READ);;
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

void playAudio(String &filename) {
  char* buf = (char*)filename.c_str();
  tmrpcm.play(buf);
}

void recordAudio() {  
  while (readPin(PCF_address_A4) == 128) {
    WIRE wire = readFromAddress(false);
    
    Serial.println("record pin: " + String(wire.pin));
    
    if(wire.pin > 0) {
      String filename = String(wire.address) + "_" + String(wire.pin) + ".wav";
      Serial.println("record filename: " + filename);    
      char* buf = (char*)filename.c_str();
      tmrpcm.startRecording(buf,16000, A0);  
     
      int pin = wire.pin;
    
      while(pin == wire.pin) {
        wire = readFromAddress(false);
        wdt_reset();
      } 
      
      Serial.println("stop");
      tmrpcm.stopRecording(buf); 
    }
    
    wdt_reset();
  }
}
// ------------------------------------




