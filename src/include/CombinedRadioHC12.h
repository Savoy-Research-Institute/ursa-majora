// CombinedRadioHC12.h

#ifndef COMBINED_RADIO_HC12_H
#define COMBINED_RADIO_HC12_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <PPMReader.h>

// Channel definitions for Radio
#define THROTTLE 0
#define ROLL 1
#define PITCH 2
#define YAW 3
#define SWITCH3WAY_1 4
#define POT1 5
#define SWITCH3WAY_2 6
#define POT2 7

#define NUM_CHANNELS 8

class HC12 {
public:
    HC12(int rxPin, int txPin);
    bool begin(long baudRate = 9600);
    bool isAvailable();
    void send(const char* message);
    int receive(char* buffer, int bufferSize);
    bool setChannel(int channel);
    bool setBaudRate(long baudRate);
    bool setTransmissionPower(int power);
    bool sendWithAck(const char* message, unsigned long timeout = 1000);
    void sendAck();
    bool waitForAck(unsigned long timeout = 1000);

private:
    SoftwareSerial* _serial;
    int _rxPin;
    int _txPin;
    bool sendCommand(const char* command, const char* expectedResponse, unsigned long timeout = 500);
    
    static const char* ACK_MESSAGE;
};

class Radio {
public:
    Radio(int ppmPin, int radioTransistorPin);
    void init();
    void switchOn();
    void switchOff();
    void readAllChannels();
    
    // Getter functions for channel values
    int getThrottle();
    int getRoll();
    int getPitch();
    int getYaw();
    int getSwitch3Way1();
    int getPot1();
    int getSwitch3Way2();
    int getPot2();

private:
    PPMReader* _ppm;
    int _radioTransistorPin;
    bool _isOn;
    unsigned long _previousMillis;
    const unsigned long _interval = 50; // 50ms interval for updates
    volatile int _channelValues[NUM_CHANNELS];
};

#endif // COMBINED_RADIO_HC12_H
