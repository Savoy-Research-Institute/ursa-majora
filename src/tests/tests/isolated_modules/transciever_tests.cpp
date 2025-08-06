#include "HC12.h"

#define RX_PIN 2
#define TX_PIN 3

HC12 hc12(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(9600);
  if (hc12.begin(9600)) {
    Serial.println("HC-12 initialized");
  } else {
    Serial.println("HC-12 initialization failed");
  }
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readString();
    if (hc12.sendWithAck(message.c_str())) {
      Serial.println("Message sent and acknowledged");
    } else {
      Serial.println("Message sent but not acknowledged");
    }
  }

  if (hc12.isAvailable()) {
    char buffer[64];
    int bytesRead = hc12.receive(buffer, sizeof(buffer));
    if (bytesRead > 0) {
      Serial.print("Received: ");
      Serial.println(buffer);
      // Send acknowledgment
      hc12.sendAck();
    }
  }
}