#include <pcmRF.h>
#include <pcmConfig.h>

#include <SD.h>
#include <SPI.h>

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

const int leakPin = 3;
const int clockPin = 3;
const int readButtonPin = 3;

TMRpcm audio;

Sd2Card card;
SdVolume volume;
SdFile root;

void setup() {

  wdt_enable(WDTO_8S);

  Serial.begin(9600);  

  openSD();

  setAudio();

  setPin();
}

void loop() {
  int pin = 0; 
  int pinForSet = 0;
  
  if(!audio.isPlaying()) {
    digitalWrite(leakPin, HIGH);
    pin = readPin(30);
    
    if (pin != 0) {
      pinForSet = readPin(1);
      
      if(pinForSet != 0) {
        //registerWav();
      }
      else {
        Serial.println("pin read: " + String(pin));	
        playAudio(pin);
      }
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

void setPin() {
  pinMode(leakPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(readButtonPin, INPUT);
}

void setAudio() {
  audio.speakerPin = speakerPin;
  audio.setVolume(setVolume);
}

// -------------------------------------

// ------------- function --------------
int readPin(int numberOfPin) {
  int i;
  int pin = 0;
  int val = LOW;
  
  for(i = 1; i <= numberOfPin; i++) {
    digitalWrite(clockPin, HIGH);
    val = digitalRead(readButtonPin);
    if (val == HIGH and pin == 0) {
      pin = i;
    }    
  }
  
  return pin;
}

File readSDCard(String namefile) {
  return SD.open(namefile.c_str(),FILE_READ);
}

void playAudio(int pin) {
  File file;
  String filename = "";
  char *buf;       //[BUFFER_SIZE_FILENAME];


  /*
  //Make the track an int to inc or dec and then convert that int to a string. 
   //With the int now a string, use strcat() to make it look like "0002.wav".
   track = 2;
   char *StrTrack = itoa(track);
   strcat(StrTrack,".wav"); // It should produce "0002.wav".
   strcat(filename,".wav");
   */

  //filename = "pin" + String(pin) + ".wav";  
  filename = "pin1.wav";
  if((file = readSDCard(filename))) {
    Serial.println("filename: " + filename);
    file.close();
    filename.toCharArray(buf, filename.length());
    audio.play(buf);
    //audio.play("pin1.wav");
  } 
}

// ------------------------------------




