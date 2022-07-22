#include <DS1307.h>  //biblioteca RTC
#include <Wire.h>    //biblioteca RTC

#include <SD.h>      //bilbioteca SD card
#include <SPI.h>     //bilbioteca SD card

#include <LiquidCrystal.h>
const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

File myFile;
int pinoSS = 53; // Pin 53 para Mega / Pin 10 para UNO

int rtc[7];  

const int sensorCorrente = A0;  
float sensorValue = 0;    
float currentValue = 0;    
float voltsporUnidade = 0.0048875855327468;
float ruido =0.00;

int boia = 22;
int rele = 24;
int ledred = 26;
int ledgreen = 28;
int botton = 30;

byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2; // 2,3,18,19,20,21
float calibrationFactor = 4.5;
volatile byte pulseCount;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long oldTime;

void setup()
{
Serial.begin(9600);
lcd.begin(16,2);

//RTC.stop();
//RTC.set(DS1307_SEC,00);     //define o segundo
//RTC.set(DS1307_MIN,8);     //define o minuto
//RTC.set(DS1307_HR,21);      //define a hora
//RTC.set(DS1307_DOW,2);      //define o dia da semana
//RTC.set(DS1307_DATE,10);    //define o dia
//RTC.set(DS1307_MTH,2);      //define o mes
//RTC.set(DS1307_YR,20);      //define o ano
//RTC.start();

pinMode(sensorPin, INPUT);
pinMode(rele, OUTPUT);
pinMode(ledred, OUTPUT);
pinMode(ledgreen, OUTPUT);
pinMode(pinoSS, OUTPUT);
pinMode(botton, INPUT);
digitalWrite(sensorPin, HIGH);
  digitalWrite(ledgreen, LOW);
    digitalWrite(ledred, LOW);
      digitalWrite(rele, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);


if (SD.begin()) { // Inicializa o SD Card
Serial.println("SD Card pronto para uso."); // Imprime na tela
  digitalWrite(ledgreen, HIGH);
  delay(3000);
  digitalWrite(ledgreen, LOW);
    } 

  else {
Serial.println("Falha na inicialização do SD Card.");
  digitalWrite(ledred, HIGH);
  delay(3000);
  digitalWrite(ledred, LOW);
return;
  }
// Serial.print(rtc[7]);
}

void loop()
{
RTC.get(rtc,true);
  for(int i=0; i<7; i++)
  {
Serial.print(rtc[4]); //dia
Serial.print(" "); 
Serial.print(rtc[5]); //mes
Serial.print(" ");
Serial.print(rtc[6]); //ano
Serial.print(" ");  
Serial.print(rtc[3]); //dia da semana
Serial.print(" "); 
Serial.print(rtc[2]); //hora
Serial.print(" "); 
Serial.print(rtc[1]); //minuto
Serial.print(" "); 
Serial.print(rtc[0]); //segundo
Serial.println(" ");   
  }

  if ((millis() - oldTime) > 1000)   // Only process counters once per second
  {
    detachInterrupt(sensorInterrupt);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

    oldTime = millis();
    
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;

    unsigned int frac;

    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print(".");             // Print the decimal point
    
    frac = (flowRate - int(flowRate)) * 10;
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");

    Serial.print("  Output Liquid Quantity: ");             // Output separator
    Serial.print(totalMilliLitres);
    Serial.println("mL");
    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

     if(frac, DEC > 0){
  horaAtual(); 
  myFile = SD.open("Dados.txt", FILE_WRITE);
  myFile.print("Flow rate: ");
    myFile.print(int(flowRate));
  myFile.print(".");
    myFile.print(frac, DEC);
  myFile.println("L/min");
      myFile.close();
    }    
  }
//               /*
      //Função liga bomba
      if(rtc[2] == 13 && rtc[1] >= 30){  //horario para ligar
        horaAtual();
        Nivel();
          if(digitalRead(boia) == 1){
             lcd.clear();
              lcd.write("Bomba ativada");
              Serial.print("Bomba ativada");
                digitalWrite(rele, LOW);
                  digitalWrite(ledgreen, HIGH);
                    digitalWrite(ledred, LOW);
                    delay(3000);
                    acs712();
                    if(currentValue <=3){
                      
                     lcd.clear();
                      lcd.write("Corrente insuficiente");
                      Serial.print("Corrente insuficiente");
                      delay(1000);
                      for (int i = 0; i < 12; i++) {   //Rolando o display para a esquerda 12 vezes
                      lcd.scrollDisplayLeft();
                       }
                        digitalWrite(rele, HIGH);
                         digitalWrite(ledgreen, LOW);
                          digitalWrite(ledred, HIGH);
                              } 
                            }                    
                          }
      if(rtc[2] >= 14 /*&& rtc[1] >= 03*/){
        Serial.println("Bomba desativada");
        digitalWrite(rele, HIGH);
          digitalWrite(ledgreen, LOW);
            digitalWrite(ledred, HIGH);
      }

      // Função morte instantânea **Cuidado**
      if(digitalRead(botton) == HIGH){
             lcd.print("SD successfully saved");
             Serial.print("SD successfully saved");
             delay(1000);
             for (int i = 0; i < 12; i++) {   //Rolando o display para a esquerda 12 vezes
              lcd.scrollDisplayLeft();
               }
                digitalWrite(ledgreen, HIGH);
                  digitalWrite(ledred, HIGH);
               while (1) {}
                }
//                */
}                

void acs712()
{
  currentValue =0;
  for(int index =0; index<5000; index++){
    sensorValue = analogRead(sensorCorrente); // le o sensor na pino analogico A0
    sensorValue = (sensorValue -510)* voltsporUnidade; // ajusta o valor lido para volts começando da metada ja que a saída do sensor é vcc/2 para corrente =0
    currentValue = currentValue +(sensorValue/66)*1000; // a saída do sensor 66 mV por amper  
  }
  myFile = SD.open("Dados.txt", FILE_WRITE);
  myFile.print("Corrente = ");
  myFile.println(currentValue);
  myFile.close(); 
}

void Nivel()
{
int estado = digitalRead(boia);  
  Serial.print("Leitura do sensor : ");
  Serial.println(estado);  
  switch(estado)
  {
// ambos os casos não serão usados para automação bomba, necessario apenas variavel digitalRead    
  case 0:    
    Serial.println("Caixa abaixo do nivel minino");
    break;
            
  case 1:    
    Serial.println("Caixa acima do nivel minino"); 
    break;
  }
  myFile = SD.open("Dados.txt", FILE_WRITE);
    if(estado == 1){
      myFile.println("Caixa acima do nivel minino");
    }
    else{
      myFile.println("Caixa abaixo do nivel minino");
    }
   myFile.close();
}

void horaAtual()
{
    myFile = SD.open("Dados.txt", FILE_WRITE);
myFile.print("Date: ");
  myFile.print(rtc[4]);
myFile.print("/");
  myFile.print(rtc[5]);
myFile.print("/");
  myFile.print(rtc[6]);
myFile.print(" Hour ");
  myFile.print(rtc[2]);
myFile.print(":");
  myFile.println(rtc[1]);
myFile.print(":");
  myFile.println(rtc[0]);
myFile.close();
}

void pulseCounter()
{
  pulseCount++;
}