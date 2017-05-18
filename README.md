# Ultrasonic Obstacle Control
End-of-degree engineering Electronic and Automatic Engineering

The following code has been made for an Arduino board.
It uses the "NewPing" library to measure the distance of 5 ultrasonic sensors HC-SR04.
4 Sensors to measure lateral distances and one to measure height.
If the lower sensor measures more than 1.5 meters, the control begins to operate. If any of the side sensors measures a distance of less than 1.5 meters, send a RCOverride MAVLINK command, which overwrites the inputs that the APM receives from the flight controller.