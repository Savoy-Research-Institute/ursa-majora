// CombinedRadioHC12.cpp

#include "CombinedRadioHC12.h"

// HC12 Implementation

const char *HC12::ACK_MESSAGE = "ACK";

HC12::HC12(int rxPin, int txPin) : _rxPin(rxPin), _txPin(txPin)
{
    _serial = new SoftwareSerial(rxPin, txPin);
}

bool HC12::begin(long baudRate)
{
    pinMode(_rxPin, INPUT);
    pinMode(_txPin, OUTPUT);
    _serial->begin(baudRate);
    return sendCommand("AT", "OK");
}

bool HC12::isAvailable()
{
    return _serial->available() > 0;
}

void HC12::send(const char *message)
{
    _serial->println(message);
}

int HC12::receive(char *buffer, int bufferSize)
{
    int i = 0;
    unsigned long startTime = millis();
    while (millis() - startTime < 100 && i < bufferSize - 1)
    {
        if (_serial->available())
        {
            buffer[i] = _serial->read();
            i++;
        }
    }
    buffer[i] = '\0';
    return i;
}

bool HC12::setChannel(int channel)
{
    if (channel < 1 || channel > 100)
        return false;
    char command[10];
    sprintf(command, "AT+C%03d", channel);
    return sendCommand(command, "OK");
}

bool HC12::setBaudRate(long baudRate)
{
    const char *command;
    switch (baudRate)
    {
    case 1200:
        command = "AT+B1200";
        break;
    case 2400:
        command = "AT+B2400";
        break;
    case 4800:
        command = "AT+B4800";
        break;
    case 9600:
        command = "AT+B9600";
        break;
    case 19200:
        command = "AT+B19200";
        break;
    case 38400:
        command = "AT+B38400";
        break;
    case 57600:
        command = "AT+B57600";
        break;
    case 115200:
        command = "AT+B115200";
        break;
    default:
        return false;
    }
    if (sendCommand(command, "OK"))
    {
        _serial->begin(baudRate);
        return true;
    }
    return false;
}

bool HC12::setTransmissionPower(int power)
{
    if (power < 1 || power > 8)
        return false;
    char command[8];
    sprintf(command, "AT+P%d", power);
    return sendCommand(command, "OK");
}

bool HC12::sendWithAck(const char *message, unsigned long timeout)
{
    send(message);
    return waitForAck(timeout);
}

void HC12::sendAck()
{
    send(ACK_MESSAGE);
}

bool HC12::waitForAck(unsigned long timeout)
{
    char buffer[4];
    unsigned long startTime = millis();
    while (millis() - startTime < timeout)
    {
        if (isAvailable())
        {
            int bytesRead = receive(buffer, sizeof(buffer));
            if (bytesRead > 0 && strcmp(buffer, ACK_MESSAGE) == 0)
            {
                return true;
            }
        }
    }
    return false;
}

bool HC12::sendCommand(const char *command, const char *expectedResponse, unsigned long timeout)
{
    _serial->println(command);
    char response[20];
    unsigned long startTime = millis();
    int i = 0;
    while (millis() - startTime < timeout)
    {
        if (_serial->available())
        {
            response[i] = _serial->read();
            i++;
            if (i >= 19)
                i = 19;
            response[i] = '\0';
            if (strstr(response, expectedResponse) != NULL)
            {
                return true;
            }
        }
    }
    return false;
}

// Radio Implementation

Radio::Radio(int ppmPin, int radioTransistorPin)
    : _radioTransistorPin(radioTransistorPin), _isOn(false), _previousMillis(0)
{
    _ppm = new PPMReader(ppmPin, NUM_CHANNELS);
}

void Radio::init()
{
    pinMode(_radioTransistorPin, OUTPUT);
    digitalWrite(_radioTransistorPin, LOW);
    _isOn = false;
}

void  ::switchOn()
{
    if (!_isOn)
    {
        digitalWrite(_radioTransistorPin, HIGH);
        _isOn = true;
    }
}

void Radio::switchOff()
{
    if (_isOn)
    {
        digitalWrite(_radioTransistorPin, LOW);
        _isOn = false;
    }
    
}

void Radio::readAllChannels()
{
    if (!_isOn)
        return;

    unsigned long currentMillis = millis();
    if (currentMillis - _previousMillis >= _interval)
    {
        _previousMillis = currentMillis;

        for (int i = 0; i < NUM_CHANNELS; i++)
        {
            _channelValues[i] = _ppm->latestValidChannelValue(i, 0);
        }
        // Uncomment for debugging
        /*
        for (int i = 0; i < NUM_CHANNELS; i++) {
            Serial.print(channelValues[i]);
            Serial.print(" ");
        }
        Serial.println();
        */
    }
}// the software should be able to read PWM and PPM, whichever is defined,
//this will also need to be changed on the radio, whatever it transmits...

int Radio::getThrottle() { return _channelValues[THROTTLE]; }
int Radio::getRoll() { return _channelValues[ROLL]; }
int Radio::getPitch() { return _channelValues[PITCH]; }
int Radio::getYaw() { return _channelValues[YAW]; }
int Radio::getSwitch3Way1() { return _channelValues[SWITCH3WAY_1]; }
int Radio::getPot1() { return _channelValues[POT1]; }
int Radio::getSwitch3Way2() { return _channelValues[SWITCH3WAY_2]; }
int Radio::getPot2() { return _channelValues[POT2]; }