
#define ENABLE_NETWORK
//#define RTEMS_NETWORKING

#define CONFIGURE_MAXIMUM_DRIVERS           10   
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
#define CONFIGURE_APPLICATION_NEEDS_NULL_DRIVER 1

#define CONFIGURE_MAXIMUM_TASKS             20
#define CONFIGURE_MAXIMUM_SEMAPHORES        20
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES    20
#define CONFIGURE_EXTRA_TASK_STACKS         (50*RTEMS_MINIMUM_STACK_SIZE)
//#define CONFIGURE_EXECUTIVE_RAM_SIZE    (1024*1024)
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 20
//#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM
//#define CONFIGURE_FILESYSTEM_ALL
#define CONFIGURE_STACK_CHECKER_ENABLED
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MALLOC_STATISTICS
#define CONFIGURE_UNIFIED_WORK_AREAS
#define CONFIGURE_UNLIMITED_OBJECTS

//for spacewire
#define CONFIGURE_INIT_TASK_STACK_SIZE (10*1024)
#define CONFIGURE_INIT_TASK_PRIORITY   120
#define CONFIGURE_INIT_TASK_INITIAL_MODES (RTEMS_PREEMPT | \
                                           RTEMS_NO_TIMESLICE | \
                                           RTEMS_NO_ASR | \
                                           RTEMS_INTERRUPT_LEVEL(0))   

#include <rtems/confdefs.h>

/* RTEMS shell configuration*/

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL_NETWORKING
#define CONFIGURE_SHELL_COMMANDS_ALL
#define CONFIGURE_SHELL_COMMAND_DRVMGR
#define CONFIGURE_SHELL_NO_COMMAND_HEXDUMP
#define CONFIGURE_SHELL_NO_COMMAND_MKRFS
#define CONFIGURE_SHELL_NO_COMMAND_MSDOSFMT
#define CONFIGURE_SHELL_NO_COMMAND_MOUNT
#define CONFIGURE_SHELL_NO_COMMAND_UNMOUNT
#define CONFIGURE_SHELL_NO_COMMAND_DEBUGRFS
#define CONFIGURE_SHELL_NO_COMMAND_DF
#define CONFIGURE_SHELL_NO_COMMAND_MD5
#define CONFIGURE_SHELL_NO_COMMAND_MALLOC_INFO
#define CONFIGURE_SHELL_NO_COMMAND_RTRACE
#define CONFIGURE_SHELL_NO_COMMAND_RTC
#define CONFIGURE_SHELL_NO_COMMAND_PCI
//#define CONFIGURE_SHELL_USER_COMMANDS &Shell_USERCMD_Command
//#define CONFIGURE_SHELL_USER_COMMANDS &shell_spwinfo
#include <rtems/shellconfig.h>

/* Configure Driver manager */
#if defined(RTEMS_DRVMGR_STARTUP)/* if --drvmgr was given to configure */
 /* Add Timer and UART Driver for this example */
  #ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
   #define CONFIGURE_DRIVER_AMBAPP_GAISLER_GPTIMER
  #endif
  #ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
   #define CONFIGURE_DRIVER_AMBAPP_GAISLER_APBUART
  #endif
#endif
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRETH   /* GRETH Driver */
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRSPW   /* GRSPW Driver */
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GRCAN   /* GRCAN Driver */
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GR1553B
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GR1553BM
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GR1553BC
#define CONFIGURE_DRIVER_AMBAPP_GAISLER_GR1553RT


#include <drvmgr/drvmgr_confdefs.h> 
	
void system_init(void);

#if 1  /* Enable driver manager usage https://devel.rtems.org/wiki/TBR/UserManual/DriverManager */
#include <drvmgr/ambapp_bus_grlib.h>
#include <drvmgr/ambapp_bus.h>
#include <ambapp_ids.h>
#include <drvmgr/drvmgr.h>

/* GRSPW0 resources  */
struct drvmgr_key grlib_grspw0_res[] =

{
	{"txDesc", DRVMGR_KT_INT, {(unsigned int)16}},
	{"rxDesc", DRVMGR_KT_INT, {(unsigned int)32}},
	DRVMGR_KEY_EMPTY
};
/* GRSPW1 and GRSPW2 resources */
struct drvmgr_key grlib_grspw1_res[] =

{
	{"txDesc", DRVMGR_KT_INT, {(unsigned int)16}},
	{"rxDesc", DRVMGR_KT_INT, {(unsigned int)32}},
	DRVMGR_KEY_EMPTY
};
/* GRSPW1 and GRSPW2 resources */
struct drvmgr_key grlib_grspw2_res[] =

{
	{"txDesc", DRVMGR_KT_INT, {(unsigned int)16}},
	{"rxDesc", DRVMGR_KT_INT, {(unsigned int)32}},
	DRVMGR_KEY_EMPTY
};
struct drvmgr_key grlib_greth0_res[] =

{
	{"txDescs", DRVMGR_KT_INT, {(unsigned int)16}},
	{"rxDescs", DRVMGR_KT_INT, {(unsigned int)32}},
	DRVMGR_KEY_EMPTY
};
struct drvmgr_key grlib_grcan0_res[] =

{
	{"txDescs", DRVMGR_KT_INT, {(unsigned int)16}},
	{"rxDescs", DRVMGR_KT_INT, {(unsigned int)32}},
	DRVMGR_KEY_EMPTY
};
/* GRLIB Plug & Play bus driver resources */
struct drvmgr_drv_res grlib_drv_resources[] =

{
	{DRIVER_AMBAPP_GAISLER_GRSPW2_ID, 0, &grlib_grspw0_res[0]},
	{DRIVER_AMBAPP_GAISLER_GRSPW2_ID, 1, &grlib_grspw1_res[0]},
	{DRIVER_AMBAPP_GAISLER_GRSPW2_ID, 2, &grlib_grspw2_res[0]},
	{DRIVER_AMBAPP_GAISLER_GRCAN_ID, 0, &grlib_grcan0_res[0]},
	{DRIVER_AMBAPP_GAISLER_GRETH_ID, 0, &grlib_greth0_res[0]},
	DRVMGR_RES_EMPTY
};

struct grlib_config grlib_bus_config = 
{
	&ambapp_plb,		/* AMBAPP bus setup */
	&grlib_drv_resources,	/* Driver configuration */
};
void system_init(void)
{
	/* Register GRLIB root bus */
	ambapp_grlib_root_register(&grlib_bus_config);

        /* Initializing Driver Manager if not alread performed by BSP */
  	printf("Initializing Driver manager...\n");
  	if ( drvmgr_init() ) {
  	   printf("Driver manager Failed to initialize\n");
	   exit(-1);
	}
	/* Print Driver manager drivers and their assigned devices */
  	printf("[drvmgr] ");
  	drvmgr_summary();

  	/* Initialize networking */
 	printf("Initializing network...\n");
  	rtems_bsdnet_initialize_network ();
  	printf("[netmgr] ");
	rtems_bsdnet_show_inet_routes();
}


#endif  /* Enable driver manager usage */
