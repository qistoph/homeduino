#include <RFControl.h>

int interrupt_pin = -1;

void rfcontrol_command_send();
void rfcontrol_command_receive();
void rfcontrol_command_raw();

bool in_raw_mode = false;

void rfcontrol_loop() {
  if(in_raw_mode){
    if(RFControl::existNewDuration()){
      HomeClient->print(RFControl::getLastDuration() * RFControl::getPulseLengthDivider());
      HomeClient->print(", ");
      static byte line=0;
      line++;
      if(line>=16){
        line=0;
        HomeClient->write('\n');
      }
    }
  }
  else{
    if(RFControl::hasData()) {
      unsigned int *timings;
      unsigned int timings_size;
      RFControl::getRaw(&timings, &timings_size);
      unsigned int buckets[8];
      unsigned int pulse_length_divider = RFControl::getPulseLengthDivider();
      RFControl::compressTimings(buckets, timings, timings_size);
      HomeClient->print("RF receive ");
      for(unsigned int i=0; i < 8; i++) {
        unsigned long bucket = buckets[i] * pulse_length_divider;
        HomeClient->print(bucket);
        HomeClient->write(' ');
      }
      for(unsigned int i=0; i < timings_size; i++) {
        HomeClient->write('0' + timings[i]);
      }
      HomeClient->print("\r\n");
      RFControl::continueReceiving();
    }
  }
}

void rfcontrol_command() {
  char* arg = sCmd.next();
  if(arg == NULL) {
    argument_error();
    return;
  }
  if (strcmp(arg, "send") == 0) {
    rfcontrol_command_send();
  } else if (strcmp(arg, "receive") == 0) {
    rfcontrol_command_receive();
  } else if(strcmp(arg, "raw") == 0) {
    rfcontrol_command_raw();
  } else {
    argument_error();
  }
}

void rfcontrol_command_raw(){
  char* arg = sCmd.next();
  if(arg == NULL) {
    argument_error();
    return;
  }
  int interrupt_pin = atoi(arg);
  RFControl::startReceiving(interrupt_pin);
  in_raw_mode = true;
  HomeClient->print(F("ACK\r\n"));
}

void rfcontrol_command_receive() {
  char* arg = sCmd.next();
  if(arg == NULL) {
    argument_error();
    return;
  }
  interrupt_pin = atoi(arg);
  RFControl::startReceiving(interrupt_pin);
  in_raw_mode = false;
  HomeClient->print(F("ACK\r\n"));
}


void rfcontrol_command_send() {
  char* arg = sCmd.next();
  if(arg == NULL) {
    argument_error();
    return;
  }
  int transmitter_pin = atoi(arg);

  arg = sCmd.next();
  if(arg == NULL) {
    argument_error();
    return;
  }
  int repeats = atoi(arg);

  // read pulse lengths
  unsigned long buckets[8];
  for(unsigned int i = 0; i < 8; i++) {
    arg = sCmd.next();
    if(arg == NULL) {
      argument_error();
      return;
    }
    buckets[i] = strtoul(arg, NULL, 10);
  }
  //read pulse sequence
  arg = sCmd.next();
  if(arg == NULL) {
    argument_error();
    return;
  }
  RFControl::sendByCompressedTimings(transmitter_pin, buckets, arg, repeats);
  in_raw_mode = false;
  HomeClient->print(F("ACK\r\n"));
}
