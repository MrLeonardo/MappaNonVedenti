#include <pcmRF.h>
#include <pcmConfig.h>

#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include <avr/wdt.h>

#include <TMRpcm.h>

/*
TMRpcm audio;
 audio.play("filename");    plays a file
 audio.play("filename",30); plays a file starting at 30 seconds into the track
 audio.speakerPin = 11;     set to 5,6,11 or 46 for Mega, 9 for Uno, Nano, etc.
 audio.disable();           disables the timer on output pin and stops the music
 audio.stopPlayback();      stops the music, but leaves the timer running
 audio.isPlaying();         returns 1 if music playing, 0 if not
 audio.pause();             pauses/unpauses playback
 audio.quality(1);          Set 1 for 2x oversampling
 audio.volume(0);           1(up) or 0(down) to control volume
 audio.setVolume(0);        0 to 7. Set volume level
 audio.loop(1);             0 or 1. Can be changed during playback for full control of looping. 
 */

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

TMRpcm audio;

Sd2Card card;
SdVolume volume;
SdFile root;

void setup() {
  wdt_enable(WDTO_8S);
  Serial.begin(9600);  
  Wire.begin();
  openSD();  
  setAudio();
}

void loop() {
  int pin = 0;
  int address;
  int i = 0;
  
  if(!audio.isPlaying()) {
    
    Serial.println("not play");
    
    int numberOfAddress = (sizeof(ArrayOfPCF_address)/sizeof(int));
    
    for(i = 0; i < numberOfAddress && pin == 0; i++) {      
      pin = readPin(ArrayOfPCF_address[i]);
      address = ArrayOfPCF_address[i];
    }    
    
    if(address == PCF_address_A4 && pin == 128) {
      //recordAudio(address+"_"+pin);
      Serial.println("record pin: " + String(pin));
    }
    else {
      if(pin > 0) {
        Serial.println("pin: " + String(pin) + " " + String(address));
        //playAudio(String(address) + "_" + String(pin) + ".wav");
        playAudio("60_1.wav");        
      }
    }
      
  }
  else {
    Serial.println("play");
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
  audio.speakerPin = speakerPin;
  audio.setVolume(setVolume);
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

File readSDCard(String namefile) {  
  return SD.open(namefile.c_str(), FILE_READ);;
}

void playAudio(String filename) {
  File file;
  char *buf;       //[BUFFER_SIZE_FILENAME];
  
  filename = "60_1.wav";
  if((file = readSDCard(filename))) {
    Serial.println("file open: " + filename);
    file.close();
    filename.toCharArray(buf, filename.length());
    Serial.println("filename length: " + String(filename.length()));
    
    int i = 0;
    for(i = 0; i < filename.length(); i++) {
      buf[i] = filename[i];      
    }
    audio.play("60_1.WAV");
  } 
}

// ------------------------------------




