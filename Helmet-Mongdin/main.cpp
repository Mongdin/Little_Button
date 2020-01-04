#include "mbed.h"
#include "mpu6050.h"
#include <math.h>
#include "RF24.h"
#include <stdio.h>
Serial pc(USBTX, USBRX);
MPU6050 mpu(D4,D5); //D4,D5
//AnalogIn analog_value(A0);
//DigitalOut buzzer(D6);

const uint64_t  ADDRESS = 0xE7E7E7E700;
DigitalOut      led(LED1);                             // LED1
RF24            radio(D11,D12,D13,A3,D6);    // mosi, miso, sck, csn, ce, (irq not used)
char         payload[32];
bool            role=1;
bool            radionumber = 1; // 303



int main() {
    pc.baud(115200);
    double ax, ay, az;
    float TH = 1.0;
    float meas_r = 0;
    double all = 0;
    double ttemp = 0;
    //buzzer = 0;
    uint8_t count = 0;
    pc.printf("Start!");
    uint8_t wstat;
    
    uint8_t i = 0;
    //payload = 'G';
    
    uint8_t temp = 0; // 00: 0 , 01: 1
    // Initialize nRF24L01
    if (!radio.begin()) {
        printf("Failed to initialize nRF24L01. Check whether connected.\r\n");
        return -1;  // Exit the program
    }
    radio.setPALevel(RF24_PA_LOW);
    radio.setRetries(5, 15);
    radio.setPayloadSize(sizeof(payload));
    radio.setAutoAck(true);
    radio.setChannel(125);
    pc.printf("RF init done");
    
    radio.printDetails();
    
    radio.openWritingPipe(ADDRESS);
    radio.openReadingPipe(0, ADDRESS);
    radio.startListening();
    
    uint8_t testtest = 'G';
    
    mpu.setMaxScale(MAX_ACC_8G, MAX_GYRO_1000degpersec);
    while(true) {
        //meas_r = analog_value.read();
        mpu.readAccelemeter(ax, ay, az);
        all = (sqrt(pow(ax,2)+pow(ay,2)+pow(az,2)));
        pc.printf(" X = %.4lf7\n\r Y = %.4lf\n\r Z = %.4lf\r\n\n\r", ax, ay, az);
        //sprintf(payload," X = %.4lf7\n\r Y = %.4lf\n\r Z = %.4lf\r\n\n\r", ax, ay, az);
        sprintf(payload,"X=%.4lf7 Y=%.4lf Z=%.4lf\n\r", ax, ay, az);
        pc.printf(payload);
        
        if (role==1)
        {
        // Send led's status to slave
        radio.stopListening();
        //radio.write_payload(payload, sizeof(char));
        
        
        
        while(payload[i] != '\0')
        {
            if(radio.write(&payload[i], sizeof(payload[i]))) pc.printf("%c %s\n\r",payload[i],"Succeed");
            else pc.printf("%c %s\n\r",payload[i],"Failed");
            i++;
        }
        radio.write("\0",1);
        pc.printf("Tx end");
        i = 0;
        
        
        //radio.openWritingPipe(SLAVE_ADDRESS2);
        /*
        if (radio.write(&payload, sizeof(payload))) // send message to slave and get acknowledge
            printf("Message delivery succeeded.\r\n");
        else
            printf("Message delivery failed.\r\n");
            */
        radio.startListening();
        }
        
        /*
        if(1)//meas_r<0.5)
        {
        
        if((temp-all)>TH)
        { 
            buzzer = 1;
            count++;
        }
        
        if(count==0);
        else
        {
            if(count > 32){count = 0; buzzer = 0;}
            else{count++;}
        }
        }
        
        else count = 0;
        ttemp = all;
        //wait(0.05);
        */
        wait(1);
    }
     
}


/*
int RFtest() /////////////////////////////////수정필요, 송수신코드 분리
{
    
    if (radionumber)
    {
        radio.openWritingPipe(ADDRESS);
        radio.openReadingPipe(0, ADDRESS);  // use pipe 0 of this slave to receive messsages and send back auto acknowledge
        }
    else
    {
        radio.openWritingPipe(ADDRESS);
        radio.openReadingPipe(0, ADDRESS);  // use pipe 0 of this slave to receive messsages and send back auto acknowledge
        }
    radio.startListening();
    while (1) {
        if (role==1)
        {
        // Send led's status to slave
        radio.stopListening();
        //radio.openWritingPipe(SLAVE_ADDRESS2);
        if (radio.write(&payload, sizeof(payload))) // send message to slave and get acknowledge
            printf("Message delivery succeeded.\r\n");
        else
            printf("Message delivery failed.\r\n");
        radio.startListening();
        }
        //wait(1);
        if(role == 0)
        {
        // read message

         if (radio.available())
          {
            radio.read(&payload, sizeof(payload)); // read message and send acknowledge back to the master
            radio.stopListening();
            radio.write(&payload, sizeof(payload));
            radio.startListening();
            
            printf("recieved: %d\n\r",temp);
          }
        }
        
        if(pc.readable())
        {
            char c = pc.getc();
            printf("%c\n\r",c);
            if(c == '0')//(c == 'R') && (role == 0)
            {
                printf("RX mode\n\r");
                role = 0;
            }
                else if(c == '1')//(c == 'T') && (role == 1)
                {
                    printf("TX mode\n\r");
                    role = 1;
                    radio.startListening();
                }
            }
    }
}
*/
