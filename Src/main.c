/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  *
  *
  ******************************************************************************
  */
#define STM32F103xB

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "main.h"
#include "stdlib.h"
#include <stdio.h>
#include "enc.h"
#include "u8g_arm.h"
#include "fonts.h"
#include "ssd1306.h"
#include "u8g_arm.h"	// медленная но крутая графическая библиотека
// хендлер для нее
u8g_t u8g;
/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
/* Private variables ---------------------------------------------------------*/
extern USBD_HandleTypeDef hUsbDeviceFS;
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);  

void MX_GPIO_Init(void);
void MX_I2C1_Init(void);
void MX_ADC1_Init(void);
void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
extern uint8_t USBD_CUSTOM_HID_SendReport     (USBD_HandleTypeDef  *pdev, uint8_t *report, uint16_t len);
/* Private function prototypes -----------------------------------------------*/
unsigned int ADC_T(void);
unsigned int ADC_V(void);
void RGB_POWER(unsigned int R,unsigned int G,unsigned int B);
unsigned char dev_connected;

// последовательность нот для мелодии, заданная таблично (две мелодии)
uint16_t sound[20];         											// приве
// наши переменные
uint16_t S_ON=20; //счетчик нот, ноты играются пока он не досчитает до 32
uint8_t Delay=5; 	//*0.1 sec - длительность для ноты (0.5 сек)
uint8_t del;    	// счетчик задержки (сколько раз по 0,1 сек. прошло)
	/*----------------------- макросы запуска мелодий --------------------------*/
#define PLAY_SONG {Delay=5; S_ON=0;} 		// мелодия №1, длительность ноты 0,3 сек,
	//  запуск обнулением S_ON
uint8_t Screen;
uint32_t current_time, old_time[3], time_between;
struct Screen2and3
{
	signed char Vx, x, y, x_old, y_old;
} Note;
int count = 0;
int r=0;
int oled_set_pix (uint32_t xx, uint32_t yy, uint8_t col)
{
   ssd1306_DrawPixel(xx, yy, col);
return 0;
}

void Note_Dance(signed char x, const uint8_t note[7*10])
{ 
	Note.y=20;
	for(int i = 0; i<22; ++i)
  {
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7))
		{
			count = 0;
			break;
			HAL_Delay(500);
		}
	 for (int j = 0; j < 7*10; ++j) 
      {
				oled_set_pix(x + (j % 7) , Note.y + (j / 7), 0);
      }
	 Note.y++;
			for (int j = 0; j < 7*10; ++j) 
      {
				oled_set_pix(x + (j % 7) , Note.y + (j / 7), note[j]);
      }
	 ssd1306_UpdateScreen();
			HAL_Delay(20);
  }
	for(int i = 0; i<22; ++i)
  {
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7))
		{
	    count = 0;
			break;
			HAL_Delay(500);
		}
	 for (int j = 0; j < 7*10; ++j) 
      {
				oled_set_pix(x + (j % 7) , Note.y + (j / 7), 0);
      }
		Note.y--;
		for (int j = 0; j < 7*10; ++j) 
      {
				oled_set_pix(x + (j % 7) , Note.y + (j / 7), note[j]);
      }
	 
	ssd1306_UpdateScreen();
	HAL_Delay(10);
  }
	
}
const uint8_t Arrow[7*10] = {
 0,0,0,1,0,0,0,
 0,0,0,1,0,0,0,
 0,0,0,1,0,0,0,
 0,0,0,1,0,0,0,
 0,0,0,1,0,0,0,
 0,0,0,1,0,0,0,
 0,0,0,1,0,0,0,
 0,1,1,1,1,1,0,
 0,0,1,1,1,0,0,
 0,0,0,1,0,0,0};

 const uint8_t note1[7*10] = {
 0,0,0,1,0,0,0,
 0,0,0,1,1,0,0,
 0,0,0,1,0,1,0,
 0,0,0,1,0,0,0,
 0,0,0,1,0,0,0,
 0,0,0,1,0,0,0,
 0,0,0,1,0,0,0,
 0,0,1,1,0,0,0,
 0,1,1,1,0,0,0,
 0,1,1,0,0,0,0};
 
 const uint8_t note2[7*10] = {
 0,0,0,0,0,0,1,
 0,0,0,0,0,1,1,
 0,0,0,0,1,0,1,
 0,0,0,1,0,0,1,
 0,0,1,0,0,1,1,
 0,0,1,0,1,1,0,
 0,0,1,0,0,0,0,
 0,0,1,0,0,0,0,
 0,1,1,0,0,0,0,
 1,1,0,0,0,0,0};
 
 const uint8_t note3[7*10] = {
 0,0,0,0,0,0,1,
 0,0,0,0,0,1,0,
 0,0,0,0,0,1,0,
 0,0,0,0,0,1,0,
 0,0,0,0,0,1,0,
 0,0,0,0,0,1,0,
 0,0,0,0,1,1,0,
 0,0,0,1,1,1,0,
 0,0,0,1,1,0,0,
 0,0,0,0,0,0,0};

const uint8_t Key_Do[15*20] = {
  1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,
	1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,0,0,1,1,1,1,
	1,1,1,0,1,1,1,0,0,1,1,0,1,1,1,
	1,1,1,0,1,1,1,0,0,1,1,0,1,1,1,
	1,1,1,0,1,1,0,1,0,1,1,0,1,1,1,
	1,1,1,0,0,0,0,1,1,0,0,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  };

const uint8_t Key_Re[15*20] = {
  0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,
	1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,0,1,1,0,0,1,1,1,1,
	1,1,1,0,1,1,1,0,0,1,1,0,1,1,1,
	1,1,1,0,1,1,1,0,0,0,0,0,1,1,1,
	1,1,1,0,1,1,1,0,0,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,0,0,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  };
const uint8_t Key_Mi[15*20] = {
  0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,0,1,0,0,1,1,1,1,1,1,1,
	1,1,1,0,0,1,0,0,1,1,1,1,1,1,1,
	1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  };
	const uint8_t Key_Fa[15*20] = {
  1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,0,0,1,1,1,1,
	1,1,1,0,1,1,1,1,0,1,1,0,1,1,1,
	1,1,1,0,1,1,1,1,0,0,0,0,1,1,1,
	1,1,1,0,1,1,1,1,0,1,1,0,1,1,1,
	1,1,1,0,1,1,1,1,0,1,1,0,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  };
	const uint8_t Key_So[15*20] = {
  0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,0,0,0,1,1,1,1,0,0,0,0,
	1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,0,1,0,0,1,1,1,1,
	1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,
	1,1,1,1,1,1,1,0,0,1,1,0,1,1,1,
	1,1,1,0,1,1,1,0,0,1,1,0,1,1,1,
	1,1,1,1,0,0,0,1,1,0,0,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  };
	const uint8_t Key_La[15*20] = {
  0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,0,0,1,1,1,1,
	1,1,1,0,1,1,1,1,0,1,1,0,1,1,1,
	1,1,1,0,1,1,1,1,0,0,0,0,1,1,1,
	1,1,1,0,1,1,1,1,0,1,1,0,1,1,1,
	1,1,1,0,0,0,0,1,0,1,1,0,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  };
	const uint8_t Key_Si[15*20] = {
  0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,1,0,0,0,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,0,0,0,1,1,1,0,1,1,1,1,
	1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,
	1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,
	1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,
	1,1,1,0,1,1,1,0,1,1,0,1,1,1,1,
	1,1,1,1,0,0,0,1,1,1,0,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  };

 
void draw(void)  
{  

}  
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
	MX_TIM1_Init();
	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	MX_USB_DEVICE_Init();    
  u8g_InitComFn(&u8g, &u8g_dev_ssd1306_128x64_i2c, u8g_com_hw_i2c_fn); 
	
	Note.x_old = 64;
	Screen=1;
	Note.x=4;
	Note.y=32;
	count = 0;
	r=0;
	
	for (int i = 0; i < 3; ++i) 
    {
   	 old_time[i]=0;
    }
  
ssd1306_Init(hi2c1,0x78);
ssd1306_Fill(Black) ;
ssd1306_UpdateScreen();


  /* Infinite loop */

	while (1)
  {
		
  //this loop correspond of drawing  
   
//    ssd1306_Fill(Black); // рисуем на чистом?
//		ssd1306_SetCursor(0, 15);
//		ssd1306_WriteString(" _SPACE INVADERS_", Font_7x9, White);
//    ssd1306_SetCursor(0, 40);
//		ssd1306_WriteString(" Press the button", Font_7x9, White);
//    ssd1306_SetCursor(0, 52);
//		ssd1306_WriteString("     to start", Font_7x9, White);
//    ssd1306_UpdateScreen();


	
		
			//Первый экран (Меню)
	if(Screen==1)
	{
		ssd1306_Fill(Black); // рисуем на чистом?
		ssd1306_SetCursor(7, 1);
		ssd1306_WriteString("Write Your Music!", Font_7x9, White);
		for (int i = 0; i < 15*20; ++i)
				{
					oled_set_pix(0 + (i % 15), 44 + (i / 15), Key_Do[i]);
					oled_set_pix(16 + (i % 15), 44 + (i / 15), Key_Re[i]);
					oled_set_pix(32 + (i % 15), 44 + (i / 15), Key_Mi[i]);
					oled_set_pix(48+ (i % 15), 44 + (i / 15), Key_Fa[i]);
					oled_set_pix(64+ (i % 15), 44 + (i / 15), Key_So[i]);
					oled_set_pix(80+ (i % 15), 44 + (i / 15), Key_La[i]);
					oled_set_pix(96+ (i % 15), 44 + (i / 15), Key_Si[i]);
					oled_set_pix(112+ (i % 15), 44 + (i / 15), Key_Do[i]);
					
				}	
	  for (int i = 0; i < 7*10; ++i)
				{
				oled_set_pix(Note.x + (i % 7), 30 + (i / 7), Arrow[i]);
				}
		
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5))
		{
			Note.x_old=Note.x;
			Note.x=Note.x + 16;
			HAL_Delay (200);
		}
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4))
		{
			Note.x_old=Note.x;
			Note.x=Note.x - 16;
			HAL_Delay (200);
		}
		if(Note.x>116)
		{
			Note.x=116;
		}
		if(Note.x<4)
		{
			Note.x=4;
		}
		for (int i = 0; i < 7*10; ++i)
				{
				oled_set_pix(Note.x_old  + (i % 7), 30 + (i / 7), 0);
				oled_set_pix(Note.x + (i % 7), 30 + (i / 7), Arrow[i]);
				}
				
	 
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6))
		{
			
			switch(Note.x)
			{
				case 4:
					sound[count]=1605;
				 break;
				case 20:
					sound[count]=1430;
				 break;
				case 36:
					sound[count]=1274;
				 break;
				case 52:
					sound[count]=1203;
				 break;
				case 68:
					sound[count]=1072;
				 break;
				case 84:
					sound[count]=954;
				 break;
				case 100:
					sound[count]=850;
				 break;
				case 116:
					sound[count]=802;
				 break;
			}
			count++;
			sound[count]=2;
			r++;
			if(r==3)
			{
				r=0;
			}
			count++;
			Note.Vx=Note.Vx + 12;
			HAL_Delay (200);
		}
		if(r==0)
		{
    for (int i = 0; i < 7*10; ++i)
				{
				oled_set_pix(Note.Vx-12 + (i % 7), 16 + (i / 7), note1[i]);
				}
			} else if (r==1)
		{
			for (int i = 0; i < 7*10; ++i)
				{
				oled_set_pix(Note.Vx-12 + (i % 7), 16 + (i / 7), note2[i]);
				}
			} else if (r==2)
		{
			for (int i = 0; i < 7*10; ++i)
				{
				oled_set_pix(Note.Vx-12 + (i % 7), 16 + (i / 7), note3[i]);
				}
			}
			ssd1306_UpdateScreen();
      if(count == 20)
			{
				HAL_Delay (700);
				Screen = 2;
			}
		}
	
	
		//Второй экран (Заставка Note)
	  if (Screen==2)
	{
		    MX_TIM3_Init();
		    MX_TIM2_Init();
		    HAL_TIM_Base_Start_IT(&htim3);
		  	HAL_TIM_OC_Start(&htim2,TIM_CHANNEL_1);
        MX_ADC1_Init();
		
		  Note.y=20;
		  ssd1306_Fill(Black); // рисуем на чистом?
		  ssd1306_SetCursor(48, 1);
	  	ssd1306_WriteString("Enjoy!", Font_7x9, White);
		 ssd1306_SetCursor(15, 54);
	  	ssd1306_WriteString("2-Back To Piano", Font_7x9, White);
		for (int j = 0; j < 7*10; ++j) //Цикл стирания предыдующего шарика
      {
				oled_set_pix(25 + (j % 7) , Note.y + (j / 7), note1[j]);
				oled_set_pix(39 + (j % 7) , Note.y + (j / 7), note2[j]);
				oled_set_pix(53 + (j % 7) , Note.y + (j / 7), note3[j]);
				oled_set_pix(67 + (j % 7) , Note.y + (j / 7), note1[j]);
				oled_set_pix(81 + (j % 7) , Note.y + (j / 7), note2[j]);
				oled_set_pix(95 + (j % 7) , Note.y + (j / 7), note3[j]);
      }
			PLAY_SONG;
	  	Note_Dance(25,note1);
			if(count==0)
			{
		  HAL_TIM_OC_Stop(&htim2,TIM_CHANNEL_1);
				Note.Vx=0;
			Note.x_old = 64;
			Note.x=4;
	    count = 0;
	    r=-1;
			Screen=1;
			}
      Note_Dance(39,note2);
			if(count==0)
			{
			HAL_TIM_OC_Stop(&htim2,TIM_CHANNEL_1);
				Note.Vx=0;
			Note.x_old = 64;
			Note.x=4;
	    count = 0;
	    r=-1;
			Screen=1;
			}
	    Note_Dance(53,note3);
	    Note_Dance(67,note1);
			if(count==0)
			{
			HAL_TIM_OC_Stop(&htim2,TIM_CHANNEL_1);
				Note.Vx=0;
			Note.x_old = 64;
			Note.x=4;
	    count = 0;
	    r=-1;
			Screen=1;
			}
		  Note_Dance(81,note2);
			if(count==0)
			{
			HAL_TIM_OC_Stop(&htim2,TIM_CHANNEL_1);
				Note.Vx=0;
			Note.x_old = 64;
			Note.x=4;
	    count = 0;
	    r=-1;
			Screen=1;
			}
		  Note_Dance(95,note3);
			if(count==0)
			{
			HAL_TIM_OC_Stop(&htim2,TIM_CHANNEL_1);
				Note.Vx=0;
			Note.x_old = 64;
			Note.x=4;
	    count = 0;
	    r=-1;
			Screen=1;
			}
			ssd1306_UpdateScreen();
	}
		
	 
		
	}
  /* USER CODE END 3 */

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		if (!(S_ON==20))
		{
			if (S_ON==0){HAL_TIM_Base_Start(&htim2);}
			if(S_ON%2==0)
			{
				Delay=2;
			}
			if (del<Delay)
			{
				del++;
			}
			else 
			{
        del=0;
        if (!(htim2.Init.Prescaler == sound[S_ON]))
        {
					TIM2->PSC=sound[S_ON];
				}
        S_ON++; 
			}
		}
		else
		{
			TIM2->PSC=2;
			HAL_TIM_Base_Stop(&htim2);
		} 
	}
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
    |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void MX_I2C1_Init(void)
{

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 1200000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}
/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	//7-digit/seg`s
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15 ;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	//7-digit/dig`s
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}
/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM1_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 64;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1000;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_TIM_MspPostInit(&htim1);

}
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 7200;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}
/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 50;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 25;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
	HAL_TIM_OC_Init(&htim2);
  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

 void MX_ADC1_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
  /** Common config
  */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }
/** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

unsigned int ADC_T (void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    unsigned int ADC;

    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start (&hadc1);
    HAL_ADC_PollForConversion (&hadc1, 100);
    ADC=HAL_ADC_GetValue (&hadc1);  /* Infinite loop */
    HAL_ADC_Stop(&hadc1);

    return ADC;
}

unsigned int ADC_V (void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    unsigned int ADC;

    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    HAL_ADC_Start (&hadc1);
    HAL_ADC_PollForConversion (&hadc1, 100);
    ADC=HAL_ADC_GetValue (&hadc1);  /* Infinite loop */
    HAL_ADC_Stop(&hadc1);

    return ADC;
}

void RGB_POWER(unsigned int R,unsigned int G,unsigned int B)
{
    TIM1->CCR1=B;
    TIM1->CCR2=R;
    TIM1->CCR3=G;
}
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}

#endif

/**
  * @}
  */

/**
  * @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
