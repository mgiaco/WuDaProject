#include "bsp.h"

CONFIG_MSG  ConfigMsg;// RecvMsg;

uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};		// 选择8个Socket每个Socket发送缓存的大小，在w5500.c的void sysinit()有设置过程
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};		// 选择8个Socket每个Socket接收缓存的大小，在w5500.c的void sysinit()有设置过程

//extern uint8 MAC[6];

void Reset_W5500(void)
{
  GPIO_ResetBits(GPIOC, WIZ_RESET);
  bsp_DelayMS(1);//datasheet上注明，reset引脚低有效，需保持低电平至少500us才能重置w5500  
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


void set_network(void)															// 配置初始化IP信息并打印，初始化8个Socket
{
  uint8 ip[4];
  setSHAR(ConfigMsg.mac);
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  sysinit(txsize, rxsize); 													// 初始化8个socket
  setRTR(2000);																			// 设置超时时间
  setRCR(3);																				// 设置最大重新发送次数
  
  getSIPR (ip);
  DEBUG(COM1, ip, 4);
  getSUBR(ip);
  DEBUG(COM1, ip, 4);
  getGAR(ip);
  DEBUG(COM1, ip, 4);
}

// 设置默认MAC、IP、GW、SUB、DNS
void set_default(NetCfg_T *NetCfg)	
{  
  uint8 lip[4]={0};
  uint8 mac[6]={0x00, 0x08, 0, 0, 0, 0};//mac前两位保持不变，值为偶数
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

  set_network();//写入w5500
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
//// 中断配置程序
//void NVIC_Configuration(void)
//{
//  NVIC_InitTypeDef NVIC_InitStructure;
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;			// 抢占优先级设为1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						// 子优先级设为0
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								// IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);																// 根据指定的参数初始化NVIC寄存器
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);  						// only app, no boot included
//}

