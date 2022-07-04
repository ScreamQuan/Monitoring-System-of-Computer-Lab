#include "smartHome.h"
#include"OnBoard.h"
#include "sapi.h"
#include "hal_led.h"
#include "ds18b20.h"
#include "hal_adc.h"
#include "hal_sleep.h"




#define NUM_IN_CMD_LEDDEVICE 1
#define NUM_OUT_CMD_LEDDEVICE 3
const cId_t ledDeviceInputCommandList[NUM_IN_CMD_LEDDEVICE]=
                                {TOGGLE_LED_CMD_ID};
const cId_t ledDeviceOutputCommandList[NUM_OUT_CMD_LEDDEVICE]=
                                {LEDJOINNET_CMD_ID,HEART_BEAT_CMD_ID,TEMPERATURE_BEAT_CMD_ID};
const SimpleDescriptionFormat_t zb_SimpleDesc=
{
  ENDPOINT_ID_SMARTHOME,
  PROFILE_ID_SMARTHOME,
  DEVICE_ID_LEDDEVICE,
  DEVIDE_VERSION_ID,
  0,
  NUM_IN_CMD_LEDDEVICE,
  (cId_t*)ledDeviceInputCommandList,
  NUM_OUT_CMD_LEDDEVICE,
  (cId_t*)ledDeviceOutputCommandList  
};
/***********
ִ��ʱ�������͵����ݰ������շ��յ�ʱ������
handle:���ı�ţ�
status:ZSUCCESS��ʾ�ɹ�����
************/

void Led_Init()
{
 P1SEL |= 0XEC;
 P1DIR |= 0X13;
}

void Light_Init()
{
  P1SEL &= ~0x20;                 //����P1.5Ϊ��ͨIO��
  P1DIR &= ~0x20;                 //P1.5����Ϊ�����
}

void Buzzer_Init()
{
  P0SEL &= ~0x01;                 //����P07Ϊ��ͨIO��
  P0DIR |= 0x01;                 //P07����Ϊ�����

}

void Delay_ms(uint8 k)
{
   MicroWait(k);//������ʱ
}

uint8 GetLight()//��ǿ���
{
    uint8 light=0;//�ٷֱȵ�����ֵ
    float vol=0.0; //adc������ѹ  
    uint16 adc= HalAdcRead(HAL_ADC_CHANNEL_7, HAL_ADC_RESOLUTION_14); //ADC ����ֵ P06��
    //������ֵ8192(��Ϊ���λ�Ƿ���λ)
    if(adc>=8192)
    {
        return 0;
    }
    adc=8192-adc;//����һ�£���Ϊ��ʪ��ʱAO������ϸߵ�ƽ
                   //ʪ��ʱAO������ϵ͵�ƽ   
    //ת��Ϊ�ٷֱ�
    vol=(float)((float)adc)/8192.0;
    //ȡ�ٷֱ���λ����
    light=vol*100;
    //light=adc;
    return light;
}
void zb_SendDataConfirm( uint8 handle, uint8 status )
{
  
}

/***********
ִ��ʱ�������յ������ݰ�������
************/
void zb_ReceiveDataIndication( uint16 source, uint16 command, 
                              uint16 len, uint8 *pData  )
{
  if(command==TOGGLE_LED_CMD_ID)
  {
   
    uint8 flag[2];
          flag[0] = pData[0];
          flag[1] = pData[1];
    if(flag[0]==0x30)
    {
      if(flag[1]==0x31)
        LED_3 = 1;
      else
        LED_3 = 0;
     
    }
    else if(flag[0]==0x31)
    {
       if(flag[1]==0x31)
        LED_3 = 1;
      else
        LED_3 = 0;  
    }
  }
}


void zb_AllowBindConfirm( uint16 source )
{
}

void zb_HandleKeys( uint8 shift, uint8 keys )
{
  
}

void zb_BindConfirm( uint16 commandId, uint8 status )
{
}


//void zb_SendDataRequest ( uint16 destination, uint16 commandId, uint8 len,
//                          uint8 *pData, uint8 handle, uint8 ack, uint8 radius );
void zb_StartConfirm( uint8 status )
{
  
  if(status==ZSUCCESS)
  {
    Led_Init();//��ʼ��led
    Ds18b20Initial();//��ʼ���¶�
    uint8 flag[1];//��־
    flag[0] = 0X30;//�ڵ�һ��־
    //�ɰѽڵ���������led�Ƶ�ID�ŷ��͹�ȥ
    zb_SendDataRequest(0X0,LEDJOINNET_CMD_ID,
                       1,flag,0,FALSE,AF_DEFAULT_RADIUS);
    osal_start_timerEx(sapi_TaskID,TIMER_TIMEOUT_EVT,2000);
    osal_start_timerEx(sapi_TaskID,TEMPERATURE_TIMEOUT_EVT,1000);
  }
}

void zb_HandleOsalEvent( uint16 event )
{
  if(event&TIMER_TIMEOUT_EVT)//�������¼�
  {
    osal_start_timerEx(sapi_TaskID,TIMER_TIMEOUT_EVT,2000);
    zb_SendDataRequest(0X0,HEART_BEAT_CMD_ID,
                       0,NULL,0,FALSE,AF_DEFAULT_RADIUS); 
  }
  
  if(event&TEMPERATURE_TIMEOUT_EVT)//�¶��¼�
  {
    uint8 T[6];    //�¶�+��ʾ��     
    Temp_test();   //�¶ȼ��    
    T[0]=temp/10+48;
    T[1]=temp%10+48;
    T[2]='\r';
    T[3]=GetLight()/10+0X30;//��ǿ���
    T[4]=GetLight()%10+0X30;
    T[5]='\0';
    osal_start_timerEx(sapi_TaskID,TEMPERATURE_TIMEOUT_EVT,1000);
    zb_SendDataRequest(0X0,TEMPERATURE_BEAT_CMD_ID ,
                       6,T,0,FALSE,AF_DEFAULT_RADIUS); 
     if(GetLight()> 80)
    {
      buzzer = 1;
      Delay_ms(100);
      
    }
    else
    {
      buzzer = 0;
    }
  }
 
  }


void zb_FindDeviceConfirm( uint8 searchType, 
                          uint8 *searchKey, uint8 *result )
{
  
}