#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/shell.h>
#include <drvmgr/drvmgr.h>
#include "spw.h"
#include "tcpip.h"
#include <errno.h>     
#include <bsp/grspw.h>
#include <drvmgr/ambapp_bus.h>
#include <fcntl.h>
#include <sys/socket.h> // AF_INET, SOCK_STREAM, MSG_WAITALL
#include <netinet/in.h> // struct sockaddr_in
#define ENABLE_GRETH 1
#define UART1_DEVICE "/dev/console_b"

/****************************************************************************/
/* Task configuration                                                       */
/****************************************************************************/

rtems_task_priority Priorities[9] = {10, 20, 21, 22, 23, 24, 25, 26, 27}; 
int                 Periods[9]    = {100, 100, 200, 300, 400, 500, 600, 700, 800}; 
rtems_id            Task_id[9]    = {0, 0, 0, 0, 0, 0, 0, 0, 0};    
rtems_id            Sem_id;
rtems_name          Task_name[9]  = {0, 0, 0, 0, 0, 0, 0, 0, 0};
rtems_name          Sem_name;

#define TASK1_PRIORITY 122   // SpaceWire demo receiver task
#define TASK2_PRIORITY 121   // SpaceWire demo transmit task
#define TASK3_PRIORITY 122   // SpaceWire generic receiver task 
#define TASK4_PRIORITY 122   // Ethernet TCPIP Server task
#define TASK5_PRIORITY 122   // Ethernet TCPIP Client task
#define TASK6_PRIORITY 90    // Uart demo transmit task
#define TASK7_PRIORITY 90    // Uart demo transmit task
/****************************************************************************/
/* The_Task1(): body of the task                                             */
/****************************************************************************/

rtems_task The_Task1(rtems_task_argument unused){ 

  int fd0;
  int i=0;
  int ret;

  char dst_node=GRSPW_DEVICE_NODE1;
  spw_ioctl_pkt_send *pack= (spw_ioctl_pkt_send *)malloc(sizeof(spw_ioctl_pkt_send));

  pack->hlen=1;
  pack->hdr= &dst_node;
  pack->dlen=15;
  pack->data="TEST_DATA SPW";
  pack->sent=0;
  pack->options=0;

  printf ("[Task 1] : Bring up link SPW0 \n"); 

  fd0 = spw_open(GRSPW_DEVICE_NAME0);

  spw_setup(fd0,GRSPW_DEVICE_NAME0,GRSPW_DEVICE_NODE0,0);
  
  printf ("[Task 1] : starting to transmit in SPW0\n"); 
  while(1){
	if ( i>9 )
	  break;
	printf("[Task 1] -------------- Sending Initiator ----------------\n");
	printf("[Task 1]Sending Data: %s, destination node: 0x%x\n", pack->data, dst_node);

 	spw_transmit(fd0, pack);

	sleep(1);
	 i++;
  }
  printf("[Task 1] Transmit complete\n");
  if (ioctl(fd0,SPACEWIRE_IOCTRL_STOP) == -1) {
      printf("ioctl failed: SPACEWIRE_IOCTRL_STOP %s:\n", strerror(errno));
  }

  free(pack);
  spw_close (fd0);
  printf("[%s] device is closed \n", GRSPW_DEVICE_NAME0);
  rtems_task_delete(RTEMS_SELF);
}

/****************************************************************************/
/* The_Task2(): body of the task                                             */
/****************************************************************************/
rtems_task The_Task2(rtems_task_argument unused){ 
 
  int fd1;
  int i=0,j;
  int len;
  char *rx_pkt;
  rx_pkt = (char *)malloc(1024);

  for (j=0;j<1024;j++)
    rx_pkt[j]= NULL;

  printf ("[Task 2] : Bring up link SPW1\n"); 

  fd1 = spw_open(GRSPW_DEVICE_NAME1);
  spw_setup(fd1, GRSPW_DEVICE_NAME1,GRSPW_DEVICE_NODE1,0);
  printf ("[Task 2] : Ready to receive in SPW1\n"); 

  while(i<10){
	
	printf("[Task 2] -------------- Receiving Initiator ----------------\n");
        
        len = spw_receive(fd1,rx_pkt,256);
	
	/* handle message */
	printf("[Task 2] :Got Message of length %d \n",len);
        printf("[Task 2] :Message data: [ 0x%x 0x%x ...] (%s)\n",rx_pkt[0],rx_pkt[1],rx_pkt);
        //printf("[Task 2] :ASCII   data: [ %d %d ...] (%s)\n",rx_pkt[0],rx_pkt[1],rx_pkt);
        i++;
        sleep(1);
  }

  if (ioctl(fd1,SPACEWIRE_IOCTRL_STOP) == -1) {
      printf("ioctl failed: SPACEWIRE_IOCTRL_STOP %s:\n", strerror(errno));
  }

  printf("[Task 2] Receive complete\n");
  free(rx_pkt);
  spw_close (fd1);
  printf("[%s] device is closed \n", GRSPW_DEVICE_NAME1);
  rtems_task_delete(RTEMS_SELF);
}

/****************************************************************************/
/* The_Task3(): SPW RECEIVER body of the task                               */
/****************************************************************************/
rtems_task The_Task3(char *argv[])
{ 
  const char* device = argv[1];
  char *rx_pkt;
  int count=0;
  int len;
  int fd;
  rx_pkt = (char *)malloc(4096);
  argv[2]= atoi(argv[2]);
  argv[3]= atoi(argv[3]);
  argv[4]= atoi(argv[4]);
  printf ("Bringing up link %s with node address %d \n",argv[1], argv[2]); 

  fd = spw_open(device);
  spw_setup(fd,argv[1],argv[2],1);
  printf ("Ready to receive %d packets of size %d bytes \n", argv[3],argv[4]);
  
  while(count < argv[3]){
	
	printf("-------------- Receiving Initiator ----------------\n");

        len = spw_receive(fd,rx_pkt,argv[4]);
	if (len < 0)
	  break;

	/* handle message */
	printf("Got Message of length %d \n",len);
        printf("Message data: [ 0x%x 0x%x 0x%x 0x%x...]\n",rx_pkt[0],rx_pkt[1],rx_pkt[2],rx_pkt[3]);
        //printf("ASCII   data: [   %d   %d   %d   %d...]\n",rx_pkt[0],rx_pkt[1],rx_pkt[2],rx_pkt[3]);
	count++;
        sleep(1);
       }
  
  if (ioctl(fd,SPACEWIRE_IOCTRL_STOP) == -1) {
      printf("ioctl failed: SPACEWIRE_IOCTRL_STOP %s:\n", strerror(errno));
  }

  printf("Receive complete\n");
  free(rx_pkt);
  spw_close(fd);
  printf("[%s] device is closed and task END\n",device);
  rtems_task_delete(RTEMS_SELF);
}

/****************************************************************************/
/* The_Task4(): TCPIP SERVER body of the task                               */
/****************************************************************************/
void eth_server_tcpip(int portno,int buffersize)
{
  struct sockaddr_in serv_addr, cli_addr;
  int sockfd;
  int clilen;
  int clisockfd;
  //char *txbuffer;
  char *rxbuffer;

  //txbuffer = (char *)malloc(buffersize);
  rxbuffer = (char *)malloc(buffersize);

  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd < 0) {
      perror("[greth_server] Cannot create the socket");
      exit(1);
  }

  /*Initialize socket structure*/
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind the host address */
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
     perror("[greth_server] ERROR on binding");
     exit(1);
  }
  
  printf("[greth_server]----------- Listening Initiator at port: %d -------------\n", serv_addr.sin_port);
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
   
  /* Accept actual connection from the client */
  clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  if (clisockfd < 0) {
     perror("[greth_server] ERROR on accept");
     exit(1);
  }

  eth_transmit("Hello from server", 17, clisockfd);
  while(1)
  {
   /* If connection is established then start communicating */
   bzero(rxbuffer,buffersize);
   //bzero(txbuffer,buffersize);
   printf("[greth_server]-------------- Receiving Initiator ----------------\n");
   eth_receive(rxbuffer,buffersize,clisockfd);

   printf("[greth_server] Received the message: %s\n",rxbuffer);
   
   /* Write a response to the client */
   eth_transmit("got the message", 18, clisockfd);
   }

   free(rxbuffer);
   //free(txbuffer);
   socket_close(clisockfd);
   socket_close(sockfd);
}

rtems_task The_Task4(char *argv[])
{ 
  int port;
  int buf_size;
  port =  atoi(argv[1]);
  buf_size= atoi(argv[1]);
  eth_server_tcpip(port, buf_size);
  rtems_task_delete(RTEMS_SELF);
}
/****************************************************************************/
/* The_Task5(): TCPIP CLIENT body of the task                               */
/****************************************************************************/
rtems_task The_Task5(char *argv[])
{
  char *server_name = argv[1];//"192.168.0.81";
  int port = atoi(argv[2]);//1234;
  int buf_size= atoi(argv[3]);
  int count;
  int length;
  int sd;
  char *rx_pkt;
  rx_pkt = (char *)malloc(buf_size);
 

  /* Configure the socket */
  sd = Configure_Socket_Client(server_name,port);
  if (sd > 0)
  {
    printf("[greth_client] Connected to server: %s and port:%d \n", server_name,port);
    eth_transmit("Hello from client", 17, sd);
//    for (count = 0; count <=10; count++)
    while(1)
     {
      bzero(rx_pkt,buf_size);
      sleep(1);
      length = eth_receive(rx_pkt,buf_size,sd);
      printf("[greth_client] Received the message: %s\n",rx_pkt);
      sleep(1);
      eth_transmit(rx_pkt,length,sd);
     }
  }
  else
    printf("[greth_client] Error Cannot connet to server:%s and port:%d \n", server_name,port);
  free(rx_pkt);
  socket_close(sd);
  rtems_task_delete(RTEMS_SELF);
}

/*******************************************************************************
 * RTEMS uartdemo Task
 ******************************************************************************/

rtems_task uartdemotask(rtems_task_argument unused){ 

	int len;
	struct termios term;
	char buf[50];
	int fd,i,j;

	printf("\nHello World on System Console\n");
	fflush(NULL);

	/* Open and setup uart */

		fd = open(UART1_DEVICE, O_RDWR);
		if ( fd < 0 ) {
			printf("Failed to open %s.\nCause open failed, ERRNO: %d = %s\n\n\n",UART1_DEVICE,errno,strerror(errno));
		}

		/* Get current configuration */
		tcgetattr(fd, &term);

		/* Set Console baud */
		cfsetospeed(&term, B115200);
		cfsetispeed(&term, B115200);

		/* Do not echo chars */
		term.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|ECHOPRT|ECHOCTL|ECHOKE);

		/* Turn off flow control */
		term.c_cflag |= CLOCAL;

		/* Update driver's settings */
		tcsetattr(fd, TCSANOW, &term);


	fflush(NULL);

	/* Send a string of every UART and compare it with the received
	 * string
	 */

	for (j=1; j<=10; j++) {
		/* Prepare unique string to send */
		len = sprintf(buf, "%d. Hello World\n", j );
		printf("sending on %s :::: %s \n", UART1_DEVICE,buf);
		/* Send the string */
		for(i=0; i<len; i++) {
			/* Send 1 char */
			if ( write(fd, &buf[i], 1) != 1 ) {
			printf("Failed to send character\n");
			exit(0);
			}
		/* Force send */
		fflush(NULL);
		}	
	}	
	
	/* Tell everything is done. */
	printf("Complete Test OK.\n");
	close(fd);
	rtems_task_delete(RTEMS_SELF);
}

/*******************************************************************************
 * RTEMS uartreceiver Task
 ******************************************************************************/

rtems_task uartreceiver(rtems_task_argument unused){ 

	int len;
	struct termios term;
	char buf[20]= "";
	int fd,i,j;

	/* Open and setup uart */

		fd = open(UART1_DEVICE, O_RDWR);
		if ( fd < 0 ) {
			printf("Failed to open %s.\nCause open failed, ERRNO: %d = %s\n\n\n",UART1_DEVICE,errno,strerror(errno));
		}

		/* Get current configuration */
		tcgetattr(fd, &term);

		/* Set Console baud */
		cfsetospeed(&term, B115200);
		cfsetispeed(&term, B115200);

		/* Do not echo chars */
		term.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|ECHOPRT|ECHOCTL|ECHOKE);

		/* Turn off flow control */
		term.c_cflag |= CLOCAL;

		/* Update driver's settings */
		tcsetattr(fd, TCSANOW, &term);


	fflush(NULL);

	/* Receive a 8bits at UART
	 * 
	 */
	while(1) {
	if ( read(fd, &buf, 1) != 1 ) 
	 {
	    printf("Failed to receive %x\n", buf);
//	    exit(0);
	 }
	else
            printf(" Ox%x ", buf);
	}

	/* Tell everything is done. */
	printf("Complete Test OK.\n");
	close(fd);
	rtems_task_delete(RTEMS_SELF);
}


/*******************************************************************************
 * RTEMS user commands 
 ******************************************************************************/
/* usercommand: spwinfo */
void main_spwinfo(void)
{
  printf("[grspw] Device Information \n");
  grspw_print(OPTION_INFO_ALL);
}

/* usercommand: spwdemo */
int main_spwdemo(int argc, char* argv[])
{
  rtems_status_code    status;
  const char* option = argv[1];
 
  if(strcmp(option,"-h") == 0 )
  {
    printf("This demonstrats SpaceWire link communication \nIt uses grspw0 as TX in Task-1 and grspw1 as RX in Task-2\n");
    printf("connect grspw0 and grspw1\n");
    return 0;
  }
  printf("[grspw] -----SPACEWIRE DEMONSTRATOR--------- \n");
  Task_name[1] = rtems_build_name('T','S','K','1'); 
  Task_name[2] = rtems_build_name('T','S','K','2'); 

  status = rtems_task_create(Task_name[1],122, RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,RTEMS_DEFAULT_ATTRIBUTES, &Task_id[1]);
  printf("[Init]-- Task nb. %d created, status = %d, priority = %d, id = %x\n",1, (int)status, 122,(int)Task_id[1]); 
  status = rtems_task_create(Task_name[2],121, RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,RTEMS_DEFAULT_ATTRIBUTES, &Task_id[2]);  
  printf("[Init]-- Task nb. %d created, status = %d, priority = %d, id = %x\n",2, (int)status, 121,(int)Task_id[2]); 

  rtems_task_start(Task_id[2],The_Task2, 0);
  rtems_task_start(Task_id[1],The_Task1, 0);

  return 0;
}

/* usercommand: spwcnfstats */
int main_spwcnfstats(int argc, char* argv[])
{
  const char* device = argv[1];
  
  if(strcmp(device,"-h") == 0 )
  {
    printf("Print SpaceWire link configuration status \n");
    printf ("Usage: spwcnfstats [-h | /dev/grspwX] \n");
    return 0;
  }
  else if (argc != 2)
  {
    printf ("error: invalid options, use -h for Usage \n");
    return -1;
  }
  
  int fd;
  fd = spw_open(device);
  spw_configuration_status(fd,argv[1]);
  spw_close (fd);
  printf("[%s] device is closed \n", device);
  return 0;
}

/* usercommand: spwcnf */
int main_spwcnf(int argc, char* argv[])
{
  const char* device = argv[1];
  
  if(strcmp(device,"-h") == 0 )
  {
    printf("Configure SpaceWire link \n");
    printf ("Usage: spwcnf  [-h|/dev/grspwX]   [SET_NODE_ADDRESS{0-255}] [SET_PROMISCUOUS{0,1}]\n");
    return 0;
  }
  else if (argc != 4)
  {
    printf ("error: invalid options, use -h for Usage\n");
    return -1;
  }
  int fd;
  fd = spw_open(device);
  argv[2]= atoi(argv[2]);
  argv[3]= atoi(argv[3]);
  spw_configuration(fd, argv[1], argv[2], argv[3]);
  spw_close (fd);
  printf("[%s] device is closed \n", device);
  return 0;
}

/* usercommand: spwrcv*/
void main_spwrcv(int argc, char* argv[])
{
  rtems_status_code    status; 
  if(strcmp(argv[1],"-h") == 0 )
  {
    printf("Receive data in SpaceWire link \n");
    printf ("Usage: spwrcv  [-h|/dev/grspwX]   [SET_NODE_ADDRESS{0-255}]   [PACKET_NOS]   [PACKET_SIZE]\n");
    return 0;
  }
  else if (argc != 5)
  {
    printf ("error: invalid options, use -h for Usage\n");
    return -1;
  }

  Task_name[3] = rtems_build_name('T','S','K','3'); 

  status = rtems_task_create(Task_name[3],122, RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,RTEMS_DEFAULT_ATTRIBUTES, &Task_id[3]);
  printf("[grspw_receiver]-- Task nb. %d created, status = %d, priority = %d, id = %x\n",3, (int)status, 122,(int)Task_id[3]); 

  rtems_task_start(Task_id[3],The_Task3, argv);
  return 0;
}

/*usercommand killtask*/
static void notification(int fd, int seconds_remaining, int id)
{
  printf("Press any key to kill the task TSK%d ID:%x (%is remaining)\n", id, (int)Task_id[id], seconds_remaining );
}

int main_killtask(int argc, char* argv[])
{
  
  if(strcmp(argv[1],"-h") == 0 )
  {
    printf("Kill the existing userdefined TASK: TSK \n");
    printf ("Usage: killtask  [-h | TSK_NUMBER]  \n");
    return 0;
  }
  else if (argc != 2)
  {
    printf ("error: invalid options, use -h for Usage\n");
    return -1;
  }
  int t_id =atoi(argv[1]);

  rtems_status_code    status; 
  status = rtems_shell_wait_for_input(STDIN_FILENO,10,notification,t_id);
  if (status == RTEMS_SUCCESSFUL)
  {
   status = rtems_task_delete(Task_id[t_id]);

   if (status == RTEMS_SUCCESSFUL)
     printf ("Task TSK%d deleted sucessfully \n",t_id);
   else
     printf ("ERROR: Task delete not sucessfull\n");
  }

}

/*usercommand ethserver*/
int main_ethserver(int argc, char* argv[])
{
  rtems_status_code    status;

  if(strcmp(argv[1],"-h") == 0 )
  {
    printf("Create an TCP/IP server link \n");
    printf ("Usage: ethserver  [-h | PORT_NUMBER] [BUFFER_SIZE]\n");
    return 0;
  }
  else if (argc != 3)
  {
    printf ("error: invalid options, use -h for Usage\n");
    return -1;
  }

  printf("[greth_server] -----TCP/IP SERVER --------- \n");
  Task_name[4] = rtems_build_name('T','S','K','4'); 

  status = rtems_task_create(Task_name[4],TASK4_PRIORITY, RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,RTEMS_DEFAULT_ATTRIBUTES, &Task_id[4]);
  printf("[greth_server]-- Task nb. %d created, status = %d, priority = %d, id = %x\n",4, (int)status, TASK4_PRIORITY,(int)Task_id[4]); 

  rtems_task_start(Task_id[4],The_Task4, argv);
  return 0;
}

/*usercommand ethrxtx*/
int main_ethrxtx(int argc, char* argv[])
{
  rtems_status_code    status;  
  if(strcmp(argv[1],"-h") == 0 )
  {
    printf("Receive data and send back in TCP/IP link \n");
    printf ("Usage:  ethrxtx  [-h | SERVER_IPADDRESS] [PORT] [BUFFER_SIZE]\n");
    return 0;
  }
  else if (argc != 4)
  {
    printf ("error: invalid options, use -h for Usage\n");
    return -1;
  }


  printf("[greth_client] -----TCP/IP CLIENT --------- \n");
  Task_name[5] = rtems_build_name('T','S','K','5'); 

  status = rtems_task_create(Task_name[5],TASK5_PRIORITY, RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,RTEMS_DEFAULT_ATTRIBUTES, &Task_id[5]);
  printf("[greth_client]-- Task nb. %d created, status = %d, priority = %d, id = %x\n",5, (int)status, TASK5_PRIORITY,(int)Task_id[5]); 

  rtems_task_start(Task_id[5],The_Task5, argv);
  return 0;

}

/* usercommand: uartdemo */
int main_uartdemo(int argc, char* argv[])
{

  if(strcmp(argv[1],"-h") == 0 )
  {
    printf("task sends characters on UART %s \n", UART1_DEVICE);
    printf ("Usage: uartdemo  [-h ]  \n");
    return 0;
  }
//  else if ((argc > 1) || ((argc== 1)&&(strcmp(argv[1],"-h") != 0)))
//  {
//    printf ("error: invalid options, use -h for Usage\n");
//    return -1;
//  }

  rtems_status_code    status;

  printf("[UART] -----UART DEMONSTRATOR--------- \n");

  Task_name[6] = rtems_build_name('T','S','K','6'); 

  status = rtems_task_create(Task_name[6],TASK6_PRIORITY, RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,RTEMS_DEFAULT_ATTRIBUTES, &Task_id[6]);

  printf("[UART]-- Task nb. %d created, status = %d, priority = %d, id = %x\n",1, (int)status, TASK6_PRIORITY,(int)Task_id[6]); 

  rtems_task_start(Task_id[6],uartdemotask, 0);
  
  return 0;
}

/* usercommand: uartrx */
int main_uartrx(int argc, char* argv[])
{

  if(strcmp(argv[1],"-h") == 0 )
  {
    printf("task receives 8bit data words on UART %s \n", UART1_DEVICE);
    printf ("Usage: uartrx  [-h | BAUDRATE ]  \n");
    return 0;
  }
  else if (argc != 1)
  {
    printf ("error: invalid options, use -h for Usage\n");
    return -1;
  }
 
  rtems_status_code    status;

  printf("[UART] -----UART RECEIVER--------- \n");

  Task_name[7] = rtems_build_name('T','S','K','7'); 

  status = rtems_task_create(Task_name[7],TASK7_PRIORITY, RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,RTEMS_DEFAULT_ATTRIBUTES, &Task_id[7]);

  printf("[UART]-- Task nb. %d created, status = %d, priority = %d, id = %x\n",1, (int)status, TASK7_PRIORITY,(int)Task_id[7]); 

  rtems_task_start(Task_id[7],uartreceiver, argv);
  
  return 0;
}

/*******************************************************************************
 * RTEMS Startup Task
 ******************************************************************************/
rtems_task Init (rtems_task_argument ignored)
{
  rtems_status_code    status; 

  /* Initializing amba root bus and drivers */
  system_init();
 
  /*user defined commands*/
  rtems_shell_add_cmd ("killtask","system","Kill the user defined task",main_killtask);
  rtems_shell_add_cmd ("spwinfo","spacewire","Print SpaceWire device infomation",main_spwinfo);
  rtems_shell_add_cmd ("spwdemo","spacewire","Demostrator: transfer data from GRSPW0 to GRSPW1",main_spwdemo);
  rtems_shell_add_cmd ("spwcnfstats","spacewire","Print SpaceWire link configuration status",main_spwcnfstats);
  rtems_shell_add_cmd ("spwcnf","spacewire","Congigure SpaceWire link",main_spwcnf);
  rtems_shell_add_cmd ("spwrcv","spacewire","Receive data in a SpaceWire link",main_spwrcv);
  rtems_shell_add_cmd ("ethrxtx","ethernet","Receive and send back the IP packet",main_ethrxtx);
  rtems_shell_add_cmd ("ethserver","ethernet","Create TCP/IP server",main_ethserver);
  rtems_shell_add_cmd ("uartdemo","apbuart","print helloworld in apbuart(/dev/console_b) j2 connector of RASTA, uses 8N1 Baudrate: 115200 ",main_uartdemo);
  rtems_shell_add_cmd ("uartrx","apbuart","Receive data words in apbuart(/dev/console_b) j2 connector of RASTA, uses 8N1 Baudrate: 115200",main_uartrx);
  printf("\n");
  /* Initialize shell */
  rtems_shell_init(
    "SHLL",                          /* task_name */
    512*1024,			     /* task_stacksize */
    100,                             /* task_priority */
    "/dev/console_b",                  /* devname */
    false,                           /* forever */
    true,                            /* wait */
    NULL                             /* login rtems_shell_login_check */
  );
  status = rtems_task_delete(RTEMS_SELF);
}


/*******************************************************************************
 * RTEMS Configuration
 ******************************************************************************/
rtems_task Init (rtems_task_argument argument);
#define CONFIGURE_INIT
#include <bsp.h> /* for device driver prototypes */
#include "system.h"
#include "networkconfig.h"



