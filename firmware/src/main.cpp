#include <Arduino.h>
#include <IRremote.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();
IRsend irsend;     // IR LED Transmitter is connected to Arduino pin #3
IRrecv irrecv(5);  // IR VS1838B Receiver is connected to Arduino pin #5 #7
decode_results results;

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);   // RF Receiver on interrupt 0 => that is pin #2
  mySwitch.enableTransmit(4);  // RF Transmitter is connected to Arduino pin #4 #6
  irrecv.enableIRIn();
  
  // Optional set pulse length.
  // mySwitch.setPulseLength(320);
  mySwitch.setPulseLength(170);
  
  // Optional set protocol (default is 1, will work for most outlets)
  // mySwitch.setProtocol(2);
  
  // Optional set number of transmission repetitions.
  // mySwitch.setRepeatTransmit(15);
}

void softReset() {
  asm volatile ("  jmp 0");
}

void loop() {
  if (irrecv.decode(&results)) {    // Receiving IR signal to RF signal
    Serial.print("IR Received: ");
    Serial.println(results.value);
    mySwitch.send(results.value, 24);
    irrecv.resume();
    delay(100);
  } else {                          // Receiving RF signal to IR signal
    if (mySwitch.available()) {
      int value = mySwitch.getReceivedValue();
      if (value == 0) {
        Serial.println("Unknown encoding");
      } else {
        Serial.print("RF Received:  ");
        Serial.println(mySwitch.getReceivedValue());
        irsend.sendNEC(mySwitch.getReceivedValue(), 28);
        delay(90);
      }
      mySwitch.resetAvailable();
      softReset();
    }
  }
}
