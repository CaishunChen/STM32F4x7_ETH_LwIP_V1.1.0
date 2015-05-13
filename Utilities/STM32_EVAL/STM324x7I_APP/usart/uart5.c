
#include <string.h>
#include <stm32f4xx.h>
#include "uart5.h"
#include "tcp_server.h"
void Uart5_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//外设时钟使能 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//外设时钟使能 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);//连接复用引脚  
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2  ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化串口1的GPIO  

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化串口1的GPIO  
	 
	USART_InitStructure.USART_BaudRate = 115200;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据模式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件溢出控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//双工模式
	USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
	USART_Init(UART5, &USART_InitStructure);	
	USART_ClearFlag(UART5, USART_FLAG_TC);//清传送完成标志
	USART_Cmd(UART5, ENABLE);
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //嵌套优先级分组为1
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//嵌套通道为USART6_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//响应优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道中断使能 
	NVIC_Init(&NVIC_InitStructure);
}


void Uart5_Send(uint8_t *p,uint32_t lenght)
{
	for(uint32_t i=0;i<lenght;i++)
	{
		USART_SendData(UART5,p[i]);       
		while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);  
	}
}	

/*******************************************************************************
 * 函数名称:UART5_IRQHandler                                                                     
 * 描    述:中断程序 用于与服务器之间的数据交换                                                                
 *                                                                               
 * 输    入:无                                                                     
 * 输    出:无                                                                     
 * 返    回:void                                                               
 * 修改日期:2015年5月7日                                                                    
 *******************************************************************************/
uint8_t  RX5_BUF[336] = {0}; //接受返回数据的数组
bool     RX5_RCV = false;  //是否成功接受到数据

static uint32_t Data_len = 0;   //接受的有用数据长度
static uint32_t RX5_Index = 0;   //RX5_BUF的指针
static uint32_t uart5_state = 0;  //根据接受的数据进行跳转
static uint32_t Cmd = 0;     //服务器返回命令
void UART5_IRQHandler(void)
{
	if(USART_GetITStatus(UART5,USART_IT_RXNE)!=RESET)//数据接收扫描
	{	
		USART_ClearITPendingBit(UART5,USART_FLAG_RXNE);//清接收标志
		RX5_BUF[RX5_Index] = USART_ReceiveData(UART5); //接收从服务器返回的数据
		switch(uart5_state)
		{
			case 0:  //帧头
			{
				if(RX5_BUF[RX5_Index] == StxChl)
				{
					//memset(RX5_BUF,0,sizeof(RX5_BUF)); //影响速度
					//RX5_BUF[RX5_Index] = StxChl;
					uart5_state++;
				}					
			}break;
			case 1:  //获取命令
			{
				if(RX5_Index>=2)
				{
					Cmd = RX5_BUF[1]<<8|RX5_BUF[2] ;
					switch(Cmd)
					{
						case SignInRes:
						{
							uart5_state++;
						};break;
						case MealComparRes:
						{
							uart5_state++;
						};break;    
						case StatuUploadRes:
						{
							uart5_state++;
						};break;
						case EchoRes:
						{
							uart5_state++;
						};break;
						case TkMealRes:
						{
							uart5_state++;
						};break;  
						default:
						{
							uart5_state = 0;
						}
					}
				}
			}break;
			case 2:  //获取数据包长度
			{
				if(RX5_Index>=4)
				{
					Data_len = RX5_BUF[3]<<8|RX5_BUF[4] ;
					uart5_state++;
				}	
			}break;
			case 3: //数据入队
			{
				if((RX5_Index-4) >= Data_len)
				{
					uart5_state++;
				}
			}break;
			case 4:  //帧尾
			{
				if(RX5_BUF[RX5_Index] == EtxChl)
				{
					uart5_state++;
				}
			}break;
            case 5:  //获取CRC
            {
                if(Data_len < RX5_Index - 6)
                {
					uart5_state =0;
					RX5_RCV = true;	  //标记成功
                }
            }break;    
            default:break;
		}
		if(RX5_Index<335)
        RX5_Index++;
	}
}
