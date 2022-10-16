#include <Wire.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10); // CE, CSN
const byte address[][6] = {"00001", "00002"}; // 00002 transmit, 00001 receive
const int MPU = 0x68; // MPU6050 I2C address
float GyroZ;
float yaw;
float elapsedTime, currentTime, previousTime;
int c = 0;
float arah;

//Deklarasi pin
const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 7;
const int ENA = 5;
const int ENB = 6;
const int relay = 8;
const int IN5 = A2;
const int IN6 = A3;

int pump;
int kec;
int gerak;
int updwn;
long int data;

int expiretime = 0;

void setup() {
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);
  pinMode(relay,OUTPUT);
  pinMode(IN5,OUTPUT);
  pinMode(IN6,OUTPUT);
  Wire.begin();                      // Inisialisasi komunikasi
  Wire.beginTransmission(MPU);  // Mulai komunikasi MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Komunikasi register 6B
  Wire.write(0x00);                  
  Wire.endTransmission(true);        // End transmission
  radio.begin();
  radio.openWritingPipe(address[0]);      //Address kirim data
  radio.openReadingPipe(1, address[1]);   //Address trima data
  radio.setPALevel(RF24_PA_HIGH);
  Serial.begin(19200);
  
}

void loop() {
  delay(5);
  radio.stopListening();
  //Kirim arah MPU
  baca_mpu();
  arah = yaw;
  radio.write(&arah, sizeof(arah));
  Serial.print(yaw);
  
  radio.startListening();
      expiretime=0;
      while(!radio.available() && expiretime < 1000){
        expiretime++;
      }
            //Menerima data
            radio.read(&data, sizeof(data));
            kec = data/1000;
            data=data%1000;
            gerak = data/100;
            data=data%100;
            updwn = data/10;
            data=data%10;
            pump = data;

            //Set kecepatan
            analogWrite(ENA, kec); //ENA pin
            analogWrite(ENB, kec); //ENB pin
            Serial.print("Kecepatan: ");
            Serial.print(kec);

            //Gerak maju mundur kiri kanan
            if (gerak==0) {
                  //berhenti
                  digitalWrite(IN1,LOW);
                  digitalWrite(IN2,LOW);
                  digitalWrite(IN3,LOW);
                  digitalWrite(IN4,LOW);
                  Serial.print(", Gerak: stop");                  
            }
            if (gerak==1) {
                        //maju
                        digitalWrite(IN1,HIGH);
                        digitalWrite(IN2,LOW);
                        digitalWrite(IN3,LOW);
                        digitalWrite(IN4,HIGH);
                        Serial.print(", Gerak: maju");
            }
            if (gerak==2) {
                        //mundur
                        digitalWrite(IN1,LOW);
                        digitalWrite(IN2,HIGH);
                        digitalWrite(IN3,HIGH);
                        digitalWrite(IN4,LOW);
                        Serial.print(", Gerak: mundur");
            }
            if (gerak==3) {
                        //kiri
                        digitalWrite(IN1,LOW);
                        digitalWrite(IN2,LOW);
                        digitalWrite(IN3,LOW);
                        digitalWrite(IN4,HIGH);
                        Serial.print(", Gerak: kiri");
            }
            if (gerak==4) {
                        //kanan
                        digitalWrite(IN1,HIGH);
                        digitalWrite(IN2,LOW);
                        digitalWrite(IN3,LOW);
                        digitalWrite(IN4,LOW);
                        Serial.print(", Gerak: kanan");
            }
            
            //Gerak naik turun
            if (updwn==0) {
                  //berhenti
                  digitalWrite(IN5,LOW);
                  digitalWrite(IN6,LOW);
                  Serial.print(", NaikTurun: stop");
            }
            if (updwn==1) {
                        //naik
                        digitalWrite(IN5,HIGH);
                        digitalWrite(IN6,LOW);
                        Serial.print(", NaikTurun: naik");
            }
            if (updwn==2) {
                        //turun
                        digitalWrite(IN5,LOW);
                        digitalWrite(IN6,HIGH);
                        Serial.print(", NaikTurun: turun");
            }
            
            //Nyala atau mati pompa
            digitalWrite(relay, pump);
            Serial.print(", Pump: ");
            Serial.println(pump);
}

void baca_mpu()
{
  previousTime = currentTime;        // Menyimpan waktu sebelumnya
  currentTime = millis();            // Membaca waktu sekarang
  elapsedTime = (currentTime - previousTime) / 1000; // Dibagi 1000 menjadi detik
  Wire.beginTransmission(MPU);
  Wire.write(0x47); // Register data pertama GyroZ address 0x47
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 2, true); // Membaca 2 register total
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = GyroZ + 0.79; // GyroErrorZ ~ (-0.8)
  yaw =  yaw + GyroZ * elapsedTime;
  }
