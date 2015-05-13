/*====================================================================================
* 功能描述：迪文工业串口屏串口通讯接口

* 作    者：jackey
* 日    期：2015年4月30号
=====================================================================================*/

#include <string.h>

#include "stm32f4xx.h"
#include "uart3.h"
#include "serialscreen.h"
#include "freeRTOS.h"
#include "queue.h"

void Uart3_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

#if 1  /*stm32f407VGT*/	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//外设时钟使能 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);//连接复用引脚  
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化串口1的GPIO  
#else   /*stm32f407IGT*/
	 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//外设时钟使能 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);//连接复用引脚  
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化串口1的GPIO  
  
#endif
     
    USART_InitStructure.USART_BaudRate = 115200;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据模式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件溢出控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//双工模式
    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
    USART_Init(USART3, &USART_InitStructure);	
    USART_ClearFlag(USART3, USART_FLAG_TC);//清传送完成标志
  	USART_Cmd(USART3, ENABLE);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //嵌套优先级分组为1
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//嵌套通道为USART3_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//响应优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道中断使能 
    NVIC_Init(&NVIC_InitStructure);
}

void Uart3_Send(const uint8_t *p,uint8_t length)
{
    uint8_t i=0;
    USART_ClearFlag(USART3,USART_FLAG_TC);
    for(i=0;i<length;i++)
    {	
        USART_SendData(USART3, (u8) p[i]);   
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);    
    }
}
void Uart3_Sent(const uint8_t *p,uint8_t length)
{
    uint8_t i=0;
    USART_ClearFlag(USART3,USART_FLAG_TC);
    for(i=0;i<length;i++)
    { 
        USART3->DR = (u8) p[i];       
        while((USART3->SR&0X40)==0);//循环发送,直到发送完毕       
    }
}
  /*******************************************************************************
 * 函数名称:_LCD_Disable_RxInit                                                                     
 * 描    述:关闭LCD接受中断                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void _LCD_Disable_RxInit(void)
{
    USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);
}
  /*******************************************************************************
 * 函数名称:_LCD_Enable_RxInit                                                                     
 * 描    述:打开LCD接受中断                                                                 
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2013年8月28日                                                                    
 *******************************************************************************/ 
void _LCD_Enable_RxInit(void)
{
    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
}


#define  RX3BUF_SIZE 100         /*接收FIFO的最大容量*/  
uint8_t   RX3_BUFF[RX3BUF_SIZE]; /*接收FIFO缓冲区数组*/
uint8_t   UART_InpLen =0;        /*接收FIFO内待读取的字节数*/
uint32_t  RX_IndexR =0;          /*接受FIFO的读指针*/
uint32_t  RX_IndexW =0;          /*接受FIFO的读指针*/
 /*******************************************************************************
 * 函数名称: USART3_GetChar()
 * 描    述: 从串口读取一个字节数据（从缓冲队列读取1字节已接受的数据）
 *
 * 输    入: *chr：读取数据所存放的地址指针
 * 输    出: 返回1表示读取成功，返回0表示读取失败
 * 返    回: char
 * 修改日期: 2014年3月13日   
*******************************************************************************/
uint8_t USART3_GetChar(uint8_t *Chr)
{
    if(UART_InpLen==0)return(0);    //如果FIFO内无数据，返回0
    //_DINT();											//涉及FIFO操作时不允许中断，以免指针错乱
    UART_InpLen--;                  //待读取数据字节数减1
    *Chr =RX3_BUFF[RX_IndexR];			//待读取指针读取一个字节作为返回值
    if(++RX_IndexR>=RX3BUF_SIZE)    //读取指针递增，且判断是否下标越界
    {
        RX_IndexR =0;                 //如果越界则写指针归0(循环队列)
    }
    //_EINT();                      //FIFO操作完毕，恢复中断允许
    return(1); 											//返回成功标志	
}
 /*******************************************************************************
 * 函数名称: USART3_GetCharInRxBuf()
 * 描    述: 获取FIFO内已接受的数据字节数
 *
 * 输    入: 无
 * 输    出: 无
 * 返    回: FIFO内数据的字节数
 * 修改日期: 2014年3月13日 
*******************************************************************************/
uint32_t UART3_GetCharsInRxBuf(void)
{
    return(UART_InpLen);           //返回FIFO内数据的字节数
}

 /*******************************************************************************
 * 函数名称: UART0_ClrRxBuf()
 * 描    述: 清除接收FIFO区
 *
 * 输    入: 无
 * 输    出: 无
 * 返    回: 无
 * 修改日期: 2014年3月13日 
*******************************************************************************/
void UART3_ClrRxBuf(void)
{
	//_DINT();
	memset(RX3_BUFF,0,sizeof(RX3_BUFF));
	//_EINT();

}

 /*******************************************************************************
 * 函数名称: USART3_IRQHandler
 * 描    述: 中断触发将数据压入缓存
 *           简单的数据帧接受
 * 输    入: 无
 * 输    出: 无
 * 返    回: 无
 * 修改日期: 2014年3月13日 
*******************************************************************************/
static uint32_t uart3_state = 0;
static uint8_t content_cnt = 0 ;
ScreenFrameStruct ScreenFrame,*pScreenFrame;
xQueueHandle pUart3_xQueue;
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)//数据接收扫描
	{
		#if 1
		RX3_BUFF[RX_IndexW]=USART_ReceiveData(USART3);
		switch(uart3_state)
		{
			case 0:
			{
				if(FH0 == RX3_BUFF[RX_IndexW])
				{
					ScreenFrame.frame_head[0]  = RX3_BUFF[RX_IndexW];
				}
				else if(FH1 == RX3_BUFF[RX_IndexW])
				{
					ScreenFrame.frame_head[1]  = RX3_BUFF[RX_IndexW];
					uart3_state++;
				}
			}break;
			case 1:
			{
				ScreenFrame.frame_length[0]   =  RX3_BUFF[RX_IndexW]; 
				uart3_state++;
			}break;
			case 2:
			{
				ScreenFrame.frame_command[0]  =  RX3_BUFF[RX_IndexW]; 
				uart3_state++;
			}break;
			case 3:
			{
				if(ScreenFrame.frame_command[0]==0x81)
				{
					ScreenFrame.frame_variable[0] =  RX3_BUFF[RX_IndexW]; 
					uart3_state++;
				}
				else if(ScreenFrame.frame_command[0]==0x83)
				{
					if(content_cnt < 1)
					{
						ScreenFrame.frame_variable[0] =  RX3_BUFF[RX_IndexW]; 
						content_cnt++;
					}
					else
					{
						ScreenFrame.frame_variable[1] =  RX3_BUFF[RX_IndexW];
						content_cnt = 0;
						uart3_state++;
					}
				}
			}break;
			case 4:
			{
				ScreenFrame.frame_content_len[0]= RX3_BUFF[RX_IndexW];
				uart3_state++;
			}break;
			case 5:
			{
				ScreenFrame.frame_content[content_cnt++] =  RX3_BUFF[RX_IndexW];
				if((ScreenFrame.frame_length[0]-6) <= content_cnt)
				{
					content_cnt=0;
					uart3_state++;
				}
			}break;
			case 6:
			{
				ScreenFrame.frame_crc[content_cnt++] = RX3_BUFF[RX_IndexW];
				if(content_cnt >=2)
				{
					content_cnt = 0;
					uart3_state = 0;
					RX_IndexW = 0;
					pScreenFrame = &ScreenFrame ;
					xQueueSendToFrontFromISR(pUart3_xQueue,( void * )&pScreenFrame,( portTickType )0);	
				}	
			}break;
			default:break;
		}
		RX_IndexW++;
		#else
		UART_InpLen++;
		RX3_BUFF[RX_IndexW]=USART_ReceiveData(USART3);
		if(++RX_IndexW >=RX3BUF_SIZE)
		{
			RX_IndexW =0;
		}
		#endif
	}
}
