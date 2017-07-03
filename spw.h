

#ifndef __SPW_H__
#define __SPW_H__

#include <stdint.h>     // uint8_t
#include <bsp/grspw.h>  

#define GRSPW_DEVICE_NAME0 "/dev/grspw0"
#define GRSPW_DEVICE_NAME1 "/dev/grspw1"
#define GRSPW_DEVICE_NAME2 "/dev/grspw2"
#define GRSPW_DEVICE_NODE0 30 // an integer in the range 0 to 255
#define GRSPW_DEVICE_NODE1 31 // an integer in the range 0 to 255
#define GRSPW_DEVICE_NODE2 32 // an integer in the range 0 to 255

/******************************************************************************/

int spw_setup(int fd, char *dev_name,int nodeaddr);

int spw_configuration_status(int fd, char *dev_name);

void spw_configuration(int fd, char *dev_name, int nodeaddr, int promiscuous);

int spw_open(char *dev_name);

void spw_close(int fd);

void spw_transmit(int fd,spw_ioctl_pkt_send *packet);

int spw_receive(int fd, char *rx_buf, int buf_size );

static char *lstates[6] = {"Error-reset", 
                    "Error-wait",
                    "Ready",
                    "Started",
                    "Connecting",
                    "Run"
		     };

void rasta_spw_receive_org(uint8_t *videoFrame, int fd);

void rasta_spw_sendback(char *buffer, int fd);

void* spw_linux_init(void* hDevice, int trans_link);

void spw_linux_close(void* hDevice, int trans_link);

void linux_spw_receive(void* hDevice, int trans_link, uint8_t *videoFrame, 
						int width, int height);

void linux_spw_sendback(void* hDevice, int trans_link, char *buffer, int bpl, 
								int height);


/******************************************************************************/
#endif /* __SPW_H__ */


