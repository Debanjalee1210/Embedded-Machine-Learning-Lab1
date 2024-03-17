/*
  LED_BLE_SWITCH

  This code creates a Bluetooth® Low Energy peripheral with an LED service that contains a
  characteristic to control an LED.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  We have used a generic Bluetooth® Low Energy central app nRF Connect (Android), to interact 
  with the services and characteristics
  created in this sketch.
*/

#include <ArduinoBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

// pin to use for the red LED >> 22 defined as macro in pins_arduino.h
const int ledRed = LEDR;  
// pin to use for the green LED >> 23 defined as macro in pins_arduino.h
const int ledGreen = LEDG; 
// pin to use for the red LED >> 24 defined as macro in pins_arduino.h
const int ledBlue = LEDB; 

//to store the time instance when Red Led was turned on
unsigned long startTimeRedLed=0;
//to store the time instance when Blue Led was turned on
unsigned long startTimeBlueLed=0;
//to store the time instance when Green Led was turned on
unsigned long startTimeGreenLed=0;

//to store the current time instance to update the LED
unsigned long currentMillis = 0;  

//interval at which to blink (milliseconds) the Red LED
const long interval_5s = 5000;  

//interval at which to blink (milliseconds) the Blue LED
const long interval_4s = 4000;  

//interval at which to blink (milliseconds) the Green LED
const long interval_3s = 3000;  

//states of LED
enum led_state {DARK_STATE, //all leds off
                  RED_STATE, //Red LED is ON,Blue is OFF, Green is OFF 
                  BLUE_STATE, //Blue LED is ON,Red is OFF, Green is OFF
                  GREEN_STATE}; //Green LED is ON, Red is off, Blue is oFF

//current state of LED at any instance; by default initialized to DARK STATE
led_state currState = DARK_STATE;

//Led event when it is in DARK state
enum led_state led_dark_state(){
  //will turn off the red LED
  digitalWrite(ledRed, HIGH);
  //will turn off the blue LED  
  digitalWrite(ledBlue,HIGH);
  //will turn off the green LED
  digitalWrite(ledGreen,HIGH);
  //returns the state as dark with no leds ON
  return DARK_STATE;
}

//Led event when it is in Red state
enum led_state led_red_state(){
  //store the instance red led is turned on using millis() function
  startTimeRedLed = millis();
  // will turn the red LED on: since led R,G,B are active low leds
  digitalWrite(ledRed, LOW);  
  //will turn off the blue LED
  digitalWrite(ledBlue,HIGH);
  //will turn off the green LED
  digitalWrite(ledGreen,HIGH);
  //returns the red led as ON state
  return RED_STATE;
}

//Led event when it is in Blue state
enum led_state led_blue_state(){
  startTimeBlueLed = millis();
  //will turn off the red LED
  digitalWrite(ledRed, HIGH); 
  //will turn the blue LED on        
  digitalWrite(ledBlue, LOW); 
  //will turn off the green LED
  digitalWrite(ledGreen, HIGH);
  //returns the blue led as ON state
  return BLUE_STATE;
}

//Led event when it is in Green state
enum led_state led_green_state(){
  startTimeGreenLed = millis();
  //will turn off the red LED
  digitalWrite(ledRed, HIGH);   
  //will turn off the blue LED   
  digitalWrite(ledBlue, HIGH);
  // will turn the green LED on
  digitalWrite(ledGreen, LOW); 
  //returns the blue led as ON state
  return GREEN_STATE;
}


//setup code to run once
void setup() {
  //Initialised serial communication set to baudrate 9600
  Serial.begin(9600);
  //wait for serial setup
  while (!Serial);

  // set red LED pin to output mode
  pinMode(ledRed, OUTPUT);
  // set green LED pin to output mode
  pinMode(ledGreen, OUTPUT);
  // set blue LED pin to output mode
  pinMode(ledBlue, OUTPUT);

  // begin initialization for BLE
  if (!BLE.begin()) {
    //sanity checking for ble configuration
    Serial.println("starting Bluetooth® Low Energy module failed!");
    //wait in infinite loop
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("LED_SWITCH");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add LED service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  //debug logs
  Serial.println("BLE LED Peripheral");
}

//code to run in loop
void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      //store the current time instance
      currentMillis = millis();
      //check for the state as per BLE switch input
      switch(currState)
      {
        //when the current state is dark,check if there is BLE switch input
        case DARK_STATE:
        // if the remote device wrote to the characteristic,
        // use the value to control the LED:
        if (switchCharacteristic.written()) {
          //check if the ble input is 0x01 : oxo1 is for red switch 
          if (switchCharacteristic.value()==0x01) {
            //change the state to led red state
            currState = led_red_state();
          }
        }
        else
        {
          //if there is no input from BLE then remain in dark state
          currState = led_dark_state();
          //do nothing
        }
        break;

        case RED_STATE:
        // if the remote device wrote to the characteristic,
        // use the value to control the LED:
        if (switchCharacteristic.written()) {
          //check if the ble input is 0x02 : 0x02 is for blue switch
          if (switchCharacteristic.value()==0x02) {
            //change the current state to blue state
            currState = led_blue_state();
          }
        }
        else if(currentMillis - startTimeRedLed >= interval_5s) //check if the time interval from the instance red led is turned on is more than 5s
        {
          //debug log to print the timeout for red led if there is no switch triggered from BLE apps
          Serial.println("Red LED timeout : ");
          //store the difference in a variable to print it 
          unsigned long val = currentMillis - startTimeRedLed;
          //debug log to print the value
          Serial.println(val);
          //reset the start time to current time value for next iteration calculation
          startTimeRedLed = currentMillis;
          //change the state to dark state
          currState = led_dark_state();
        }
        break;

        case BLUE_STATE:
        // if the remote device wrote to the characteristic,
        // use the value to control the LED:
        if (switchCharacteristic.written()) {
          //check if the ble input is 0x03 : 0x03 is for green switch
          if (switchCharacteristic.value()==0x03) {
            //change the current state to green state
            currState = led_green_state();
          }
        }
        else if(currentMillis - startTimeBlueLed >= interval_4s)//check if the time interval from the instance blue led is turned on is more than 4s
        {
          //debug log to print the timeout for blue led if there is no switch triggered from BLE apps
          Serial.println("Blue LED timeout : ");
          //store the difference in a variable to print it 
          unsigned long val = currentMillis - startTimeBlueLed;
          //debug log to print the value
          Serial.println(val);
          //reset the start time to current time value for next iteration calculation
          startTimeBlueLed = currentMillis;
          //change the state to red state
          currState = led_red_state();
        }
        break;

        case GREEN_STATE:
        // if the remote device wrote to the characteristic,
        // use the value to control the LED:
        if (switchCharacteristic.written()) {
          //check if the ble input is 0x00 : 0x00 is for switching off all leds
          if (switchCharacteristic.value()==0x00) {
            //change the state to dark
            currState = led_dark_state();
          }
        }
        else if(currentMillis - startTimeGreenLed >= interval_3s)//check if the time interval from the instance green led is turned on is more than 3s
        {
          //debug log to print the timeout for green led if there is no switch triggered from BLE apps
          Serial.println("Green LED timeout : ");
          //store the difference in a variable to print it 
          unsigned long val = currentMillis - startTimeGreenLed;
          //debug log to print the value
          Serial.println(val);
          //reset the start time to current time value for next iteration calculation
          startTimeGreenLed = currentMillis;
          //change the state to blue state
          currState = led_blue_state();
        }

      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    //print out the address of the central device disconnected
    Serial.println(central.address());
  }
}

