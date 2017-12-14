------------------------------------------
RTEMS 4.12 Environment for RASTA platform
------------------------------------------
This project provides a rtems shell environment for the RASTA platform. The interfaces Uart, SpaceWire and Ethernet are activate with the drivers and are ready to use. This uses RTEMS driver manager for driver initialization. 

Preinstalled drivers for: 
1. Ethernet interface GAISLER_ETHMAC
2. Can interface GAISLER_GRCAN
3. SpaceWire interface GAISLER_SPW2
4. UART interface GAISLER_APBUART

Interface Configuration: 
1. Ethernet interface 
	-> J2-ETH: IP address: 192.168.0.81, Net Mask: 255.255.255.0
2. Spacewire interface
	-> SPW-0: grspw0, Node Address: 30
        -> SPW-1: grspw1, Node Address: 31
	-> SPW-2: grspw2, Node Address: 32
3. UART interface
	-> J2: console_b, settings: 8N, baudrate 115200 
4. CAN interface
	-> CAN-A/B: grcanO
	usage of CAN interface is not addressed in this rtems shell environment.

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

3. Openning GRMON
   -> for connecting grmon to RASTA using Ethernet. 
      $ /path/grmon -eth -emem2 -ip 192.168.0.78 -udp 8000 -u 0 -log
      (note1: the ip address of edcl and udp port may vary according to RASTA's configuration)
      (note2: the computer connecting with RASTA(at J2-ETH interface) should be in same network)


   -> for connecting grmon to RASTA using UART
      $ /path/grmon -v -u -uart /dev/ttyUSB0 -log
      (note1: Connect an UART cable form J1-DSU to computer with Grmon installed)
      (note2: the name of UART device may vary according to host computer of grmon )

4. Loading the program to RASTA
    After opening the GRMON debug interface in the host computer, the executable generated in step-1
    can be loaded by using the command in grmon interface as below 
    $ load /path/test.exe

    (note1: Loading can be faster when Grmon connects with RASTA using ethernet )


5. Executing the shell
    After loading the Shell program to the RASTA board, The rtems shell can be executed by using 
    the command in grmon interface as below
    $ run
    
6. RTEMS Shell environment
    The above command starts rtems shell environment in the RASTA board. The rtems shell opens up
    after loading the drivers and intializing ethernet network. The RTEMS shell is ready to execute
    the user commands. Use "help" command to know the support for possible rtems shell commands. In
    addition to predefined RTEMS shell commands, some of the userdefined commands are defined in
    this project to demonstrate the communication interfaces as given section below
   

------------------
rtems shell usage:
------------------
1. Following rtems shell commands added in the respective topics

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
2. Bugs in UART Receive communication

