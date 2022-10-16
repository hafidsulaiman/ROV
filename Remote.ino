#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
RF24 radio(9,10);
const byte address [][6] = {"00001","00002"}; // 00001 transmit, 00002 receive
//button declaration
int maju = 2;
int mundur = 3;
int kiri = 4;
int kanan = 5;
int naik = 6;
int turun = 7;
int pompa = 8;
int pot = A3;
float arah;
int gerak;
int updwn;
long int data;
bool pump;
int kecepatan;
int kec;
int sum;
int sum2;
int message[4];
int updown[2];
float text;
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
void setup() {
    lcd.init();
    lcd.backlight();
    pinMode(maju,INPUT);
    pinMode(mundur,INPUT);
    pinMode(kiri,INPUT);
    pinMode(kanan,INPUT);
    pinMode(naik,INPUT);
    pinMode(turun,INPUT);
    pinMode(pot,INPUT);
    pinMode(pompa,INPUT);
    radio.begin();
    radio.openWritingPipe(address[1]); //send address data
    radio.openReadingPipe(1, address[0]); //receive address data
    radio.setPALevel(RF24_PA_HIGH);
    lcd.print("LCD Bisa!");
    Serial.begin(9600);
}
void loop() {
    delay(5);
        radio.startListening();
        if (radio.available()){
        //Read MPU
            radio.read(&arah, sizeof(arah));
            lcd.clear();
            lcd.print(arah);
        }
        delay(5);
        //send command to robot
        radio.stopListening();
        kecepatan = analogRead(pot);
        kec = map(kecepatan, 0, 1023, 0, 255);
        message[0] = digitalRead(maju);
        message[1] = digitalRead(mundur);
        message[2] = digitalRead(kiri);
        message[3] = digitalRead(kanan);
        int sum = message[0]+message[1]+message[2]+message[3];
            if (sum==0 || sum>1) {
                //stop
                gerak=0;
            }
            if (sum==1) {
                if (message[0]==1 && message[1]==0 && message[2]==0 && message[3]==0) {
                    //move forward
                    gerak=1;
                }
                if (message[0]==0 && message[1]==1 && message[2]==0 && message[3]==0) {
                    //move backward
                    gerak=2;
                }
                if (message[0]==0 && message[1]==0 && message[2]==1 && message[3]==0) {
                    //move right
                    gerak=3;
                }
                if (message[0]==0 && message[1]==0 && message[2]==0 && message[3]==1) {
                    //move left
                    gerak=4;
                }
            }
        updown[0] = digitalRead(naik);
        updown[1] = digitalRead(turun);
        int sum2 = updown[0]+updown[1];
            if (sum2==0 || sum2>1) {
                //stop
                updwn=0;
            }
            if (sum2==1) {
                if (updown[0]==1 && updown[1]==0) {
                    //go up
                    updwn=1;
                }
                if (updown[0]==0 && updown[1]==1) {
                    //go down
                    updwn=2;
                }
            }
    pump = !digitalRead(pompa);
    data=kec*pow(10,3)+gerak*pow(10,2)+updwn*pow(10,1)+pump;
    Serial.println(data);
    radio.write(&data, sizeof(data));
}
