#ifndef _DEVICE_H_
#define _DEVICE_H_

#define DEVICE_ID "W5500"

#define FW_VER_HIGH   	1
#define FW_VER_LOW    	0
#define NORMAL_STATE          0

#define ON										1
#define OFF										0
#define HIGH									1
#define LOW										0

#define WIZ_SCS			    GPIO_Pin_12	// out
#define WIZ_SCLK				GPIO_Pin_13	// out
#define WIZ_MISO				GPIO_Pin_14	// in
#define WIZ_MOSI				GPIO_Pin_15	// out

#define WIZ_RESET		    GPIO_Pin_7	// out
#define WIZ_INT			    GPIO_Pin_6	// in

/* Private function prototypes -----------------------------------------------*/

//void GPIO_Configuration(void);
//void NVIC_Configuration(void);
//typedef  void (*pFunction)(void);
void set_network(void);
//void write_config_to_eeprom(void);
void set_default(NetCfg_T *NetCfg);
void Reset_W5500(void);

//void reboot(void);
//void get_config(void);

typedef struct _CONFIG_MSG
{
  uint8_t op[4];//header: FIND;SETT;FACT...
  uint8_t mac[6];
  uint8_t sw_ver[2];
  uint8_t lip[4];
  uint8_t sub[4];
  uint8_t gw[4];
  uint8_t dns[4];	
  uint8_t dhcp;
  uint8_t debug;

  uint16_t fw_len;
  uint8_t state;
  
}CONFIG_MSG;

#endif

