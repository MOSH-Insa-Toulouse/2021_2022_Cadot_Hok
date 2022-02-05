# 2021_2022_Cadot_Hok

# Mini ISS project: LoRa gas sensor
## 1- Green track: LoRa communication
We tested the send and receive functions of the LoRa module by sending raw commands (see _LoRaBlinkerTX.ino_ and _LoRaBlinkerRX.ino_). Then, we used The Things Network to create an application where we added our end device with the information we got from the _DeviceInfo.ino_ program provided by TheThingsNetwork arduino library.

![image](https://user-images.githubusercontent.com/55708764/152640016-b9ba3d7e-3fbe-475b-9fb7-583ef6a1ae15.png)

We added a decoded in the application, converting ASCII codes to recognizable text:

![image](https://user-images.githubusercontent.com/55708764/152640048-8187989a-2f98-4e14-9180-2ac3669f694b.png)

**Bonus**  
In the _Downlink.ino_ program, we tested the downlink part of TTN. We convert the received data to text corresponding with its HEX codes.

## 2- Blue track: Connected LoRaWAN application
We chose to use the LM741 AOP like this:
* Vref: output of a potentiometer used to configure the alarm threshold for gas detection
* Vin: output of our gas sensor
* Output: 0V -> 5V when Vin > Vref (means that we exceed the threshold)

The output of the comparative part is connected to pin 2 of the Arduino Uno because this pin is interrupt capable. When an interrupt is detected, we send the corresponding gas value with LoRa. Besides, we send each minute the gas sensor data for monitoring purpose.

To visualise the gas detection, we set up a RGB LED:
* Blue -> no danger, below a given non-detection threshold
* Yellow to Red -> evolving from one to the other given the output of the gas sensor from non-detection to 5V

You can watch the [provided demonstration video](https://github.com/MOSH-Insa-Toulouse/2021_2022_Cadot_Hok/blob/main/demonstration_video.mp4) to see our breadboard system working.
We observed the following messages on TTN when filming the demonstration (3 periodic messages then an alert one):

![image](https://user-images.githubusercontent.com/55708764/152640504-919f4db8-500b-4372-9535-e3817cbb39ef.png)

## 3- Red track: Energy management
We did not realise this part as we thought our system was simple and small. We chose to focus on the black track.

## 4- Black track: Node-RED dashboard
![image](https://user-images.githubusercontent.com/55708764/152640737-65287892-41c7-4117-8d98-80d7ed08e129.png)

LED is blue at the beginning, we spray some gas, it goes red and slowly goes from red to yellow and finally returns to blue. We can see the corresponding behaviour on the image above.

![image](https://user-images.githubusercontent.com/55708764/152640747-ca9d8f6f-48fc-4b55-b850-008d96c8e567.png)

We used MQTT to connect and subscribe to our device on TTN.  
