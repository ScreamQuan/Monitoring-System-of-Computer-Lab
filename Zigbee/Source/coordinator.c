#include "smartHome.h"
#include "sapi.h"

#include "osal.h"
#include "hal_uart.h"
#include "stdio.h"

#define NUM_IN_CMD_COORINATOR 3
#define NUM_OUT_CMD_COORINATOR 1
void *alloceLedDeviceNode(uint8 lednum);
void uart_receive(uint8 port,uint8 event);
const cId_t coordinatorInputCommandList[NUM_IN_CMD_COORINATOR]=
                                {LEDJOINNET_CMD_ID,HEART_BEAT_CMD_ID,TEMPERATURE_BEAT_CMD_ID};
const cId_t coordinatorOutputCommandList[NUM_OUT_CMD_COORINATOR]=
                                {TOGGLE_LED_CMD_ID};
struct led_device_node
{
  struct led_device_node *next;
  uint8 shortAddr[2];
  uint8 lostHeartCount;
  uint8 flag[1];
};

static struct led_device_node ledDeviceHeader={NULL};

/*void *alloceLedDeviceNode(uint8 lednum)
{
  return osal_mem_alloc(sizeof(struct led_device_node)-1+lednum);
}
*/
const SimpleDescriptionFormat_t zb_SimpleDesc=
{
  ENDPOINT_ID_SMARTHOME,
  PROFILE_ID_SMARTHOME,
  DEVICE_ID_COORDINATOR,
  DEVIDE_VERSION_ID,
  0,
  NUM_IN_CMD_COORINATOR,
  (cId_t*)coordinatorInputCommandList,
  NUM_OUT_CMD_COORINATOR,
  (cId_t*)coordinatorOutputCommandList  
};
/***********
ִ��ʱ�������͵����ݰ������շ��յ�ʱ������
handle:���ı�ţ�
status:ZSUCCESS��ʾ�ɹ�����
************/
void zb_SendDataConfirm( uint8 handle, uint8 status )
{
  
}

/***********
ִ��ʱ�������յ����ݰ�ʱ������
************/
void zb_ReceiveDataIndication( uint16 source, uint16 command, 
                              uint16 len, uint8 *pData  )
{
  if(command==LEDJOINNET_CMD_ID){
    char buf[100];
    struct led_device_node *p=ledDeviceHeader.next;
    while(p!=NULL)
    {
      if( pData[0]==p->flag[0])
        break;
      else
      {
        p=p->next;
      }
    }
    if(p==NULL)//�½ڵ����
    {
      struct led_device_node *np=(struct led_device_node *)osal_mem_alloc(sizeof(struct led_device_node));
      osal_memcpy(np->shortAddr,&source,2);
      np->flag[0]=pData[0];
      np->next=ledDeviceHeader.next;//ͷ��
      ledDeviceHeader.next=np; 
      p=np;
    } 
    else{
      osal_memcpy(p->shortAddr,&source,2);
    }
    sprintf(buf,"Led device come on!,shortAddr=%u,name:=%u",(uint16)p->shortAddr,(uint8)p->flag[0]);//��ӡ�ڵ��ַ�ͱ�־
    HalUARTWrite(0,buf,osal_strlen(buf));
    HalUARTWrite(0,"\r\n",2);
  }
//��������������
  else if(command==HEART_BEAT_CMD_ID) {
    struct led_device_node *p=ledDeviceHeader.next;
    while(p!=NULL)
    {
      if( osal_memcmp(&source,p->shortAddr,2)==TRUE)
        break;
      else
      {
        p=p->next;
      }
    } 
    if(p!=NULL)
    {
      p->lostHeartCount=HEART_BEAT_MAX_COUNT;
    }
  }
//��ӡ�¶�����
  else if(command==TEMPERATURE_BEAT_CMD_ID )
  {
   //uint8 buf[20];
   HalUARTWrite(0,pData,osal_strlen(pData));
   HalUARTWrite(0,"\r\n",2);
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


void zb_StartConfirm( uint8 status )
{
  halUARTCfg_t uartcfg;
  uartcfg.baudRate=HAL_UART_BR_115200;
  uartcfg.flowControl=FALSE;
  uartcfg.callBackFunc=uart_receive;
  HalUARTOpen(0,&uartcfg);
  //���ڳ�ʼ��
  if(status==ZSUCCESS)
  {
    char buf[]="Coordinator is created successfully!\r\n";
    HalUARTWrite(0,buf,osal_strlen(buf));
    osal_start_timerEx(sapi_TaskID,TIMER_TIMEOUT_EVT,2000);
  }  
}

void zb_HandleOsalEvent( uint16 event )
{
  if(event&TIMER_TIMEOUT_EVT)
  {
    struct led_device_node *p=ledDeviceHeader.next;
    struct led_device_node *pre=ledDeviceHeader.next;
    osal_start_timerEx(sapi_TaskID,TIMER_TIMEOUT_EVT,2000);
    while(p!=NULL)
    {
      p->lostHeartCount--;
      if(p->lostHeartCount<=0)
      {
         char buf[100];
         struct led_device_node *pTmp=p;
         pre->next=p->next;  
         p=p->next;
         sprintf(buf,"endpoint:shortAddr=%u is off-line\r\n",(uint16)pTmp->shortAddr);
         HalUARTWrite(0,buf,osal_strlen(buf));
         continue;
      }
      pre=p;
      p=p->next;
    }    
  }
}

void zb_FindDeviceConfirm( uint8 searchType, 
                          uint8 *searchKey, uint8 *result )
{
  
}

void uart_receive(uint8 port,uint8 event)
{
  uint16 dstAddr;
  if(event& (HAL_UART_RX_FULL|HAL_UART_RX_ABOUT_FULL|HAL_UART_RX_TIMEOUT))
  {
    //���ﲻ�Ǻ��Ͻ������ܺܺõطֳ�һ���߼����ݰ�
    uint8 buf[3];
    struct led_device_node *p=ledDeviceHeader.next;
    HalUARTRead(port,buf,2);
    while(p!=NULL)
    {
      if(p->flag[0]==buf[0])//Ѱ�ҽڵ��־
          break;
      p=p->next;
    }
    if(p!=NULL)
    {
      osal_memcpy(&dstAddr,p->shortAddr,2);
      zb_SendDataRequest(dstAddr,TOGGLE_LED_CMD_ID,
                       2,buf,0,FALSE,AF_DEFAULT_RADIUS);
    }
    
    
  }  
}
