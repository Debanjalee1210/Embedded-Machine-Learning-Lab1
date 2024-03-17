# Embedded Machine Learning-Lab1
 CEN 598: EML lab1 using Arduino nano BLE Sense  
The BMI/CEN 598 Mini Project 1 involved the development of a state machine using the Arduino Nano BLE 
Sense development board. This state machine utilizes the board's inbuilt RGB LED connected to pins 
22,23,24 respectively, to simulate the behavior of a push button switch since there is on-board switch in 
the Arduino Nano BLE Sense board, we are implementing a soft switch using nRF connect Mobile app using 
BLE protocol for transition between different states. The primary goal of this project was to create a 
functional state machine that responds to user input from the BLE application and changes LED colors 
accordingly. This report provides an overview of the project, its implementation, challenges faced, and 
lessons learned.
Project Description: 
The project requirements were as follows:
Utilize the Arduino Nano BLE Sense board with an inbuilt RGB LED connected to pins 22, 23, and 24.
Implement a state machine with the following states and transitions:
• Initial state: 'Dark' (LED off or dark color)
• Transition to 'Red' state upon pressing an onboard switch (SW).
• Transition from 'Red' to 'Dark' state after a 5-second delay.
• Transition from 'Red' to 'Blue' state upon pressing SW in our case on receiving input from BLE app.
• Transition from 'Blue' to 'Red' state after a 4-second delay.
• Transition from 'Blue' to 'Green' state upon pressing SW i.e., receiving input from BLE app.
• Transition from 'Green' to 'Blue' state after a 3-second delay.
• Transition from 'Green' to 'Dark' state upon pressing SW i.e., receiving input from BLE app.
One possible method to implement the switch was through Bluetooth communication with a phone using 
third party app like LightBLue or nRF connect mobile app, in our project we have utilized the nRF connect 
Mobile app which allows the user to send commands to indicate a switch press.