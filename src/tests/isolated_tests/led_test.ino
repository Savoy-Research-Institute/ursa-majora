//void ledON(){} from digitalWrite()

// Define the LED pins
int greenLedPin = 11; // You can change this to your specific pin for the green LED
int redLedPin = 12;   // You can change this to your specific pin for the red LED

void setup() {
  // Initialize the LED pins as outputs
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
}

void loop() {
  // Call the ledON function for the green LED
  ledON(greenLedPin); // Example: Green LED on
  ledOFF(redLedPin);
delay(1000);
  // Call the ledOFF function for the red LED
  ledOFF(greenLedPin);
  ledON(redLedPin); // Example: Red LED off
  delay(1000);

}

// Function to turn on LED
void ledON(int ledPin) {
  digitalWrite(ledPin, HIGH); // Turn on the LED
}

// Function to turn off LED
void ledOFF(int ledPin) {
  digitalWrite(ledPin, LOW); // Turn off the LED
}
