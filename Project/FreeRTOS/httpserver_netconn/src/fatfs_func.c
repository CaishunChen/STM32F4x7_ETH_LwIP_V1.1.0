#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <mem.h>
#include <stm32f4xx.h>
#include "ff.h"
/**************************************************************************
// 文件复制，将源文件夹复制到目标文件夹
// 将psrc文件,copy到pdst.
// psrc,pdst:源文件和目标文件
// fwmode:文件写入模式
// 0:不覆盖原有的文件
// 1:覆盖原有的文件
	
***************************************************************************/
FRESULT mf_copy(uint8_t* psrc, uint8_t* pdst, uint8_t fwmode )
{
  FRESULT res;
  uint16_t   br = 0;
  uint16_t   bw = 0;
  FIL  fsrc,fdst;
  
  uint8_t fbuf[512] = {0};
 
  if(fbuf == NULL)
  {
    res = 100;											
  } 	
  else
  {
    if(fwmode == 0)
    {
      fwmode = FA_CREATE_NEW;				
    } 	
    else
    {
      fwmode = FA_CREATE_ALWAYS;		
    } 	

    res = f_open( &fsrc, (const TCHAR *)psrc, FA_READ | FA_OPEN_EXISTING );	//打开只读文件

    if(res == 0) 
    {
      res = f_open( &fdst, (const TCHAR *)pdst, FA_WRITE | fwmode );	//打开第一个再打开第二个
    }            
    if(res == 0)					//2个都打开了
    {
      while(res == 0)			//开始复制
      {
        res = f_read(&fsrc, fbuf, 512, (UINT*)&br); 	//从源文件读出512字节
        if(res || br == 0)
        {
          break;
        }
        res = f_write(&fdst, fbuf, (UINT)br, (UINT*)&bw); // 写入目的文件
        if(res || bw < br)
        {
          break;
        }
      }
      f_close(&fsrc);
      f_close(&fdst);
    }
  }
  return res;
}


/**********************************************************
	*得到路径下的文件夹
返回值：卷标号或者文件夹的首地址

***********************************************************/
uint8_t *get_src_dname(uint8_t* dpfn)
{
  uint16_t temp = 0;
  while(*dpfn != 0)
  {
    dpfn++;   
		temp++;
  }
  if(temp < 1)
  {
    return &dpfn[0];										//卷标名，不带“/”
  }
 
	while((*dpfn != 0x3a)&&(*dpfn != 0x2f))				//追述到倒数第一个“/”或者“：”处
  {
    dpfn--;					
  } 
  return ++dpfn;
}
