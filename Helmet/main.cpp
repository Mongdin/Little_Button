#include "mbed.h"
#include "mpu6050.h"
#include <math.h>
#include "RF24.h"
#include <stdio.h>
Ticker flipper;
Ticker lled;
InterruptIn emer(D7);
Serial pc(USBTX, USBRX);
MPU6050 mpu(D4,D5); //D4,D5
DigitalIn  analog_value(A0);
DigitalOut  lineLED(D8);
DigitalOut testled(D10);
DigitalOut emerled(D9);
uint16_t emergency_value = 1024;
int lledcount = 0;
const uint64_t  ADDRESS = 0xF0F0F0F000;
DigitalOut      led(LED1);                             // LED1
RF24            radio(A6,A5,A4,A3,D3);    // mosi, miso, sck, csn, ce, (irq not used) D11,D12,D13,D10,A6,A5,A4,A3,D3
int             payload;
bool            role=1;
bool            radionumber = 1; // 303
int            payload2;

void lledflip() {
    if(lledcount>0){
    if(lledcount <= 10) {lineLED = 1; lledcount++;}
    else {lledcount = 0; lineLED = 0;}
    }
}

void emerflip() {
      pc.printf("Emergency TX Started\n\r");
      while(1) {
          
          if(radio.write(&emergency_value,2)) {pc.printf("%d Emergency TX Secceed\n\r",emergency_value); wait(0.5); break;}
          else pc.printf("%d Emergency TX Failed\n\r",emergency_value);
          
    };//응급상황표시(1024)
    if (lledcount == 0)lledcount = 1;  
}

void flip() {
      while(!radio.write(&payload, 2)){ pc.printf("%d Timed TX Failed\n\r",payload); wait(0.5);} //센서값 전송
      pc.printf("%d Timed TX Secceed\n\r",payload);
      wait(0.5);
}



int main() {
    pc.baud(115200);
    emer.fall(&emerflip);
    flipper.attach(&flip, 5.0); //ticker init
    lled.attach(lledflip,1.0);
    double ax, ay, az;
    float TH = 60.0; // 충격 상수
    float meas_r = 0; // 근접센서
    int all = 0;
    double ttemp = 0;
    //buzzer = 0;
    uint8_t count = 0;
    pc.printf("Start!");
    uint8_t wstat;
    int sendValue = 0; // 
    testled = 0;
    lineLED = 0;
    
    
    uint8_t i = 0;
    
    uint8_t temp = 0; // 00: 0 , 01: 1
    // Initialize nRF24L01
    if (!radio.begin()) { // nrf 초기화 실패시 프린트
        printf("Failed to initialize nRF24L01. Check whether connected.\r\n");
        return -1;  // Exit the program
    }
    radio.setPALevel(RF24_PA_LOW); // nrf 전력 초기화
    radio.setRetries(5, 15);
    radio.setPayloadSize(2); // 보낼 메세지 사이즈 계산
    radio.setAutoAck(true); // ack 보내기
    radio.setChannel(62); // channel 설정
    pc.printf("RF init done"); // nrf 초기화 완료 시 프린트
    
    radio.printDetails();
    
    radio.openWritingPipe(ADDRESS); // 주소 쓰는 부분
    radio.openReadingPipe(0, ADDRESS); // 주소 읽는 부분
    //radio.startListening();
    
    
    mpu.setMaxScale(MAX_ACC_8G, MAX_GYRO_1000degpersec); // 자이로 스케일?
    

   while(1){
        meas_r = analog_value.read(); 
        mpu.readAccelemeter(ax, ay, az);
        all = 10*(sqrt(pow(ax,2)+pow(ay,2)+pow(az,2)));
        sendValue = (all | ((int)meas_r << 8));
        //printf("value: 0x%x, %d\n\r",sendValue,sendValue );
        payload = sendValue;
        payload2 = (int)meas_r;
        //sprintf(payload,"shock\n\r");
        //printf("all: %lf\n\r",all);
        //sprintf(payload2,"NONE\n\r");
        
        if(!meas_r) { // 하이바 썼을때
             //pc.printf("meas_r: %f\n\r",meas_r);
            
            if (role == 1)//Tx
            {
             // Send led's status to slave
                
                if(all >= TH)//충격 감지 
                {
                    while(!radio.write(&payload, 2)) {pc.printf("%d TX Failed\n\r",payload); wait(0.5);}
                    pc.printf("%d TX Succeed\n\r",payload);
                    if (lledcount == 0)lledcount = 1;
                }
            } // role
        }// 근접
        
    }// while
}// main끝
     