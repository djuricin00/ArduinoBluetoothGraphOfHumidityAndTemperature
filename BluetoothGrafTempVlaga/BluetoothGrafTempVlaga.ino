/*
 * bt tx - 3
 * bt rx - 2
 * -----------
 * dht11 left pin +
 * dht11 rigth pin -
 * dht 2nd pin 7
 * dht 1st - 2nd pin resistor 10kohm
 * WRITTEN BY : NIKOLA DJURICIN
 * DATE : 03.06.2018
 */
#include <SoftwareSerial.h> // 
#include <dht.h>

#define Rx 3
#define Tx 2

SoftwareSerial bluetooth(Rx,Tx);
dht DHT;

void setup() 
{
  Serial.begin(9600);
  bluetooth.begin(9600);
}

byte r,s,ostatak;
int temp[24],vlaga[24],p,brojac=-1;
double srednjaVrednostTemp=999,srednjaVrednostVlaga=999;
bool prolazak = false; 

void loop() 
{
  for(int i=0; i<24; i++)
  {
    if(i==0)
    {
      p = DHT.read11(7);
      r = DHT.temperature;
      Serial.print("t = ");
      Serial.println(r);
      temp[i] = r;
      r = DHT.humidity;
      Serial.print("v = ");
      Serial.println(r);
      Serial.println();
      vlaga[i] = r;
    }
    if(i>0)
    {
      r = srednjaVrednostTemp/brojac; // davanje srednje vrednosti temperature u poslednjem satu
      temp[i] = r;
      r = srednjaVrednostVlaga/brojac; // davanje srednje vrednosti vlage u poslednjem satu
      vlaga[i] = r;
    }

    if(i == 23){prolazak = true;}
    
    for(int j=0; j<3600; j++) // delay 1 hour
    {
      if(bluetooth.available()) 
      {
        s = bluetooth.read();
        Serial.println(s);
        if(s == 24) // keyword od button-a sa aplikacije
        {
          ostatak = (3600-j)/60; // do sledeceg ocitavanje je ostalo jos[m]
          bluetooth.write(ostatak);
          Serial.println(ostatak);
          if(!prolazak) 
          {
            for(int k=0; k<=i; k++){poruka(k);j+=2;} // prvi graf, ne dovrsen
          }
          
          else if(prolazak) // redovan graf
          {
            for(int k=i+1; k<24; k++){poruka(k);j+=2;} // stare vrednosti sa vecim rednim brojem
            for(int k=0; k<=i; k++){poruka(k);j+=2;} // stare vrednosti sa manjim rednim brojem
          }
        }
      }
      else
      {
        delay(1000);
       // Serial.println(j);
        if(j%60 == 0)// every min get the values
        {
          p = DHT.read11(7);
          srednjaVrednostVlaga+= DHT.humidity;
          srednjaVrednostTemp+= DHT.temperature; 
          Serial.println(srednjaVrednostVlaga);
          Serial.println(srednjaVrednostTemp);
          brojac++;
          Serial.println(brojac);
        }
      }
    }
  }
}

void poruka(int j) // f-ja
{
  bluetooth.write(200+temp[j]);
  Serial.println(temp[j]);
  delay(1000);
  Serial.println(vlaga[j]);
  bluetooth.write(vlaga[j]);
  delay(1000);
}
