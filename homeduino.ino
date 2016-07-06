#include <SerialCommand.h>
#include <RFControl.h>
#include <DHTlib.h>

#define NETWORK_ENABLED
#define MAX_RECORDINGS 300 // If you're using Aruino env, set this in RFContro.h too

#ifdef NETWORK_ENABLED
#include "network.h"
#endif

void argument_error();

Stream& HomeClient = Serial;

SerialCommand sCmd;

#include "rfcontrol_command.h"
#ifdef KEYPAD_ENABLED
#include "keypad_command.h"
#endif
#include "dht_command.h"

void digital_read_command();
void digital_write_command();
void analog_read_command();
void analog_write_command();
void reset_command();
void pin_mode_command();
void ping_command();
void unrecognized(const char *command);


void setup() {
  Serial.begin(115200);
  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("DR", digital_read_command);
  sCmd.addCommand("DW", digital_write_command);
  sCmd.addCommand("AR", analog_read_command);
  sCmd.addCommand("AW", analog_write_command);
  sCmd.addCommand("PM", pin_mode_command);
  sCmd.addCommand("RF", rfcontrol_command);
  sCmd.addCommand("PING", ping_command);
  sCmd.addCommand("DHT", dht_command);
  sCmd.addCommand("RESET", reset_command);
  #ifdef KEYPAD_ENABLED
  sCmd.addCommand("K", keypad_command);
  #endif
  sCmd.setDefaultHandler(unrecognized);
  HomeClient.print(F("ready\r\n"));

  #ifdef NETWORK_ENABLED
  network_setup();
  #endif
}

void loop() {
  // handle serial command
  sCmd.readSerial();
  // handle rf control receiving
  rfcontrol_loop();
  #ifdef KEYPAD_ENABLED
  // handle keypad keypress
  keypad_loop();
  #endif

  #ifdef NETWORK_ENABLED
  network_loop();
  #endif
}

void digital_read_command() {
  char* arg = sCmd.next();
    if(arg == NULL) {
      argument_error();
      return;
    }
    int pin = atoi(arg);
    int val = digitalRead(pin);
    HomeClient.print(F("ACK "));
    HomeClient.write('0' + val);
    HomeClient.print(F("\r\n"));
}

void analog_read_command() {
  char* arg = sCmd.next();
    if(arg == NULL) {
      argument_error();
      return;
    }
    int pin = atoi(arg);
    int val = analogRead(pin);
    HomeClient.print(F("ACK "));
    HomeClient.print(val);
    HomeClient.print(F("\r\n"));
}

void digital_write_command() {
  char* arg = sCmd.next();
    if(arg == NULL) {
      argument_error();
      return;
    }
    int pin = atoi(arg);
    arg = sCmd.next();
    if(arg == NULL) {
      argument_error();
      return;
    }
    int val = atoi(arg);
    digitalWrite(pin, val);
    HomeClient.print(F("ACK\r\n"));
}

void analog_write_command() {
  char* arg = sCmd.next();
    if(arg == NULL) {
      argument_error();
      return;
    }
    int pin = atoi(arg);
    arg = sCmd.next();
    if(arg == NULL) {
      argument_error();
      return;
    }
    int val = atoi(arg);
    analogWrite(pin, val);
    HomeClient.print(F("ACK\r\n"));
}

void pin_mode_command() {
  char* arg = sCmd.next();
    if(arg == NULL) {
      argument_error();
      return;
    }
    int pin = atoi(arg);
    arg = sCmd.next();
    if(arg == NULL) {
      argument_error();
      return;
    }
    // INPUT 0x0
    // OUTPUT 0x1
    int mode = atoi(arg);
    pinMode(pin, mode);
    HomeClient.print(F("ACK\r\n"));
}

void ping_command() {
  char *arg;
  HomeClient.print("PING");
  arg = sCmd.next();
  if (arg != NULL) {
    HomeClient.write(' ');
    HomeClient.print(arg);
  }
  HomeClient.print(F("\r\n"));
}

void reset_command() {
  RFControl::stopReceiving();
  HomeClient.print(F("ready\r\n"));
}

void argument_error() {
  HomeClient.print(F("ERR argument_error\r\n"));
}
// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  HomeClient.print(F("ERR unknown_command\r\n"));
}

