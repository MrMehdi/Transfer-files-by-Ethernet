#include "FreeRtOS.h"
#include "task.h"
#include "portmacro.h"
#include "led.h"
#include "lcd.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"
#include "TskInit.h"
static const char TskInitName[7] = "TskInit";
static unsigned short TskInitStackDepth = 128;						//�����ջ��ȣ���λΪ�ֽ�(word)
static UBaseType_t TskInitPri = 5;									//�������ȼ�,0Ϊ���
static TaskHandle_t TskInitHandler;									//������

//������
void TskInit(void *arg)
{
	u32 total,free;
	u8 res;
	while(SD_Init())//��ⲻ��SD��
	{
		LCD_ShowString(30,400,200,24,24,(u8 *)"SD Card Error!");
		vTaskDelay(100);					
		LCD_ShowString(30,400,200,24,24,(u8 *)"Please Check! ");
		vTaskDelay(100);
	}
	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
  	f_mount(fs[0],"0:",1); 					//����SD�� 
 	res=f_mount(fs[1],"1:",1); 				//����FLASH.	
	if(res==0X0D)//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
		LCD_ShowString(30,400,200,24,24,(u8 *)"Flash Disk Formatting...");	//��ʽ��FLASH
		res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:ALIENTEK");	//����Flash���̵�����Ϊ��ALIENTEK
			LCD_ShowString(30,400,200,24,24,(u8 *)"Flash Disk Format Finish");	//��ʽ�����
		}
		else 
			LCD_ShowString(30,400,200,24,24,(u8 *)"Flash Disk Format Error ");	//��ʽ��ʧ��
		vTaskDelay(1000);
	}													    
	LCD_Fill(30,400,240,400+24,BLACK);		//�����ʾ	

	while(1)
	{
		while(exf_getfree((u8 *)"1:",&total,&free))	//�õ��ⲿFLASH����������ʣ������
		{
			LCD_ShowString(30,400,200,24,24,(u8 *)"FLASH Fatfs Error!");
			vTaskDelay(200);
			LCD_Fill(30,400,240,400+24,BLACK);	//�����ʾ			  
			vTaskDelay(200);
		}
		LCD_ShowString(30,400,200,24,24,(u8 *)"FATFS OK!");	 
		LCD_ShowString(30,430,300,24,24,(u8 *)"FLASH Total Size :    MB");	 
		LCD_ShowString(30,460,300,24,24,(u8 *)"FLASH  Free Size :    MB"); 	    
		LCD_ShowNum(30+12*19,430,total>>10,2,24);					//��ʾFLASH�������� MB
		LCD_ShowNum(30+12*19,460,free>>10,2,24);					//��ʾFLASH��ʣ������ MB
		while(exf_getfree((u8 *)"0",&total,&free))	//�õ�SD������������ʣ������
		{
			LCD_ShowString(30,400,240,24,24,(u8 *)"SD Card Fatfs Error!");
			vTaskDelay(200);
			LCD_Fill(30,400,240,400+24,BLACK);	//�����ʾ			  
			vTaskDelay(200);
		}
		LCD_ShowString(30,490,300,24,24,(u8 *)"SD Total Size :     MB");	 
		LCD_ShowString(30,520,300,24,24,(u8 *)"SD  Free Size :     MB"); 	    
		LCD_ShowNum(30+12*16,490,total>>10,3,24);					//��ʾSD�������� MB
		LCD_ShowNum(30+12*16,520,free>>10,3,24);					//��ʾSD��ʣ������ MB
		vTaskDelay(500);
	}

	vTaskSuspend(NULL);  					//������������
}
int TskInitCreate(void)
{
	BaseType_t err = xTaskCreate(TskInit, TskInitName, TskInitStackDepth, (void *)NULL, 
		TskInitPri, (TaskHandle_t *)&TskInitHandler);
	
	return 0;
}
