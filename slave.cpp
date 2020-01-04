#include "mbed.h"
#include "RF24.h"

const uint64_t  ADDRESS = 0xE7E7E7E700;//0xF0F0F0F0F0F0F001L;

DigitalOut      led(LED1);
RF24            radio(A6,A5,A4,A3,D3);    // mosi, miso, sck, csn, ce, (irq not used)
char         payload;
char            arr[32];
uint8_t         i = 0;
int main(void)
{
   printf("RESET Device\n\r");
   // Initialize nRF24L01
    if (!radio.begin()) {
        printf("Failed to initialize nRF24L01. Check whether connected.\r\n");
        return -1;  // Exit the program
    }
    radio.setPALevel(RF24_PA_LOW);
    radio.setRetries(5, 15);
    radio.setPayloadSize((32)); // 주고 받는게 우선 사이즈가 같아야한다. 
    radio.setAutoAck(true);
    radio.setChannel(62);
    radio.openReadingPipe(0, ADDRESS);  // use pipe 0 of this slave to receive messsages and send back auto acknowledge
    radio.startListening();

    while (1) {
        if (radio.available())
        {
            radio.read(&payload, 1); // read message and send acknowledge back to the master
            //printf("%c\n\r",payload);
            if(payload != '\0')
            {
                arr[i] = payload;
                i++;
            }
            else
            {
                printf("arr: %s\n\r",arr); 
                i = 0;
            }
        }
        

        
    }
}

