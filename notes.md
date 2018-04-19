# General hardware considerations
We can use Nano's ICSP power pins to get power for bluetooth module. Digital pins are not suitable to supply power for HC-05.


# Analog (and digital) pins as power pins
Setting pin 19 (A5) as HIGH and pin 18 (A4) as LOW provides the 5V with few milliamps needed by the accelerometer to work.

    pinMode(groundpin, OUTPUT);
    pinMode(powerpin, OUTPUT);
    digitalWrite(groundpin, LOW);
    digitalWrite(powerpin, HIGH); 


# URM37 v3.2
Note: them doesn't need any configuration or mode change to work with NewPing library. Just connect Trig and PWM pins and that's it.


# HC-05 bluetooth module
Specifications:
Bluetooth protocol: Bluetooth Specification v2.0 + EDR
Rate: 2.1Mbps(Max)/160 kbps(Asynchronous);1Mbps/1Mbps(Synchronous)
Support profiles: Bluetooth serial port (master & slave)
Power Supply: +3.3 VDC 50mA

One thing to note is the power consumption; 3.3v at 50mA. I do not know if 50mA is constant or if it is the maximum the board can draw (likely it is the maximum) and this too much for a regular Arduino pin to supply so the modules should not be connected to a regular Arduino pin.


# HC-05 and Ubuntu
Check is there any device binded

    $ rfcomm show -a

    rfcomm0: 20:16:10:09:14:26 channel 1 closed 

Find out bluetooth MAC-address

    $ hcitool scan

    Scanning ...
	    20:16:10:09:14:26	hc01.com HC-05

Bind using rfcomm

    $ sudo rfcomm bind 0 20:16:10:09:14:26 1

Now we could connect to HC-05 with terminal. For example, CuteCom

    $ sudo apt-get install cutecom
    $ cutecom

Choose same baudrate on both sides. Ensure, that blueman or other programs not using TTY asigned to your /dev/rfcommX device (where X is number of rfcomm connection, 0 in our case). Check it with 

    $ sudo lsof | grep /dev/rfcomm0

If there any of processes, that are using your rfcomm, you can kill them or call

    $ sudo rfcomm release 0

If rfcomm still exists, restart computer, 

    $ rfcomm show 0 
    
shouldn't return our connection. If so, you can bind again and use your device.

The main idea, that bluetooth module should not be connected to computer, when you want to open serial connection! If you create bind with blueman, again, you shouldn't create connection with it, just open serial port.

If there are problems with connection, also ensure, that /dev/rfcommX has group dialout and your user is in this group

    $ ll /dev | grep rfcomm0

You should be able to send and recieve data to and from bluetooth connection with

    $ cat /dev/rfcomm0
    $ echo "Hello!" > /dev/rfcomm0


# Create symlink
ln -s /srv/git/c_library_v1 mavlink


# Mavlink
* * * 
Here is the sequence I use do override remote control:

-> Heartbeat - To introduce myself as a ground station ;
-> Arm Command
-> Wait a few seconds
-> Override RC Commands

As a Ground station, I use the sys_id 253.

* * *
In my investigation I concluded that you need two things to make it works. First of all, you need the correct system ID which is saved onto the APM or PIXHAWK. So, with Mission Planner software you can check this to be sure you are using the correct ID. Here you have a picture example (http://ardupilot.org/copter/docs/parameters.html#sysid-mygcs-my-ground-station-number):

Allows restricting radio overrides to only come from my ground station

Value 	Meaning
255 	Mission Planner and DroidPlanner
252 	AP Planner 2

The second thing you have to do is to send a HearBeat message every second or two seconds, if possible. This may inform UAV that you are still there and have not lost the communication. This last step is very important because the Ardupilot have a dedicated Failsafe that checks the communication between the UAV and the GCS if the RC_OVERRIDE command is working.


# VS Code properties
If using Uno/Nano (with ATmega328P), set in c_cpp_properties.json:
    in includePath - "${ARDUINO_DIR}/hardware/arduino/avr/variants/standard
    in defines - "__AVR_ATmega328P__"

If using Mega instead, then set:
    in includePath - "${ARDUINO_DIR}/hardware/arduino/avr/variants/mega
    in defines - "__AVR_ATmega2560__" (not sure about this)
