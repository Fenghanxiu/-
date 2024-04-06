#ifndef __ECHO_H__
#define __ECHO_H__

#include <STC15F2K60S2.H>
#include "main.h"
#include "intrins.h"

//#define echo_timer
#ifdef echo_timer

int read_distance();


#endif

#define echo_pca_1
#ifdef echo_pca_1
void Send_Wave();
unsigned int Get_Csb();

#endif




//#define echo_pca_2
#ifdef echo_pca_2

#define DISTANCE_OUTRANG (0xFFFF) //定义为超出量程时的距离数值

void TimerPCAInit();
void sand_ultrasonic();
void calculate_distance();
void read_distance();

extern uint16_t distance;
extern uint8_t ultrasonic_flag;
#endif



#endif
