#include "tm4c.h" //header files for all registers of the Tiva C Launchpad

#define RED_LED   (1u << 1)
#define BLUE_LED  (1u << 2)      //defining macros
#define GREEN_LED (1u << 3)


int main() {
  
    SYSCTL_RCGCGPIO_R |= (1u << 5); //enable clock for GPIOF
    GPIO_PORTF_DIR_R |= (RED_LED | BLUE_LED | GREEN_LED); //SET PINS 1,2 and 3 as outputs
    GPIO_PORTF_DEN_R |= (RED_LED | BLUE_LED | GREEN_LED);  //enable bits 1, and 3 as digital outputs
    
    GPIO_PORTF_DATA_R |= BLUE_LED; //turn the blue LED on
    while (1){
        GPIO_PORTF_DATA_R |= RED_LED; //turn red LED on and keep on
        int volatile counter = 0;
        while (counter < 1000000){ //delay loop
          ++counter;
        }
        
        
        GPIO_PORTF_DATA_R &= ~RED_LED; //turn LED off
        counter = 0;
        while (counter < 1000000){
          ++counter;
        }
    }
    return 0;
}
