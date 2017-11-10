/*
 * Network configuration
 
**IMP** this function brings the leon_eth driver
RTEMS_BSP_NETWORK_DRIVER_ATTACH
 */

#ifndef _RTEMS_NETWORKCONFIG_H_
#define _RTEMS_NETWORKCONFIG_H_

#include <rtems/rtems_bsdnet.h>
#include <bsp.h> 
#include <bsp/network_interface_add.h>
#define LARGE_LOMTU 1
/* Default network interface for leon_greth.c*/
#if 0
static struct rtems_bsdnet_ifconfig netdriver_config = {
        RTEMS_BSP_NETWORK_DRIVER_NAME,          /* name */
        RTEMS_BSP_NETWORK_DRIVER_ATTACH,        /* attach function */
        NULL,                           /* next interfaces */
        "192.168.0.80",                 /* IP address */
        "255.255.255.0",                /* IP net mask */
        NULL,                           /* Driver supplies hardware PHY address */
        0,                              /* ignore broadcast */
        0,                              /* mtu */
        0,                              /* rbuf_count */
        0,                              /* xbuf_count */
        0,                              /* port */
        0,                              /* irq */
        0,                              /* bpar */
        NULL                            /* driver control pointer */
};

#endif


#if 0
/*
 * Loopback interface
 */

extern void rtems_bsdnet_initialize_loop();
static struct rtems_bsdnet_ifconfig loopback_config = {
        "loop0",                        /* name */
        rtems_bsdnet_initialize_loop,        /* attach function */
//	rtems_bsdnet_attach,
        NULL,                           /* link to next interface */
        "127.0.0.1",                    /* IP address */
        "255.0.0.0",                    /* IP net mask */
	NULL,                           /* hardware_address */
	0,
};
#endif

/* Table used by network interfaces that register themselves using the
 * network_interface_add routine. From this table the IP address, netmask 
 * and Ethernet MAC address of an interface is taken.
 *
 * The network_interface_add routine puts the interface into the
 * rtems_bsnet_config.ifconfig list.
 *
 * Set IP Address and Netmask to NULL to select BOOTP.
 */
struct ethernet_config interface_configs[] =
{
       { "192.168.0.81", "255.255.255.0", {0x80,0x80,0x80,0x80,0x80,0x80}},// NULL - take PHY address and IP from device
  };

/*
 * Network configuration
 */
struct rtems_bsdnet_config rtems_bsdnet_config = {
//	&netdriver_config,	/* link to next interface */
//	&loopback_config,
	NULL,
	NULL,			/* BOOTP */
	100,			/* Default network task priority IMP NETWORK is also a TASK */
	128*1024,		/* Default mbuf capacity */
	256*1024,		/* Default mbuf cluster capacity */
	"rtems_host",		/* Host name */
	"localnet",		/* Domain name */
	"192.168.0.1",		/* Gateway */
	"192.168.0.1",		/* Log host */
	{"192.168.0.1" },	/* Name server(s) */
	{"192.168.0.1" },	/* NTP server(s) */
	0,                      /* sb_efficiency */
	0,                      /* udp_tx_buf_size */
	0,                      /* udp_rx_buf_size */
	0,                      /* tcp_tx_buf_size */
	0                       /* tcp_rx_buf_size */
};


#endif /* _RTEMS_NETWORKCONFIG_H_ */




