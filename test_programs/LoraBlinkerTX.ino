/*
 * Author: JP Meijers
 * 30 September 2016
 */

#include <SoftwareSerial.h>

SoftwareSerial loraSerial(10, 11);

String str;

void setup() {
  //output LED pin
  pinMode(13, OUTPUT);
  led_off();
  
  // Open serial communications and wait for port to open:
  
  Serial.begin(57600);
  
  loraSerial.begin(9600);
  loraSerial.setTimeout(1000);
  lora_autobaud();
  
  led_on();
  delay(1000);
  led_off();

  Serial.println("Initiating LoRa");
  
  loraSerial.listen();
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  loraSerial.println("sys get ver"); //p16
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("mac pause"); //p22
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
//  loraSerial.println("radio set bt 0.5");
//  wait_for_ok();
  
  loraSerial.println("radio set mod lora"); //p40
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set freq 869100000"); //p40
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set pwr 14"); //p41
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set sf sf7"); //p41
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set afcbw 41.7");//p41
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set rxbw 125");//p41
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
//  loraSerial.println("radio set bitrate 50000");
//  wait_for_ok();
  
//  loraSerial.println("radio set fdev 25000");
//  wait_for_ok();
  
  loraSerial.println("radio set prlen 8"); //p42
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set crc on");//p42
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set iqi off");//p42
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set cr 4/5");//p42
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set wdt 60000"); //disable for continuous reception //p43
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set sync 12"); //p43
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  
  loraSerial.println("radio set bw 125"); //p43
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);

  Serial.println("starting loop");
}

void loop() {
  led_on();
  //loraSerial.println("radio tx 20"); //p39
  loraSerial.println("radio tx 48656c6C6F"); //p39 Transmit Hello
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  str = loraSerial.readStringUntil('\n');
  Serial.println(str);
  led_off();
  delay(200);
}

void lora_autobaud()
{
  String response = "";
  while (response=="")
  {
    delay(1000);
    loraSerial.write((byte)0x00);
    loraSerial.write(0x55);
    loraSerial.println();
    loraSerial.println("sys get ver");
    response = loraSerial.readStringUntil('\n');
  }
}

/*
 * This function blocks until the word "ok\n" is received on the UART,
 * or until a timeout of 3*5 seconds.
 */
int wait_for_ok()
{
  str = loraSerial.readStringUntil('\n');
  if ( str.indexOf("ok") == 0 ) {
    return 1;
  }
  else return 0;
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}
