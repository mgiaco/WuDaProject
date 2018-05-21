#include "bsp.h"

CONFIG_MSG  ConfigMsg;// RecvMsg;

uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};		// ѡ��8��Socketÿ��Socket���ͻ���Ĵ�С����w5500.c��void sysinit()�����ù���
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};		// ѡ��8��Socketÿ��Socket���ջ���Ĵ�С����w5500.c��void sysinit()�����ù���

//extern uint8 MAC[6];

void Reset_W5500(void)
{
  GPIO_ResetBits(GPIOC, WIZ_RESET);
  bsp_DelayMS(1);//datasheet��ע����reset���ŵ���Ч���豣�ֵ͵�ƽ����500us��������w5500  
  GPIO_SetBits(GPIOC, WIZ_RESET);
  bsp_DelayMS(1600);
}
//reboot 
//void reboot(void)
//{
//  pFunction Jump_To_Application;
//  uint32 JumpAddress;
//  JumpAddress = *(vu32*) (0x00000004);
//  Jump_To_Application = (pFunction) JumpAddress;
//  Jump_To_Application();
//}


void set_network(void)															// ���ó�ʼ��IP��Ϣ����ӡ����ʼ��8��Socket
{
  uint8 ip[4];
  setSHAR(ConfigMsg.mac);
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  sysinit(txsize, rxsize); 													// ��ʼ��8��socket
  setRTR(2000);																			// ���ó�ʱʱ��
  setRCR(3);																				// ����������·��ʹ���
  
  getSIPR (ip);
  DEBUG(COM1, ip, 4);
  getSUBR(ip);
  DEBUG(COM1, ip, 4);
  getGAR(ip);
  DEBUG(COM1, ip, 4);
}

// ����Ĭ��MAC��IP��GW��SUB��DNS
void set_default(NetCfg_T *NetCfg)	
{  
  uint8 lip[4]={0};
  uint8 mac[6]={0x00, 0x08, 0, 0, 0, 0};//macǰ��λ���ֲ��䣬ֵΪż��
  uint8 sub[4]={255,255,255,0};
  uint8 gw[4]={192,168,1,1};
  uint8 dns[4]={8,8,8,8};
  
  mac[2]=NetCfg->mcuID[0];
  mac[3]=NetCfg->mcuID[1];
  mac[4]=NetCfg->mcuID[2];
  mac[5]=NetCfg->mcuID[3];
  
  
  memcpy(lip, NetCfg->local_ip, 4);
 
  memcpy(ConfigMsg.lip, lip, 4);
  memcpy(ConfigMsg.sub, sub, 4);
  memcpy(ConfigMsg.gw,  gw, 4);
  memcpy(ConfigMsg.mac, mac,6);
  memcpy(ConfigMsg.dns,dns,4);

  ConfigMsg.dhcp=0;
  ConfigMsg.debug=1;
  ConfigMsg.fw_len=0;
  
  ConfigMsg.state=NORMAL_STATE;
  ConfigMsg.sw_ver[0]=FW_VER_HIGH;
  ConfigMsg.sw_ver[1]=FW_VER_LOW;

  set_network();//д��w5500
}

//void write_config_to_eeprom(void)
//{
//  uint8 data;
//  uint16 i,j;
//  uint16 dAddr=0;
//  for (i = 0, j = 0; i < (uint8)(sizeof(ConfigMsg)-4);i++) 
//  {
//    data = *(uint8 *)(ConfigMsg.mac+j);
//    at24c16_write(dAddr, data);
//    dAddr += 1;
//    j +=1;
//  }
//}
//void get_config(void)
//{
//  uint8 tmp=0;
//  uint16 i;
//  for (i =0; i < CONFIG_MSG_LEN; i++) 
//  {
//    tmp=at24c16_read(i);
//    *(ConfigMsg.mac+i) = tmp;
//  }
//  if((ConfigMsg.mac[0]==0xff)&&(ConfigMsg.mac[1]==0xff)&&(ConfigMsg.mac[2]==0xff)&&(ConfigMsg.mac[3]==0xff)&&(ConfigMsg.mac[4]==0xff)&&(ConfigMsg.mac[5]==0xff))
//  {
//    set_default();
//  }
//}
//
//// �ж����ó���
//void NVIC_Configuration(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;			// ��ռ���ȼ���Ϊ1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						// �����ȼ���Ϊ0
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQͨ��ʹ��
//	NVIC_Init(&NVIC_InitStructure);																// ����ָ���Ĳ�����ʼ��NVIC�Ĵ���
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);  						// only app, no boot included
//}

