#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <Arduino.h>
#include <SD.h>

class DataLogger {
public:
    DataLogger(int chipSelectPin);
    void begin();
    void switchOn();
    void switchOff();
    bool isOn() const;
    
    bool readFile(const char* filename, String& content);
    bool writeFile(const char* filename, const String& content);
    bool appendFile(const char* filename, const String& content);
    
    bool storeCheatCodeScript(const char* scriptName, const String& script);
    bool retrieveCheatCodeScript(const char* scriptName, String& script);
    
    void logBlackboxData(const String& data);
    
    bool createLogFile(const char* filename);
    bool writeLogData(const char* filename, const String& data);

private:
    int _chipSelectPin;
    bool _isOn;
    File _currentFile;
    
    bool initializeSDCard();
};

#endif // DATA_LOGGER_H