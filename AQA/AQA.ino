

// **** INCLUDES *****
#include <rn2xx3.h>
#include "LowPower.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 15

OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

#define Offset 0.00            //deviation compensate
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40    //times of collection
#define RadioPin 9
#define SensorPin 8

void switchRadio(bool radioOn)
{
  digitalWrite(RadioPin, radioOn);
}
void switchSensors(bool sensorOn)
{
  digitalWrite(SensorPin, sensorOn);
}

int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;    

int kondPin = A0;
int turbPin = A2;
int phPin   = A1;

rn2xx3 myLora(Serial1); 

void setup()
{
  // Enter idle state for 8 s with the rest of peripherals turned off
  // Each microcontroller comes with different number of peripherals
  // Comment off line of code where necessary
  
  // ATmega32U4 ******
  
  // Open serial communications and wait for port to open:
  
  Serial.begin(19200); //serial port to computer
  Serial1.begin(9600); //serial port to radio

  delay(2000);

  //Pin 9 is controlling the pwr to the radio
  pinMode(RadioPin, OUTPUT);
  digitalWrite(RadioPin,LOW);
  //Pin 8 is controlling the pwr to the sensors
  pinMode(SensorPin, OUTPUT);
  digitalWrite(SensorPin,LOW);
  
  initialize_radio();
  //transmit a startup message
  //myLora.tx("Initiated Node");
  
}

bool initialize_radio()
{
  //reset rn2483
  
  switchRadio(true);

  LowPower.idle(SLEEP_120MS, ADC_ON, TIMER4_OFF, TIMER3_OFF, TIMER1_ON, 
           TIMER0_ON, SPI_OFF, USART1_ON, TWI_OFF,USB_ON);
  //wait for the RN2xx3's startup message
  Serial1.flush();
  
  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  byte tries;
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    //Resets the radio
    switchRadio(false);
    LowPower.idle(SLEEP_1S, ADC_ON, TIMER4_OFF, TIMER3_OFF, TIMER1_ON, 
           TIMER0_ON, SPI_OFF, USART1_OFF, TWI_OFF,USB_ON);
    switchRadio(true);
    LowPower.idle(SLEEP_120MS, ADC_ON, TIMER4_OFF, TIMER3_OFF, TIMER1_ON, 
           TIMER0_ON, SPI_OFF, USART1_ON, TWI_OFF,USB_ON);
    Serial1.flush();
    
    myLora.autobaud();
    LowPower.idle(SLEEP_4S, ADC_ON, TIMER4_OFF, TIMER3_OFF, TIMER1_ON, 
           TIMER0_ON, SPI_OFF, USART1_ON, TWI_OFF,USB_ON);
    hweui = myLora.hweui();
    tries++;
    if(tries > 10)
    {
      switchRadio(false);
      return false;
    }
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
   * OTAA: initOTAA(String AppEUI, String AppKey);
   * If you are using OTAA, paste the example code from the TTN console here:
   */
  const char *appEui = "70B3D57ED0007AD4";
  const char *appKey = "E3184346A03F2F3DB359EA3BDC51FD5C";
  join_result = myLora.initOTAA(appEui, appKey);
  if(!join_result)
  {
    switchRadio(false);
    return false;
  }
  Serial.println("Successfully joined TTN");
  return true;
}

// the loop routine runs over and over again forever:
int timer = 0;
void loop()
{
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  
    operate();
    timer = 0;
    
}

void operate()
{
  if(initialize_radio() == false)
  {
    switchRadio(false);
    return;
  }
  switchSensors(true);
  LowPower.idle(SLEEP_250MS, ADC_ON, TIMER4_OFF, TIMER3_OFF, TIMER1_ON, 
           TIMER0_ON, SPI_OFF, USART1_ON, TWI_OFF,USB_ON);
  sensors.begin();
  Serial.println("Gathering data");
  LowPower.idle(SLEEP_1S, ADC_ON, TIMER4_OFF, TIMER3_OFF, TIMER1_ON, 
           TIMER0_ON, SPI_OFF, USART1_ON, TWI_OFF,USB_ON);
  sensors.requestTemperatures();
    
  String payload = "";
  String konduktivitet = String(analogRead(kondPin));
  String turbiditet    = String(analogRead(turbPin));
  String ph            = String(analogRead(phPin));
  String temperatur  = String(sensors.getTempCByIndex(0));
  
  payload = konduktivitet + "," + turbiditet + "," + ph +","+ temperatur;
  Serial.println("Transmitting data");
  myLora.tx(payload);
  LowPower.idle(SLEEP_250MS, ADC_ON, TIMER4_OFF, TIMER3_OFF, TIMER1_ON, 
           TIMER0_ON, SPI_OFF, USART1_ON, TWI_OFF,USB_ON);
  switchSensors(false);
  switchRadio(false);
  
}

