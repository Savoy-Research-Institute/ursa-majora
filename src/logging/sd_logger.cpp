#include "data_logger.h"

DataLogger::DataLogger(int chipSelectPin) : _chipSelectPin(chipSelectPin), _isOn(false) {}

void DataLogger::begin() {
    pinMode(_chipSelectPin, OUTPUT);
    switchOff();
}

void DataLogger::switchOn() {
    digitalWrite(_chipSelectPin, HIGH);
    _isOn = true;
}

void DataLogger::switchOff() {
    digitalWrite(_chipSelectPin, LOW);
    _isOn = false;
}

bool DataLogger::isOn() const {
    return _isOn;
}

bool DataLogger::initializeSDCard() {
    if (!_isOn) {
        return false;
    }
    return SD.begin(_chipSelectPin);
}

bool DataLogger::readFile(const char* filename, String& content) {
    if (!_isOn || !initializeSDCard()) {
        return false;
    }
    
    File file = SD.open(filename, FILE_READ);
    if (!file) {
        return false;
    }
    
    content = file.readString();
    file.close();
    return true;
}

bool DataLogger::writeFile(const char* filename, const String& content) {
    if (!_isOn || !initializeSDCard()) {
        return false;
    }
    
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
        return false;
    }
    
    size_t bytesWritten = file.print(content);
    file.close();
    
    return bytesWritten == content.length();
}

bool DataLogger::appendFile(const char* filename, const String& content) {
    if (!_isOn || !initializeSDCard()) {
        return false;
    }
    
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
        return false;
    }
    
    file.seek(file.size());
    size_t bytesWritten = file.print(content);
    file.close();
    
    return bytesWritten == content.length();
}

bool DataLogger::storeCheatCodeScript(const char* scriptName, const String& script) {
    String filename = String("cheatcodes/") + scriptName + ".lua";
    return writeFile(filename.c_str(), script);
}

bool DataLogger::retrieveCheatCodeScript(const char* scriptName, String& script) {
    String filename = String("cheatcodes/") + scriptName + ".lua";
    return readFile(filename.c_str(), script);
}

void DataLogger::logBlackboxData(const String& data) {
    appendFile("blackbox.log", data + "\n");
}

bool DataLogger::createLogFile(const char* filename) {
    if (!_isOn || !initializeSDCard()) {
        return false;
    }
    
    if (SD.exists(filename)) {
        return true;  // File already exists
    }
    
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
        return false;
    }
    
    file.close();
    return true;
}

bool DataLogger::writeLogData(const char* filename, const String& data) {
    return appendFile(filename, data + "\n");
}