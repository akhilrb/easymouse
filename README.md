# easymouse
A serial based pointing device controlled by a wearable.


## Driver

Not exactly a driver, but an interpreter of sorts.
A Python script manages the inputs received from the wearable.<br>

#### Features:

* Can differentiate between Unicode-Decode-Error and Serial-Exception
* Automatically reconnects on byte mismatch due to Serial Error
* Requests port reconfiguration if UDE is encountered
* Optionally configurable verbose output


## Wearable

Currently, an Arduino and MPU-6050 is used to detect hand movement.<br>
Custom buttons provide quick functions, which can be edited in the driver.<br>