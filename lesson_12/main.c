#include <stdint.h>

#include "lm4f120h5qr.h"
#include "delay.h"

#define LED_RED (1U << 1)
#define LED_BLUE (1U << 2)
#define LED_GREEN (1U << 3)

typedef struct{
  uint16_t x;
    uint8_t y;
}  Point;

Point p1, p2;

typedef struct {
    Point top_left;
    Point bottom_left;
} Window;

typedef struct{
  Point corners[3];
}Triangle;
  
 Window w, w2;
 Triangle t;

int main(){
  Point *pp;
  Window *wp;
  
    p1.x = sizeof(Point);
    p1.y = p1.x - 3U;
  
    w.top_left.x = 1U;
    w.bottom_left.y = 2U;
    
    t.corners[0].x = 1U;
    t.corners[2].y = 2U;
    
    p2 = p1;
    w2 = w;
    
    pp=&p1;
    wp = &w2;
    
    (*pp).x = 2U;
    (*wp).top_left = *pp;
    
    pp->x = 1U;
    wp->top_left = *pp;
      
    SYSCTL_RCGCGPIO_R |= (1U << 5);      /* enable clock for GPIOF */
    SYSCTL_GPIOHBCTL_R |= (1U << 5);             // enable AHB for GPIOF
    GPIO_PORTF_AHB_DIR_R  |= (LED_RED | LED_BLUE | LED_GREEN);
    GPIO_PORTF_AHB_DEN_R  |= (LED_RED | LED_BLUE | LED_GREEN);
 
    GPIO_PORTF_AHB_DATA_BITS_R[LED_BLUE] = LED_BLUE; //turn the blue LED on
    while(1){
          GPIO_PORTF_AHB_DATA_BITS_R[LED_RED] = LED_RED;
   
    delay(500000);
      GPIO_PORTF_AHB_DATA_BITS_R[LED_RED] = 0;
    delay(500000);
   }
  //return 0;
}

