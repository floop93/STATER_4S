/*
 * content.c
 *
 *  Created on: 26.03.2021
 *      Author: FILIP
 */



/* includes ----------------------------------*/
#include "content.h"

/* definitions -------------------------------*/

#define sensor_number 4					//how many sensors are in system
#define average_matrix_size 40 			//size of matrix used to average measurement
#define calculation_factor 350			//used to convert counter value to mm
#define TIM1_prsc 50					//prescaler of timer1
#define sensor_defective_treshold 10	//maximum of defective readings in row before deactivating controll

#define AngleCorectionUpper_hist 50					//Angle Correction upper hysteresis in mm
#define AngleCorectionLower_hist_min 100			//Angle Correction lower hysteresis minimum in mm
#define AngleSensRangeFactor 8						//this factor changes range of sensitivity POT influence (higher factor -> lower influence)

#define HeightCorrection_minimum 300				//minimal height that can be set in mm
#define HeightCorrection_hist_min 50				//minimum hysteresis of height correction
#define HeightSensRangeFactor 8						//this factor changes range of sensitivity POT influence (higher factor -> lower influence)
#define HeightRangeFactor 2							//this factor changes heght range that can be set on POT (higher factor -> lower range)

//location of sensor on beam
#define sensor_left_edge 0
#define sensor_left_center 1
#define sensor_right_edge 2
#define sensor_right_center 3

#define hydr_on_time_L 100			//hydraulic on impuls time
#define hydr_off_time_L 4000		//hydraulic stop time between impulses
#define hydr_on_time_H 100			//hydraulic on impuls time
#define hydr_off_time_H 1000		//hydraulic stop time between impulses

#define calc_factor 7//calculation_factor/TIM1_prsc
#define TIM1_over_boundry 2560000/TIM1_prsc
#define TIM1_20us 1280/TIM1_prsc
#define TIM1_100us 6400/TIM1_prsc
#define TIM1_no_obsticle 1600000/TIM1_prsc
#define TIM1_max_diff max_diff/TIM1_prsc




/* type definitions ---------------------------*/

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_adc1;

/* variables ---------------------------------*/
uint8_t sensor_nbr=0;
uint16_t blink=0;
uint32_t ADC_raw[1];
uint16_t POT_sens=0;
uint16_t POT_height=0;
uint32_t R_height=0;
uint32_t L_height=0;
uint32_t L_actual=0, R_actual=0;
uint32_t AClower_hist=0;
uint32_t ACupper_hist=0;
uint32_t beam_height=0;
uint32_t beam_height_set=0;
uint32_t beam_hist=0;

uint16_t t1=0, t2=0, t3=0;
uint16_t SW1_timeout=0, SW2_timeout=0, SW3_timeout=0;
uint8_t SW1_hold=0, SW2_hold=0, SW3_hold=0;
uint16_t hydr_timeout1=0;
uint16_t hydr_timeout2=0;

uint8_t hydr_Rflag=0;
uint8_t hydr_Lflag=0;
uint8_t hydr_UPflag=0;
uint8_t hydr_DWflag=0;

uint32_t time1[average_matrix_size];
uint32_t time2[average_matrix_size];
uint32_t time3[average_matrix_size];
uint32_t time4[average_matrix_size];

uint32_t time1_sum=0;
uint32_t time2_sum=0;
uint32_t time3_sum=0;
uint32_t time4_sum=0;

uint32_t distance[4];

uint16_t time_save_cnt[4];

uint8_t sensor_cnt=1;
uint8_t sensor_read_ok[4];
uint8_t sensor_fault=0;
uint8_t sensor_fault_cnt[4];
uint16_t sensor_timeout=0;
volatile uint8_t measurment_ongoing=0;

volatile uint8_t mode=0;

uint8_t two_sensor_mode=0;

uint8_t sequence=0;

//uint32_t test1=0, test2=0;


/* function prototypes ----------------------*/
void main_content(void);
void loop_content(void);
void ms_period(void);

void ADC_handler(void);
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin);
void measurement(void);
void error_handler(void);
void controll(void);

/* functions ---------------------------------*/
void main_content(void)
{
	//ADC config
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_Delay(100);
	HAL_ADC_Start_DMA(&hadc1, ADC_raw, 2);

	//timer start
	HAL_TIM_Base_Start(&htim1);

	sensor_nbr=sensor_number;
	ACupper_hist=AngleCorectionUpper_hist;

	LED1_on;



}


void loop_content(void)
{
	ADC_handler();
	measurement();
	error_handler();
	controll();

}

void ms_period(void)
{
	if(blink<1000) blink++;


	if(t1<120) t1++;
	if(t2<120) t2++;
	if(t3<1100) t3++;
	if(SW1_timeout<1100) SW1_timeout++;
	//if(SW2_timeout<1100) SW2_timeout++;
	//if(SW3_timeout<1100) SW3_timeout++;

	if(sensor_timeout<1000) sensor_timeout++;
	if(hydr_timeout1<5000) hydr_timeout1++;
	if(hydr_timeout2<5000) hydr_timeout2++;

}


void ADC_handler(void)
{
	uint32_t calc=0;

	if(t1>100)
	{
		t1=0;

		calc=ADC_raw[0]&0xFFFF;
		POT_height=4095-calc;



		calc=ADC_raw[0]>>16;
		POT_sens=4095-calc;



	}



}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{

	if(measurment_ongoing==1)
	{
		TIM1->CNT=0;
		measurment_ongoing=2;
	}





}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	uint32_t calc=0;
	if(measurment_ongoing==2)
	{
		measurment_ongoing=1;
		TIM1->CNT=0;
	}

	if(measurment_ongoing==3)
	{
		calc=TIM1->CNT;

		if(calc>TIM1_over_boundry) //incorrect value
		{
			sensor_fault_cnt[sensor_cnt-1]++;

			//sensor_fault&=1<<(sensor_cnt-1);

			sensor_read_ok[sensor_cnt-1]=0;
		}
		else if(calc>TIM1_no_obsticle)	//result doesnt count, no obsticle detected
		{

		}
		else
		{
			switch(sensor_cnt)
			{
			case 1:
				time_save_cnt[0]++;

				if(time_save_cnt[0]>=average_matrix_size)
				{
					time_save_cnt[0]=0;
				}


				time1_sum-=time1[time_save_cnt[0]];
				time1_sum+=calc;
				time1[time_save_cnt[0]]=calc;

				distance[0]=(time1_sum/average_matrix_size)/calc_factor;

				break;
			case 2:

				time_save_cnt[1]++;

				if(time_save_cnt[1]>=average_matrix_size)
				{
					time_save_cnt[1]=0;
				}


				time2_sum-=time2[time_save_cnt[1]];
				time2_sum+=calc;
				time2[time_save_cnt[1]]=calc;

				distance[1]=(time2_sum/average_matrix_size)/calc_factor;

				break;
			case 3:

				time_save_cnt[2]++;

				if(time_save_cnt[2]>=average_matrix_size)
				{
					time_save_cnt[2]=0;
				}


				time3_sum-=time3[time_save_cnt[2]];
				time3_sum+=calc;
				time3[time_save_cnt[2]]=calc;

				distance[2]=(time3_sum/average_matrix_size)/calc_factor;

				break;
			case 4:

				time_save_cnt[3]++;

				if(time_save_cnt[3]>=average_matrix_size)
				{
					time_save_cnt[3]=0;
				}

				time4_sum-=time4[time_save_cnt[3]];
				time4_sum+=calc;
				time4[time_save_cnt[3]]=calc;

				distance[3]=(time4_sum/average_matrix_size)/calc_factor;

				break;
			default:
				break;
			}

			sensor_read_ok[sensor_cnt-1]=1;
		}

		measurment_ongoing=0;	//end of measurement cycle

		//next sensor selection

		if(sensor_cnt>=sensor_nbr)
		{
			sensor_cnt=1;
		}
		else
		{
			sensor_cnt++;
		}

	}


}

void measurement(void)
{
	static uint8_t trig_flag=0;

	//start measurement cycle

	if(measurment_ongoing==0)
	{
		if(trig_flag==0)	//start impuls
		{


			TIM1->CNT=0;
			trig_flag=1;
			switch(sensor_cnt)
			{
			case 1:
				TRIG1_on;
				break;
			case 2:
				TRIG2_on;
				break;
			case 3:
				TRIG3_on;
				break;
			case 4:
				TRIG4_on;
				break;
			default:
				break;
			}
		}

		if(trig_flag==1)
		{
			if(TIM1->CNT>TIM1_20us) //20us
			{
				switch(sensor_cnt)
				{
				case 1:
					TRIG1_off;
					break;
				case 2:
					TRIG2_off;
					break;
				case 3:
					TRIG3_off;
					break;
				case 4:
					TRIG4_off;
					break;
				default:
					break;
				}
				measurment_ongoing=1;
				trig_flag=0;

				sensor_timeout=0;
			}
		}



	}


	//if there is no response from sensor, reset flag and add sensor fault

	if(measurment_ongoing==1)
	{
		if(sensor_timeout>200)
		{
			sensor_fault_cnt[sensor_cnt-1]++;
			sensor_read_ok[sensor_cnt-1]=0;

			//next sensor selection

			if(sensor_cnt>=sensor_nbr)
			{
				sensor_cnt=1;
			}
			else
			{
				sensor_cnt++;
			}

			measurment_ongoing=0;
		}
	}

	// check if there is measurment
	if(measurment_ongoing==2)
	{
		if(TIM1->CNT>TIM1_100us)	//100us
		{
			measurment_ongoing=3;
		}
	}



}

void error_handler(void)
{
	if(t2>10)
	{
		t2=0;

		for(uint8_t i=0; i<sensor_nbr; i++)
		{
			if(sensor_read_ok[i]==0)
			{
				if(sensor_fault_cnt[i]>sensor_defective_treshold)
				{
					sensor_fault|=1<<i;
				}
			}
			else
			{
				sensor_fault_cnt[i]=0;
			}

		}


	}


}

void controll(void)
{
	uint32_t calc;


	if(t3>5)
	{
		t3=0;

		//make calculation
		if(sensor_nbr>3)	//4 sensors
		{
			calc=distance[sensor_left_edge]+distance[sensor_left_center];
			calc/=2;
			L_height=calc;

			calc=distance[sensor_right_edge]+distance[sensor_right_center];
			calc/=2;
			R_height=calc;

			AClower_hist=(POT_sens/AngleSensRangeFactor)+AngleCorectionLower_hist_min;

			calc=0;

			for(uint8_t i=0; i<4; i++)
			{
				calc+=distance[i];
			}

			beam_height=calc/4;

			beam_hist=(HeightCorrection_hist_min+(POT_sens/HeightSensRangeFactor))/2;

			beam_height_set=(POT_height/HeightRangeFactor)+HeightCorrection_minimum;

		}
		else				//2 sensors
		{
			switch(two_sensor_mode)	//to do in future... maybe
			{
			case 0:	//LE RE

				break;
			case 1:	//LE RC

				break;
			case 2:	//LC RE

				break;
			case 3: //LC RC

				break;
			}
		}

		if(sensor_fault==0)	//normal operation
		{
			if(SW1_state)	//STOP operation
			{
				mode=0;
				LED1_on;
				LED2_off;
				LED3_off;

			}
			if(SW2_state)	//angle correction on
			{
				mode=1;
				LED1_off;
				LED2_on;
				LED3_off;

			}
			if(SW3_state)	//angle and height correction on
			{
				mode=2;
				LED1_off;
				LED2_off;
				LED3_on;

			}

			if(mode==0)	//STOP operation
			{
				OUT_DW_off;
				OUT_UP_off;
				OUT_L_off;
				OUT_R_off;
			}
			else if(mode<3) //angle correction
			{

				if(L_height>R_height)	//right side lower then left
				{
					OUT_L_off;
					calc=L_height-R_height;
					hydr_Lflag=0;

					if(calc<ACupper_hist)	//turn off coil for picking up right side
					{
						OUT_R_off;
						hydr_Rflag=0;
					}

					if(calc>AClower_hist)	//turn on coil
					{
						if(hydr_Rflag==0)
						{
							OUT_R_on;
							hydr_timeout1=0;
							hydr_Rflag=1;
						}
					}

					if(hydr_Rflag==1)
					{
						if(hydr_timeout1>hydr_on_time_L)
						{
							OUT_R_off;
							hydr_timeout1=0;
							hydr_Rflag=2;
						}
					}
					else if(hydr_Rflag==2)
					{
						if(hydr_timeout1>hydr_off_time_L)
						{
							OUT_R_on;
							hydr_timeout1=0;
							hydr_Rflag=1;
						}
					}


				}
				else	//left side lower than right
				{
					OUT_R_off;
					calc=R_height-L_height;
					hydr_Rflag=0;

					if(calc<ACupper_hist)	//turn off coil for picking up left side
					{
						OUT_L_off;
						hydr_Lflag=0;
					}

					if(calc>AClower_hist)	//turn on coil
					{
						if(hydr_Lflag==0)
						{
							OUT_L_on;
							hydr_timeout1=0;
							hydr_Lflag=1;
						}

					}

					if(hydr_Lflag==1)
					{
						if(hydr_timeout1>hydr_on_time_L)
						{
							OUT_L_off;
							hydr_timeout1=0;
							hydr_Lflag=2;
						}
					}
					else if(hydr_Lflag==2)
					{
						if(hydr_timeout1>hydr_off_time_L)
						{
							OUT_L_on;
							hydr_timeout1=0;
							hydr_Lflag=1;
						}
					}

				}

				if(mode==2)	//angle and height correction
				{
					if(beam_height>beam_height_set)	//check if need to be lowered
					{
						hydr_UPflag=0;
						OUT_UP_off;
						if((beam_height-beam_height_set)>beam_hist)
						{
							if(hydr_DWflag==0)
							{
								OUT_DW_on;
								hydr_timeout2=0;
								hydr_DWflag=1;
							}
						}
						if(hydr_DWflag==1)
						{
							if(hydr_timeout2>hydr_on_time_H)
							{
								OUT_DW_off;
								hydr_timeout2=0;
								hydr_DWflag=2;
							}
						}
						else if(hydr_DWflag==2)
						{
							if(hydr_timeout2>hydr_off_time_H)
							{
								OUT_DW_on;
								hydr_timeout2=0;
								hydr_DWflag=1;
							}
						}

					}
					else //check if need to be rised
					{
						hydr_DWflag=0;
						OUT_DW_off;
						if((beam_height_set-beam_height)>beam_hist)
						{
							if(hydr_UPflag==0)
							{
								OUT_UP_on;
								hydr_timeout2=0;
								hydr_UPflag=1;
							}
						}
						if(hydr_UPflag==1)
						{
							if(hydr_timeout2>hydr_on_time_H)
							{
								OUT_UP_off;
								hydr_timeout2=0;
								hydr_UPflag=2;
							}
						}
						else if(hydr_UPflag==2)
						{
							if(hydr_timeout2>hydr_off_time_H)
							{
								OUT_UP_on;
								hydr_timeout2=0;
								hydr_UPflag=1;
							}
						}
					}

				}


			}
			else
			{
				mode=0;
				LED1_on;
				LED2_off;
				LED3_off;
				LED4_off;

			}

		}
		else //fault - stop operation, allow to reset or reorganize sensors
		{
			OUT_DW_off;
			OUT_UP_off;
			OUT_L_off;
			OUT_R_off;

			LED1_off;
			LED2_off;
			LED3_off;
			//LED4_on;

			if(SW1_state)
			{
				if(SW1_hold==0)
				{
					SW1_hold=1;
					SW1_timeout=0;
				}
				if(SW1_timeout>500)
				{
					LED4_off;
					sensor_fault=0;
					mode=0;
				}

			}
			else
			{
				SW1_hold=0;
			}

			if(sensor_fault&0x01)
			{
				calc=1;
			}
			else if(sensor_fault&0x02)
			{
				calc=2;
			}
			else if(sensor_fault&0x04)
			{
				calc=3;
			}
			else if(sensor_fault&0x08)
			{
				calc=4;
			}

			switch(sequence)
			{
			case 0:
				if(blink>990)
				{
					blink=0;
					sequence=1;
					LED4_on;
				}
				break;
			case 1:
				if(blink>150)
				{
					blink=0;
					sequence=2;
					LED4_off;
				}
				break;
			case 2:
				if(blink>150)
				{
					blink=0;
					sequence=3;
					LED4_on;
				}
				break;
			case 3:
				if(blink>150)
				{
					blink=0;
					sequence=4;
					LED4_off;
				}
				break;
			case 4:
				if(blink>150)
				{
					blink=0;
					sequence=5;
					LED4_on;
				}
				break;
			case 5:
				if(blink>150)
				{
					blink=0;
					sequence=6;
					LED4_off;
				}
				break;
			case 6:
				if(blink>750)
				{
					blink=0;
					sequence=7;
					LED4_on;
				}
				break;
			case 7:
				if(blink>500)
				{
					blink=0;
					if(calc==1) sequence=0;
					else sequence=8;
					LED4_off;
				}
				break;
			case 8:
				if(blink>500)
				{
					blink=0;
					sequence=9;
					LED4_on;
				}
				break;
			case 9:
				if(blink>500)
				{
					blink=0;
					if(calc==2) sequence=0;
					else sequence=10;
					LED4_off;
				}
				break;
			case 10:
				if(blink>500)
				{
					blink=0;
					sequence=11;
					LED4_on;
				}
				break;
			case 11:
				if(blink>500)
				{
					blink=0;
					if(calc==3) sequence=0;
					else sequence=12;
					LED4_off;
				}
				break;
			case 12:
				if(blink>500)
				{
					blink=0;
					sequence=13;
					LED4_on;
				}
				break;
			case 13:
				if(blink>500)
				{
					blink=0;
					sequence=0;
					LED4_off;
				}
				break;
			}


		}

	}

}

