#include <Arduino.h>
#include <Adafruit_MPL3115A2.h>
#define LED_BUILTIN 2

// put function declarations here:
int myFunction(int, int);

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // change state of the LED by setting the pin to the HIGH voltage level
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // change state of the LED by setting the pin to the LOW voltage level
  delay(1000);                      // wait for a second
  int result = myFunction(2, 3);
}