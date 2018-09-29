#include "lm4f120h5qr.h"
#include "delay.h"

#define LED_RED (1U << 1)
#define LED_BLUE (1U << 2)
#define LED_GREEN (1U << 3)

int *swap(int *x, int *y);

int *swap(int *x, int *y){
  static int tmp[2];
  tmp[0] = *x;
  tmp[1]= *y;
  *x=tmp[1];
  *y=tmp[0];
  return tmp;
}

int main(){
    SYSCTL_RCGCGPIO_R |= (1U << 5);      /* enable clock for GPIOF */
    SYSCTL_GPIOHBCTL_R |= (1U << 5);             // enable AHB for GPIOF
    GPIO_PORTF_AHB_DIR_R  |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIO_PORTF_AHB_DEN_R  |= (LED_RED | LED_BLUE | LED_GREEN);
 
    GPIO_PORTF_AHB_DATA_BITS_R[LED_BLUE] = LED_BLUE; //turn the blue LED on
    while(1){
      int x = 1000000;
      int y = 1000000/2;
      int *p = swap(&x, &y);
          GPIO_PORTF_AHB_DATA_BITS_R[LED_RED] = LED_RED;
   
    delay(p[0]);
      GPIO_PORTF_AHB_DATA_BITS_R[LED_RED] = 0;
    delay(p[1]);
   }
  //return 0;
}

