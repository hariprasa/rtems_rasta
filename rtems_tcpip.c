

/* Standard C headers */
#include <stdio.h>
#include <stdlib.h>     // atoi()
#include <string.h>     // bcopy()
#include <errno.h>      // errno

/* Sockets */
#include <unistd.h>     // gethostname()
#include <netdb.h>      // gethostbyname()
#include <arpa/inet.h>  // inet_ntop(), inet_addr()
#include <sys/socket.h> // AF_INET, SOCK_STREAM, MSG_WAITALL
#include <netinet/in.h> // struct sockaddr_in

#include "tcpip.h"

/*****************************************************************************/

int Configure_Socket_Client(char *server_name, int port)
{
  int sd;
  int len;
  int status;
  struct sockaddr_in server;
  struct hostent *hp;
  struct sockaddr_in addr;
  char ThisHost[MAXHOSTNAME];
  char str[INET_ADDRSTRLEN];

  /* Obtain information of the current machine (client) */
  gethostname(ThisHost, MAXHOSTNAME); // get name of current machine

  printf("Information of the TCP connection \n");
  printf("Client running on: %s \n", ThisHost);

  /* Server IP address and port number which would be connected to */
  hp = gethostbyname(server_name);
  if (hp == NULL)
  {
    //inet_pton(AF_INET, server_name, &(addr.sin_addr.s_addr));
    addr.sin_addr.s_addr = inet_addr(server_name);
    if ((hp = gethostbyaddr((char*)&addr.sin_addr.s_addr,
			    sizeof(addr.sin_addr.s_addr),AF_INET)) == NULL)
    {
      fprintf(stderr, "Can not find the host: %s \n", server_name);
      exit(EXIT_FAILURE);
    }
  }
  bcopy(hp->h_addr, &(server.sin_addr), hp->h_length);
  inet_ntop(AF_INET, &(server.sin_addr), str, INET_ADDRSTRLEN);
  printf("Server IP Address: %s \n", str);
  printf("Server port number: %d \n", port);

  /* Fill in the structure with customer data */
  server.sin_family = hp->h_addrtype; // or: server.sin_family = AF_INET;
  server.sin_port = htons(port);

  /* Create the socket */
  //sd = socket(AF_INET, SOCK_STREAM, 0);
  sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sd < 0)
  {
    perror("CLIENT Cannot create the socket");
    exit(EXIT_FAILURE);
  }

  /* Connect to server */
  len = sizeof(struct sockaddr_in);
  status = connect(sd, (struct sockaddr *)&server, len);
  if (status < 0)
  {
    perror("CLIENT Cannot connect to the server");
    return -1;
  }
  return sd;
}




void socket_close(int sd)
{
  close(sd);
}


/* TCP/IP channel RECEIVE */
int eth_receive(char *data, int len, int sd)
{
  int rc;
  rc = recv(sd,data,len,0);
  if (rc < 0) 
  {
     perror(" ERROR reading from socket");
     exit(1);
  }
  else
    ;// printf("Number of characters/bytes received: %d \n", rc);
    // the size of a char variable is 1 byte
  return rc;
}

/* TCP/IP channel TRANSMIT */
void eth_transmit(char *data, int len, int sd)
{
  int sent;

  sent = send(sd, data, len, 0);
  if (sent < 0) {
     perror("ERROR writing to socket");
     exit(1);
  }
  //else
    // printf("Number of characters sent: %d \n",sent);
    // the size of a char variable is 1 byte
}

/* TCP/IP channel RECEIVE */
void rasta_tcpip_receive(uint8_t *videoFrame, int width, int height, int sd)
{
  int rc;

  rc = recv(sd, videoFrame, width*height*12/8, MSG_WAITALL);
  printf("Number of characters/bytes received: %d \n", rc);
  // the size of a char variable is 1 byte
}


/* TCP/IP channel SEND BACK */
void rasta_tcpip_sendback(char *buffer, int bpl, int height, int sd)
{
  int sent, size;

  size = bpl * height; // 1228800

  sent = send(sd, buffer, size, 0);
  //sent = send(sd, image_ctx.xImage->data, size, 0);
  printf("Number of characters sent: %d \n",sent);

  if (buffer != NULL)
    free (buffer);
}




