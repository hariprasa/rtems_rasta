

/* Standard C headers */
#include <stdio.h>      // printf()
#include <fcntl.h>      // open()
#include <errno.h>      // errno
#include <string.h>
#include <stdlib.h>     // abort()
#include <sys/ioctl.h>  // ioctl()
#include <sched.h>      // sched_yield()


/* SpaceWire on RASTA */
#include <rtems.h>      // define all RTEMS specific types
#include <bsp/grspw.h>      
#include <amba.h>       // amba_conf

#include "spw.h"

void print_config(spw_config *cnf);
//#define OPTION_INFO_ALL   0xffffffff

/*****************************************************************************/
int spw_configuration_status(int fd, char *dev_name)
{
  spw_config *config = (spw_config *)malloc(sizeof(spw_config));

  printf("[%s] Checking configuration \n", dev_name);
  if (ioctl(fd, SPACEWIRE_IOCTRL_GET_CONFIG, config) == -1) 
  {
    printf("ioctl failed: SPACEWIRE_IOCTRL_GET_CONFIG \n");
    free (config);
    return -1;
  }
  print_config(config);
  free (config);
  return 0;
}

/*****************************************************************************/

void spw_configuration(int fd, char *dev_name, int nodeaddr, int promiscuous)
{

  printf("Present configuration :\n", dev_name);
  spw_configuration_status(fd, dev_name);

  if (ioctl(fd,SPACEWIRE_IOCTRL_STOP, NULL) == -1) 
      printf("ioctl failed: SPACEWIRE_IOCTRL_STOP %s:\n", strerror(errno));

  /* important to start the link */
  if ( ioctl(fd, SPACEWIRE_IOCTRL_SET_DISCONNECT,43) == -1 ) // 850/20=43ns 
      printf("SPACEWIRE_IOCTRL_SET_DISCONNECT, errno: %d\n",errno);

  /* important to start the link */
  if ( ioctl(fd, SPACEWIRE_IOCTRL_SET_COREFREQ,0) == -1 )  // 0 = same as system freq
      printf("SPACEWIRE_IOCTRL_SET_COREFREQ, errno: %d\n",errno);

  /* may be important to start the link */
//  if ( ioctl(fd, SPACEWIRE_IOCTRL_SET_TIMER,0) == -1 )  
//      printf("SPACEWIRE_IOCTRL_SET_COREFREQ, errno: %d\n",errno);

  /* Set blocking mode for transmissions */
  if ( ioctl(fd, SPACEWIRE_IOCTRL_SET_TXBLOCK, 1) == -1 )  
    printf("SPACEWIRE_IOCTRL_SET_TXBLOCK, errno: %d \n", errno);

  /* Set blocking receiving mode */
  if ( ioctl(fd, SPACEWIRE_IOCTRL_SET_RXBLOCK, 1) == -1 ) 
    printf("SPACEWIRE_IOCTRL_SET_RXBLOCK, errno: %d \n", errno);

  /* Set the node address of the device, VERY IMPORTANT for read() */
  if (ioctl(fd, SPACEWIRE_IOCTRL_SET_NODEADDR, nodeaddr) == -1) 
    printf("ioctl failed: SPACEWIRE_IOCTRL_SET_NODEADDR, errno: %s\n", strerror(errno));

  /* Enable or disable the promiscuous mode of the device */
  if (ioctl(fd, SPACEWIRE_IOCTRL_SET_PROMISCUOUS, promiscuous) == -1) 
    printf("ioctl failed: SPACEWIRE_IOCTRL_SET_PROMISCUOUS, errno: %s\n", strerror(errno));

  if (ioctl(fd, SPACEWIRE_IOCTRL_SET_CLKDIV, 0) == -1)
    printf("ioctl failed: SPACEWIRE_IOCTRL_SET_CLKDIV %s\n", strerror(errno));

  printf("Final configuration :\n", dev_name);
  spw_configuration_status(fd, dev_name);
}

/*****************************************************************************/

int spw_setup(int fd, char *dev_name, int nodeaddr, int promiscuous)
{
  int ret_val;
  int lstatus;

  spw_configuration(fd, dev_name,nodeaddr,promiscuous); 

  /* Make sure link is up */
  while( ioctl(fd, SPACEWIRE_IOCTRL_START, 20) == -1 ) //timeout 20 ticks
  {
    printf("[%s] SPACEWIRE_IOCTRL_START failed, errno: %d \n",dev_name, errno);
    sched_yield();
  }
  printf("[%s] link is started \n", dev_name);


  if (ioctl(fd, SPACEWIRE_IOCTRL_GET_LINK_STATUS, &(lstatus)) == -1) 
     printf("ioctl failed: SPACEWIRE_IOCTRL_GET_LINK_STATUS\n");
  
  printf("[%s] Spacewire link %d is in %d-%s state\n",dev_name, nodeaddr, lstatus, lstates[lstatus]);

  return(0);
}

int spw_open(char *dev_name)
{
 int filedes;

 filedes = open(dev_name, O_RDWR);
  if (filedes < 0)
  {
    printf("Error Opening %s, errno: %s \n",dev_name, strerror(errno));
      exit(0);
  }
  else
  {
    printf("\n[%s] device is opened \n", dev_name);
    return filedes;
  }
}

void spw_close(int fd)
{
  if (ioctl(fd,SPACEWIRE_IOCTRL_STOP, NULL) == -1) {
      printf("ioctl failed: SPACEWIRE_IOCTRL_STOP %s:\n", strerror(errno));
  }
  close(fd);
}

/* SpaceWire Transmit */
void spw_transmit(int fd, spw_ioctl_pkt_send *packet)
{
#if 1
  if (ioctl(fd,SPACEWIRE_IOCTRL_SEND,packet) == -1)
    printf("ioctl failed: SPACEWIRE_IOCTRL_SEND %s:\n", strerror(errno));
#else
  int i=0;
  int ret;
  char *tx_pkt;
  tx_pkt = (char *)malloc(packet->hlen + packet->dlen);

  for (i=0; i< packet->hlen; i++)
    tx_pkt[i] = packet->hdr[i];
  for (i=0; i<= packet->dlen; i++)
    tx_pkt[packet->hlen + i] = packet->data[i];
  i=0; 

  if ((ret=write(fd, tx_pkt, 16)) <= 0 )
    printf("Write failed: errno: %s, ret: %d\n",strerror(errno),ret);

#endif
}

int spw_receive(int fd, char *rx_buf, int buf_size )
{
   int len;
   len = read(fd, rx_buf, buf_size);
   if ( len <= 0 ){
      printf("Read Failed, errno: %d, ret: %d\n",errno,len);
   }
   return len;
}


/* SpaceWire RECEIVE */
void rasta_spw_receive_org(uint8_t *videoFrame, int fd)
{
  int len;
  int i;

  for(i = 0; i < 450; i++)
  {
    len = read(fd, (char*)videoFrame + 1024 * i, 1024);
    // packetSize = 1024 (bytes), (460800 = 1024 * 450 packets)
    // only receive 1023 useful bytes per pack

    if (len < 0)
    {
      printf("Error during read, errno: %d \n", errno);
      printf("Received %d bytes \n", len);
      // to show incorrect result
    }
    else
    {
      printf("Reception successful (i = %d), received %d bytes \n", i, len);
      // the string starts from buf2[0]
    }
  }
}


/* SpaceWire SEND BACK */
void rasta_spw_sendback(char *buffer, int fd)
{
  int result;
  int i;


  // 1228800 bytes = 1024 bytes per pack * 1200 packets
  for(i = 0; i < 1200; i++)
  {
    result = write(fd, buffer + i*1024, 1024); // 1024 bytes per pack
                                               // without NODE_ADR_RX
    if (result < 0)
      printf("Transmission failed at /dev/grspw0 \n");
    else
      printf("Transmission successful (i = %d), sent %d bytes \n", i, result);
  }

  if (buffer != NULL)
    free (buffer);
}

/* SpaceWire Configuration */
void print_config(spw_config *cnf) 
{
        printf("******** SPW_CONFIG ********  \n");
        printf("Node Address: %i\n", cnf->nodeaddr);
        printf("Destination Key: %i\n", cnf->destkey);
        printf("Clock Divider: %i\n", cnf->clkdiv);
        printf("Rx Maximum Packet: %i\n", cnf->rxmaxlen);
        printf("Timer: %i\n", cnf->timer);
        printf("Disconnect: %i\n", cnf->disconnect);
        printf("Promiscuous: %i\n", cnf->promiscuous);
        printf("RMAP Enable: %i\n", cnf->rmapen);
        printf("RMAP Buffer Disable: %i\n", cnf->rmapbufdis);
        printf("Linkdisabled: %i\n", cnf->linkdisabled);
        printf("Linkstart: %i\n", cnf->linkstart);
        printf("Check Rmap Error: %i\n", cnf->check_rmap_err);
        printf("Remove Protocol ID: %i\n", cnf->rm_prot_id);
        printf("Blocking Transmit: %i\n", cnf->tx_blocking);
        printf("Blocking Tx when buffer full: %i\n", cnf->tx_block_on_full);
	printf("Blocking Receive: %i\n", cnf->rx_blocking);
        printf("Disable when Link Error: %i\n", cnf->disable_err);
        printf("Link Error IRQ Enabled: %i\n", cnf->link_err_irq);
        printf("Link Error Event Task ID: %i\n", cnf->event_id);
        printf("RMAP Available: %i\n", cnf->is_rmap);
        printf("RMAP CRC Available: %i\n", cnf->is_rmapcrc);
        printf("Unaligned Receive Buffer Support: %i\n", cnf->is_rxunaligned);
        printf("Read Nodemask: %i\n", cnf->nodemask);
        printf("Read Timeout: %i\n", cnf->rtimeout);
        printf("Keep source address in userbuffer: %i\n", cnf->keep_source);
        printf("\n");
}
