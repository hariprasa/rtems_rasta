------------------------------------------
RTEMS 4.12 Environment for RASTA platform
------------------------------------------
This project provides a rtems shell environment for the RASTA platform. The interfaces Uart, SpaceWire and Ethernet are activate with the drivers and are ready to use.

--------------
Prerequisites:
--------------
1. Install RTEMS 4.12 and set it's Makefile.inc path to the variable "RTEMS_MAKEFILE_PATH". Optionaly, the RTEMS kernel can be compiled with "--enable-drvmgr" parameter to load drivermanger at startup  
2. Install RTEMS 4.12 tools such as "sparc-rtems4.12-gcc, etc." using rtems source builder
3. This project assumes the user has prior knowledge of rtems, using grmom and rasta platform

------------
Description:
------------
1. init.c             	-> main rtems file containing rtems shell sources 
2. networkconfig.h    	-> ethernet link configuration
3. system.h		-> rtems, drivers configuration configuration 
4. spw.h		-> spacewire link configuration and drivers
5. rtems_spw.c		-> spacewire link driver's sources
6. tcpip.h		-> ethernet link drivers
7. rtems_tcpip.c	-> ethernet link driver's sources
8. Makefile		-> makefile to compile and generate the executable.

--------------
project usage:
--------------
1. Compilation of the sources
   $ make
   the executable is found in o-optimize/test.exe.

2. To include new sources files for the project, Add the file name to "CSRCS" variable in Makefile

------------------
rtems shell usage:
------------------
1. rtems shell commands added in the following topics

   spacewire
   ---------
   spwinfo      - Print SpaceWire device infomation
   spwdemo      - Demostrator: transfer data from GRSPW0 to GRSPW1
   spwcnfstats  - Print SpaceWire link configuration status
   spwcnf       - Congigure SpaceWire link
   spwrcv       - Receive data in a SpaceWire link

   ethernet
   --------
   ethrxtx      - Receive and send back the IP packet
   ethserver    - Create TCP/IP server

   uart
   ----
   uartdemo     - Sends "hello world" in apbuart(/dev/console_b) at j2 connector

   system
   ------
   killtask     - Kill the user defined task

2. For usage information of shell commands as mentioned above in runtime, use as below
   # command_name -h
   
3. rtems shell command usages
   
   caution! : The options mentioned for the commands in "[....]" are mandatory.

   spwdemo
   -------
   # spwdemo -h
   This demonstrats SpaceWire link communication 
   It uses grspw0 as TX in Task-1 and grspw1 as RX in Task-2
   connect grspw0 and grspw1

   spwcnfstats
   -----------
   # spwcnfstats -h
   Print SpaceWire link configuration status 
   Usage: spwcnfstats [-h | /dev/grspwX]

   spwcnf
   ------
   # spwcnf -h
   Configure SpaceWire link 
   Usage: spwcnf  [-h|/dev/grspwX]   [SET_NODE_ADDRESS{0-255}] [SET_PROMISCUOUS{0,1}]

   spwrcv
   ------
   # spwrcv -h
   Receive data in SpaceWire link 
   Usage: spwrcv  [-h|/dev/grspwX]   [SET_NODE_ADDRESS{0-255}]   [PACKET_NOS]   [PACKET_SIZE]
   
   ethrxtx
   -------
   # ethrxtx -h
   Receive data and send back in TCP/IP link 
   Usage:  ethrxtx  [-h | SERVER_IPADDRESS] [PORT] [BUFFER_SIZE]

   ethserver
   ---------
   # ethserver -h
   Create an TCP/IP server link 
   Usage: ethserver  [-h | PORT_NUMBER] [BUFFER_SIZE]

   uartdemo
   --------
   #uartdemo -h
   task sends characters on UART /dev/UARTn_DEVICE

   killtask
   --------
   # killtask -h 
   Kill the existing userdefined TASK: TSK 
   Usage: killtask  [-h | TSK_NUMBER] 


---------------
Known problems:
---------------
1. ping to any IP address from the rtems shell results in error.

