#include "tm4c.h"

#define RED_LED   (1u << 1)
#define BLUE_LED  (1u << 2)
#define GREEN_LED (1u << 3)


int main() {
  
    SYSCTL_RCGCGPIO_R |= (1u << 5); //enable clock for GPIOF
    SYSCTL_GPIOHBCTL_R |= (1 << 5); //enable high peformance bus(AHB) for GPIOF
    GPIO_PORTF_AHB_DIR_R |= (RED_LED | BLUE_LED | GREEN_LED); //SET PINS 1,2 and 3 as outputs
    GPIO_PORTF_AHB_DEN_R |= (RED_LED | BLUE_LED | GREEN_LED);  //enable bits 1, and 3 as digital outputs
    
    GPIO_PORTF_AHB_DATA_BITS_R[BLUE_LED] = BLUE_LED;; //turn the blue LED on
    while (1){
        GPIO_PORTF_AHB_DATA_BITS_R[RED_LED] = RED_LED; //turn on the red LED
        int volatile counter = 0;
        while (counter < 1000000){ //delay loop
          ++counter;
        }
        
        GPIO_PORTF_AHB_DATA_BITS_R[RED_LED] = 0; //turn LED off
        counter = 0;
        while (counter < 1000000){
          ++counter;
        }
    }
    return 0;
}
