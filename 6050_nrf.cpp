#include "mbed.h"
#include "mpu6050.h"
#include <math.h>
#include "RF24.h"
#include <stdio.h>
Serial pc(USBTX, USBRX);
MPU6050 mpu(D4,D5); //D4,D5
DigitalIn  analog_value(A0);
DigitalOut  lineLED(D8);
//DigitalOut buzzer(D6);

const uint64_t  ADDRESS = 0xE7E7E7E700;
DigitalOut      led(LED1);                             // LED1
RF24            radio(A6,A5,A4,A3,D3);    // mosi, miso, sck, csn, ce, (irq not used) D11,D12,D13,D10,A6,A5,A4,A3,D3
int             payload;
bool            role=1;
bool            radionumber = 1; // 303
int            payload2;


int main() {
    pc.baud(115200);
    double ax, ay, az;
    float TH = 6.0; // 충격 상수
    float meas_r = 0; // 근접센서
    int all = 0;
    double ttemp = 0;
    //buzzer = 0;
    uint8_t count = 0;
    pc.printf("Start!");
    uint8_t wstat;
    int sendValue = 0; // 
    
    uint8_t i = 0;
    
    uint8_t temp = 0; // 00: 0 , 01: 1
    // Initialize nRF24L01
    if (!radio.begin()) { // nrf 초기화 실패시 프린트
        printf("Failed to initialize nRF24L01. Check whether connected.\r\n");
        return -1;  // Exit the program
    }
    radio.setPALevel(RF24_PA_LOW); // nrf 전력 초기화
    radio.setRetries(5, 15);
    radio.setPayloadSize(sizeof(payload)); // 보낼 메세지 사이즈 계산
    radio.setAutoAck(true); // ack 보내기
    //radio.setChannel(62); // channel 설정
    pc.printf("RF init done"); // nrf 초기화 완료 시 프린트
    
    //radio.printDetails();
    
    radio.openWritingPipe(ADDRESS); // 주소 쓰는 부분
    radio.openReadingPipe(0, ADDRESS); // 주소 읽는 부분
    radio.startListening(); // tx 시작
    
    
    mpu.setMaxScale(MAX_ACC_8G, MAX_GYRO_1000degpersec); // 자이로 스케일?
    

   while(1){
        meas_r = analog_value.read(); 
        mpu.readAccelemeter(ax, ay, az);
        all = (sqrt(pow(ax,2)+pow(ay,2)+pow(az,2)));
        sendValue = (all | ((int)meas_r << 4));
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
                radio.stopListening();
                if(all >= TH)//충격 감지 
                {
                        if(radio.write(&payload, 4))
                        { 
                            pc.printf(" %d\n\r",payload,"Succeed");
                        }
                        else pc.printf("%d %s\n\r",payload,"Failed");
                        
                    
                    radio.write("\0",1);
                    lineLED = 1;
                    wait(0.1);
                    lineLED = 0;
                    wait(0.1);
                }
            } // role
        }// 근접
        
        
         
        else
        { // 하이바 벗었을때
            //pc.printf("meas_r 2: %f\n\r",meas_r);
            radio.stopListening();
            
                if(radio.write(&payload2, 4))
                    { 
                        pc.printf("%d\n\r ",payload2,"Succeed");
                    }
                    else 
                    {
                        pc.printf("%d\n\r ",payload2,"Failed");
                    
                    }
                    
            
                    radio.write("\0",1);
                    radio.stopListening();
            
        }
    }// while
}// main끝
     


