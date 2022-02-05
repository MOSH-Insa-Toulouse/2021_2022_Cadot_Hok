/*
 * Author: JP Meijers
 * Date: 2016-02-07
 * Previous filename: TTN-Mapper-TTNEnschede-V1
 *
 * This program is meant to be used with an Arduino UNO or NANO, conencted to an RNxx3 radio module.
 * It will most likely also work on other compatible Arduino or Arduino compatible boards, like The Things Uno, but might need some slight modifications.
 *
 * Transmit a one byte packet via TTN. This happens as fast as possible, while still keeping to
 * the 1% duty cycle rules enforced by the RN2483's built in LoRaWAN stack. Even though this is
 * allowed by the radio regulations of the 868MHz band, the fair use policy of TTN may prohibit this.
 *
 * CHECK THE RULES BEFORE USING THIS PROGRAM!
 *
 * CHANGE ADDRESS!
 * Change the device address, network (session) key, and app (session) key to the values
 * that are registered via the TTN dashboard.
 * The appropriate line is "myLora.initABP(XXX);" or "myLora.initOTAA(XXX);"
 * When using ABP, it is advised to enable "relax frame count".
 *
 * Connect the RN2xx3 as follows:
 * RN2xx3 -- Arduino
 * Uart TX -- 10
 * Uart RX -- 11
 * Reset -- 12
 * Vcc -- 3.3V
 * Gnd -- Gnd
 *
 * If you use an Arduino with a free hardware serial port, you can replace
 * the line "rn2xx3 myLora(mySerial);"
 * with     "rn2xx3 myLora(SerialX);"
 * where the parameter is the serial port the RN2xx3 is connected to.
 * Remember that the serial port should be initialised before calling initTTN().
 * For best performance the serial port should be set to 57600 baud, which is impossible with a software serial port.
 * If you use 57600 baud, you can remove the line "myLora.autobaud();".
 *
 */
  #include <rn2xx3.h>
  #include <SoftwareSerial.h>
  #include <MsTimer2.h>

  #define APP_EUI "70B3D57ED0036E7D"
  #define APP_KEY "FA3B31FF8A4DD870112C4502A0347A66"

  #define GASPIN A1
  #define red_pin 3
  #define green_pin 4
  #define blue_pin 5

  #define MIN_VOLTAGE 1.5
  #define VOLTAGE_ALERT 3.0

  #define interrupt_pin 2

  #define potentio_pin A2

  #define lora true
  
  #define outpin A3

SoftwareSerial mySerial(10, 11); // RX, TX

volatile float voltage = 0.0;
volatile bool alert = false;

//create an instance of the rn2xx3 library,
//giving the software serial as port to use
rn2xx3 myLora(mySerial);

// the setup routine runs once when you press reset:
void setup()
{
  //output LED pin
  pinMode(13, OUTPUT);
  led_on();

  //gas sensor
  pinMode(GASPIN, INPUT);

  //rgb
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);

  //potentio
  pinMode(potentio_pin, INPUT);

  //outpin
  pinMode(outpin, INPUT);

  // Open serial communications and wait for port to open:
  Serial.begin(57600); //serial port to computer
  mySerial.begin(9600); //serial port to radio
  Serial.println("Startup");

  if (lora) initialize_radio();

  // Interruption

  pinMode(interrupt_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interrupt_pin), myISR, HIGH); 

  // Periodic send
  MsTimer2::set(60000, myISR); // 60000ms period
  MsTimer2::start();
  
  led_off();
}

// LoRa

void initialize_radio()
{
  //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;


  /*
   * ABP: initABP(String addr, String AppSKey, String NwkSKey);
   * Paste the example code from the TTN console here:
   */
//  const char *devAddr = "02017201";
//  const char *nwkSKey = "AE17E567AECC8787F749A62F5541D522";
//  const char *appSKey = "8D7FFEF938589D95AAD928C2E2E7E48F";
//
//  join_result = myLora.initABP(devAddr, appSKey, nwkSKey);

  /*
   * OTAA: initOTAA(String AppEUI, String AppKey);
   * If you are using OTAA, paste the example code from the TTN console here:
   */
  const char *appEui = APP_EUI;
//  const char *appKey = "5902794DA4B51AA42FF82DE5EA382D06";
  const char *appKey = APP_KEY;

  join_result = myLora.initOTAA(appEui, appKey);


  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");

}

void myISR() {
  alert = true;
}

void send_gas_value() {
  String data = String(voltage);
  int tx = 0;
  if (lora) tx = myLora.tx(data); //one byte, blocking function
  if (tx == 0)
    Serial.println("Could not send message");
  else 
    Serial.println("Successfully sent : " + data);
}

// RGB

volatile void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
 {
  analogWrite(red_pin, red_light_value);
  analogWrite(green_pin, green_light_value);
  analogWrite(blue_pin, blue_light_value);
}

void blink() {
  RGB_color(255, 0, 0); // Red
  delayMicroseconds(500);
  RGB_color(0, 255, 0); // Green
  delayMicroseconds(500);
  RGB_color(0, 0, 255); // Blue
  delayMicroseconds(500);
  RGB_color(255, 255, 125); // Raspberry
  delayMicroseconds(500);
  RGB_color(0, 255, 255); // Cyan
  delayMicroseconds(500);
  RGB_color(255, 0, 255); // Magenta
  delayMicroseconds(500);
  RGB_color(255, 255, 0); // Yellow
  delayMicroseconds(500);
  RGB_color(255, 255, 255); // White
  delayMicroseconds(500);
}

void led_gradient() {
  float rate = (voltage - MIN_VOLTAGE)/ VOLTAGE_ALERT;
  int green_led_value = 255 * (1 - rate);
  // Gradient from yellow to red
  RGB_color(255, green_led_value, 0);
}

// the loop routine runs over and over again forever:
void loop()
{
  led_on();
  Serial.println("Potentio : " + String((analogRead(potentio_pin) / 1024.0)* (5.0-3.3) + 3.3));
  if (!alert) {
    voltage = getGasData();
    String str = String(voltage);
    if (voltage > VOLTAGE_ALERT) {
      RGB_color(255,0,0);
      // TODO TX LoRa
    } else if (voltage < MIN_VOLTAGE) {
      RGB_color(0,0,255); 
    } else {
      // LED Gradient according to the gas sensor's value
      led_gradient();
    }
  } else {
    send_gas_value();
    alert = false;
  }

  //blink();
  led_off();
  delay(500);
}

float getGasData() {
  float sensor_volt = 0;//=(float)analogRead(GASPIN)/1024*5.0;
  float sensorValue = 0;
  /*--- Get a average data by testing 100 times ---*/
  float out = 0;
  for(int x = 0 ; x < 100 ; x++)
  {
      sensorValue = sensorValue + analogRead(GASPIN);
      out = out + analogRead(outpin);
  }
  sensorValue = sensorValue/100.0;
  Serial.println("OUTPIN = " + String(out/100.0/1024*5.0));
  /*-----------------------------------------------*/
  sensor_volt = sensorValue/1024*5.0;
  Serial.println("Sensor gas voltage = " + String(sensor_volt));    
  return sensor_volt;
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}
