

#ifndef __TCPIP_H__
#define __TCPIP_H__


#define MAXHOSTNAME 80

/******************************************************************************/

int Configure_Socket_Client(char *server_name, int port);

void socket_close(int sd);

void rasta_tcpip_receive(uint8_t *videoFrame, int width, int height, int sd);

void rasta_tcpip_sendback(char *buffer, int bpl, int height, int sd);


int configure_socket_client_linux(char *server_name, int port);

void socket_close_linux(int sd);

void linux_tcpip_receive(uint8_t *videoFrame, int width, int height, int sd);

void linux_tcpip_sendback(char *buffer, int bpl, int height, int sd);


int eth_receive(char *data, int len, int sd);
void eth_transmit(char *data, int len, int sd);
void eth_server_tcpip(int portno, int buffersize);
/******************************************************************************/
#endif /* __TCPIP_H__ */


