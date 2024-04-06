#ifndef __HARDWARE_H__
#define __HARDWARE_H__

/* 
	���ţ���ν������BASE����
	����ledpwm������ܡ��̵�������������ne555��Ƶ�ʡ�
	�Լ�һЩ��������ʹ�õ��������ʱ��
 */

#include <STC15F2K60S2.H>

#include "main.h"

#define CHANNLE(n, x) { P0 = x; P2 |= n << 5; P2 &= 0x1F; }

#define SCREEN_NUM 7 //���������

extern uint8_t xdata *digs; //��ǰ��ʾ����
extern uint8_t xdata dig_buf[][8];
extern uint8_t xdata dig_switch; //�����ÿһλ�Ŀ���

extern uint8_t xdata led_buf; //ÿ��led��״̬
extern uint8_t xdata led_pwm_duty; //led��pwmռ�ձȣ�Ĭ����ռ�ձ�

/* ��������״̬
   bit 0 - 3Ϊ��������������
   bit 4Ϊ�̵������
   bit 5Ϊ����������
   bit 6Ϊ��������� */
extern uint8_t xdata high_power_state;

/* ���ⶨʱ�� */
extern uint8_t xdata Timer10ms_cnt;
extern uint8_t xdata Timer50ms_cnt;
extern uint8_t xdata Timer100ms_cnt;
extern uint16_t xdata Timer500ms_cnt;
extern uint16_t xdata Timer1s_cnt;
extern uint16_t xdata Timern_cnt;
extern uint16_t xdata operate_timer_cnt;
extern unsigned char key_value ; //������ֵ
extern unsigned char key_state ; //����״̬

extern uint16_t xdata ne555_out;

void NE555Init(void);
void Timer1Init(void);
void KeyAction();
void key_pad_scan_v1();
void key_pad_scan_v2();
void key_pad_scan_v3();

#endif
