//code to run hc12 in isolation...
//https://www.allaboutcircuits.com/projects/understanding-and-implementing-the-hc-12-wireless-transceiver-module/

/*  HC12 Send/Receive Example Program 1
    By Mark J. Hughes 
    for AllAboutCircuits.com
    
    Connect HC12 "RXD" pin to Arduino Digital Pin 4
    Connect HC12 "TXD" pin to Arduino Digital Pin 5
    Connect HC12 "Set" pin to Arduino Digital Pin 6
   
    Do not power over USB.  Per datasheet, 
    power HC12 with a supply of at least 100 mA with 
    a 22 uF - 1000 uF reservoir capacitor.
    Upload code to two Arduinos connected to two computers.
    
    Transceivers must be at least several meters apart to work.
   
   Upload the following code and open the serial port monitor in the Arduino IDE to send/receive messages.

The functions "*.available()" reads the number of bytes stored in the Arduino's Serial or SoftwareSerial FIFO buffers. 
As long as there is a non-zero value returned by "*.available()", the "*.read()" function pulls one byte out of the buffer 
and the "*.write()" function sends that byte along to the other serial port to be read by either the computer or the HC-12.

The Arduino UART has a 64-byte receive buffer built into the hardware so any bytes of data that exceed the 64-byte limit will be discarded. 
The software serial also has a 64-byte buffer; however, the SoftwareSerial library can be modified to increase that, if required.


 */

#include 

const byte HC12RxdPin = 4;                  // Recieve Pin on HC12
const byte HC12TxdPin = 5;                  // Transmit Pin on HC12

SoftwareSerial HC12(HC12TxdPin,HC12RxdPin); // Create Software Serial Port

void setup() {
  Serial.begin(9600);                       // Open serial port to computer
  HC12.begin(9600);                         // Open serial port to HC12
}

void loop() {
  if(HC12.available()){                     // If Arduino's HC12 rx buffer has data
    Serial.write(HC12.read());              // Send the data to the computer
    }
  if(Serial.available()){                   // If Arduino's computer rx buffer has data
    HC12.write(Serial.read());              // Send that data to serial
  }
}


/*In the first program, single bytes were detected and read from the serial buffer and software serial buffer and immediately written to the other serial port. The following program will read entire strings of data from the serial buffers and store it until a newline character is detected at which point it will write out the entire buffer.

The first program had no way to enter command mode and change the settings of the HC-12 transceiver. The following program can detect command sequences and change the settings of remote and local transceivers (provided they are running the newer version of the code). Note that it is important to change the settings of the remote transceiver first and then change the settings of the local transceiver to match, as any initial changes to the local transceiver will interrupt communication between the two.

The Arduino will collect strings from the serial port and software serial port. Once the new line has been detected, there is a check to confirm if the string is a command sequence (commands begin with AT+). If a command sequence is present, the local chip executes the command and transmits the command to the remote chip for execution there.   The code keeps the HC12s on the same channel at same power levels during testing.

Use caution as it is possible to accidentally lower the remote power level to a point that the remote transceiver can no longer communicate with the local transceiver. This is quickly fixed by broadcasting a new power level. Changing the baud rate can disable communication between the HC-12 and the Arduino unless the Arduino baud rate is changed at the same time.*/


/*  HC12 Send/Receive Example Program 2
    By Mark J. Hughes
    for AllAboutCircuits.com

    This code will automatically detect commands as sentences that begin
    with AT and both write them and broadcast them to remote receivers
    to be written.  Changing settings on a local transceiver will
    change settings on a remote receiver.

    Connect HC12 "RXD" pin to Arduino Digital Pin 4
    Connect HC12 "TXD" pin to Arduino Digital Pin 5
    Connect HC12 "Set" pin to Arduino Digital Pin 6

    Do not power HC12 via Arduino over USB.  Per the data sheet,
    power the HC12 with a supply of at least 100 mA with
    a 22 uF to 1000 uF reservoir capacitor and connect a 1N4007 diode in 
    series with the positive supply line if the potential difference exceeds 4.5 V.

    Upload code to two Arduinos connected to two computers 
    that are separated by at least several meters.

*/

#include 

const byte HC12RxdPin = 4;                      // "RXD" Pin on HC12
const byte HC12TxdPin = 5;                      // "TXD" Pin on HC12
const byte HC12SetPin = 6;                      // "SET" Pin on HC12

unsigned long timer = millis();                 // Delay Timer

char SerialByteIn;                              // Temporary variable
char HC12ByteIn;                                // Temporary variable
String HC12ReadBuffer = "";                     // Read/Write Buffer 1 for HC12
String SerialReadBuffer = "";                   // Read/Write Buffer 2 for Serial
boolean SerialEnd = false;                      // Flag to indicate End of Serial String
boolean HC12End = false;                        // Flag to indiacte End of HC12 String
boolean commandMode = false;                    // Send AT commands

// Software Serial ports Rx and Tx are opposite the HC12 Rx and Tx
// Create Software Serial Port for HC12
SoftwareSerial HC12(HC12TxdPin, HC12RxdPin);

void setup() {

  HC12ReadBuffer.reserve(64);                   // Reserve 64 bytes for Serial message input
  SerialReadBuffer.reserve(64);                 // Reserve 64 bytes for HC12 message input

  pinMode(HC12SetPin, OUTPUT);                  // Output High for Transparent / Low for Command
  digitalWrite(HC12SetPin, HIGH);               // Enter Transparent mode
  delay(80);                                    // 80 ms delay before operation per datasheet
  Serial.begin(9600);                           // Open serial port to computer
  HC12.begin(9600);                             // Open software serial port to HC12
}

void loop() {

  while (HC12.available()) {                    // While Arduino's HC12 soft serial rx buffer has data
    HC12ByteIn = HC12.read();                   // Store each character from rx buffer in byteIn
    HC12ReadBuffer += char(HC12ByteIn);         // Write each character of byteIn to HC12ReadBuffer
    if (HC12ByteIn == '\n') {                   // At the end of the line
      HC12End = true;                           // Set HC12End flag to true
    }
  }

  while (Serial.available()) {                  // If Arduino's computer rx buffer has data
    SerialByteIn = Serial.read();               // Store each character in byteIn
    SerialReadBuffer += char(SerialByteIn);     // Write each character of byteIn to SerialReadBuffer
    if (SerialByteIn == '\n') {                 // Check to see if at the end of the line
      SerialEnd = true;                         // Set SerialEnd flag to indicate end of line
    }
  }

  if (SerialEnd) {                              // Check to see if SerialEnd flag is true

    if (SerialReadBuffer.startsWith("AT")) {    // Has a command been sent from local computer
      HC12.print(SerialReadBuffer);             // Send local command to remote HC12 before changing settings
      delay(100);                               //
      digitalWrite(HC12SetPin, LOW);            // Enter command mode
      delay(100);                               // Allow chip time to enter command mode
      Serial.print(SerialReadBuffer);           // Echo command to serial
      HC12.print(SerialReadBuffer);             // Send command to local HC12
      delay(500);                               // Wait 0.5s for a response
      digitalWrite(HC12SetPin, HIGH);           // Exit command / enter transparent mode
      delay(100);                               // Delay before proceeding
    } else {
      HC12.print(SerialReadBuffer);             // Transmit non-command message
    }
    SerialReadBuffer = "";                      // Clear SerialReadBuffer
    SerialEnd = false;                          // Reset serial end of line flag
  }

  if (HC12End) {                                // If HC12End flag is true
    if (HC12ReadBuffer.startsWith("AT")) {      // Check to see if a command is received from remote
      digitalWrite(HC12SetPin, LOW);            // Enter command mode
      delay(100);                               // Delay before sending command
      Serial.print(SerialReadBuffer);           // Echo command to serial.
      HC12.print(HC12ReadBuffer);               // Write command to local HC12
      delay(500);                               // Wait 0.5 s for reply
      digitalWrite(HC12SetPin, HIGH);           // Exit command / enter transparent mode
      delay(100);                               // Delay before proceeding
      HC12.println("Remote Command Executed");  // Acknowledge execution
    } else {
      Serial.print(HC12ReadBuffer);             // Send message to screen
    }
    HC12ReadBuffer = "";                        // Empty buffer
    HC12End = false;                            // Reset flag
  }
}
/*
The previous code, if uploaded to both boards, allows you to program both the local and remote transceiver at the same time.  

Here are some other commands for your reference:

Valid Baud Rate Commands: AT+B1200, AT+B2400, AT+B4800, AT+9600, AT+19200, AT+38400, AT+57600, AT+115200

Valid Channel Commands:  AT+C001, AT+C002, AT+C0xx, ... , AT+C099, AT+C100. Per its datasheet, if using multiple HC-12s on different channels, stagger adjacent channels at least five apart 
40
kHz
1 channel
×
5 channels
=
200
kHz
Valid Transmit Power Commands: AT+P1 (-1 dBm), AT+P2 (2 dBm), AT+P3 (5 dBm), AT+P4 (8 dBm), AT+P5 (11 dBm), AT+P6 (14 dBm), AT+P7 (17 dBm), AT+P8 (20 dBm)

Return to default settings:  AT+DEFAULT 

Additional commands are available; refer to the user manual for more information.*/



//https://howtomechatronics.com/tutorials/arduino/arduino-and-hc-12-long-range-wireless-communication-module/