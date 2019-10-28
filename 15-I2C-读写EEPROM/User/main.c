/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"
#include "./i2c/bsp_i2c_ee.h"

uint8_t cal_flag = 0;
uint8_t k;

/*存储小数和整数的数组，各7个*/
long double double_buffer[7] = {0};
int int_bufffer[7] = {0};

#define DOUBLE_ADDR       10
#define LONGINT_ADDR      70

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  HAL_Init();        
  /* 配置系统时钟为72 MHz */ 
  SystemClock_Config();

  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  DEBUG_USART_Config();
  
  printf("\r\n 这是一个EEPROM 读写小数和长整数实验 \r\n");

  /* I2C 外设初(AT24C02)始化 */
  i2c_CfgGpio();


  /*读取数据标志位*/
  ee_ReadBytes(&cal_flag, 0, 1);
  
    if( cal_flag != 0xCD )	/*若标志等于0xcd，表示之前已有写入数据*/
    {      
        printf("\r\n没有检测到数据标志，FLASH没有存储数据，即将进行小数写入实验\r\n");
        cal_flag =0xCD;
        
        /*写入标志到0地址*/
        ee_WriteBytes(&cal_flag, 0, 1); 
        
        /*生成要写入的数据*/
        for( k=0; k<7; k++ )
        {
           double_buffer[k] = k +0.1;
           int_bufffer[k]=k*500+1 ;
        }

        /*写入小数数据到地址10*/
        ee_WriteBytes((void*)double_buffer,DOUBLE_ADDR, sizeof(double_buffer));
        /*写入整数数据到地址60*/
        ee_WriteBytes((void*)int_bufffer, LONGINT_ADDR, sizeof(int_bufffer));
              
        printf("向芯片写入数据：");
        /*打印到串口*/
        for( k=0; k<7; k++ )
        {
          printf("小数tx = %LF\r\n",double_buffer[k]);
          printf("整数tx = %d\r\n",int_bufffer[k]);
        }
        
        printf("\r\n请复位开发板，以读取数据进行检验\r\n");      
		
    }    
    else
    {      
      	 printf("\r\n检测到数据标志\r\n");

				/*读取小数数据*/
        ee_ReadBytes((void*)double_buffer, DOUBLE_ADDR, sizeof(double_buffer));
				
				/*读取整数数据*/
        ee_ReadBytes((void*)int_bufffer, LONGINT_ADDR, sizeof(int_bufffer));
	
			
				printf("\r\n从芯片读到数据：\r\n");			
        for( k=0; k<7; k++ )
				{
					printf("小数 rx = %LF \r\n",double_buffer[k]);
					printf("整数 rx = %d \r\n",int_bufffer[k]);				
				}
      
    }   

  
  while (1)
  {      
  }
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 1
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
