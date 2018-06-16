/******************************************************************************
*  File name: tcp.h
*
*  Author: Vasco Amaral and Rui Silva
*
*  Release date: 2017/04/07
*
*  Description: Header file for the implementation of all the functions
* 				     needed to communicate through TCP.
*
*  Version: 1.0
*
*  Change log: N/A
*
******************************************************************************/

/* Prevent multiple inclusions                                      */
#ifndef tcpHeader
#define tcpHeader

#include "defs.h"

/******************************************************************************
 *  Data type: TCP_in
 *
 *  Description: Adress and socket to be connected to, by other message server.
 ******************************************************************************/
struct TCP_in {
  int fd;
  struct sockaddr_in addr;
};

/******************************************************************************
 *  Data type: TCP_socket
 *
 *  Description: Socket describing the current connection to a message server.
 ******************************************************************************/
struct TCP_socket {
  int fd;
};

/******************************************************************************
 * Connect_tcp()
 *
 * Arguments: ip, port - Ip and port of the server to connect.
 * Returns: Socket to use in comunication.
 *
 * Description: Connects to another tcp server.
 *****************************************************************************/
int Connect_tcp(int ip, int port);

/******************************************************************************
 * Listen_tcp()
 *
 * Arguments: ip, port - Port to run the tcp server.
 * Returns: Pointer to structure TCP_in with socket and adress.
 *
 * Description: Binds a tcp server with the system in designated port and
 *              listens to incoming connections.
 *****************************************************************************/
struct TCP_in *Listen_tcp(int port);

/******************************************************************************
 * Write_tcp()
 *
 * Arguments: fd - Socket of the server to send a message.
 *            message - Message to be sent.
 * Returns: Integer with info on possible errors.
 *
 * Description: Send a tcp message.
 *****************************************************************************/
int Write_tcp(int fd, char *message);

/******************************************************************************
 * Read_tcp()
 *
 * Arguments: fd - Socket of the server sending a message.
 *            flag - Flag indicating if it is a big message or just a line.
 * Returns: Message read.
 *
 * Description: Receives a tcp message.
 *****************************************************************************/
char *Read_tcp(int fd, int flag);

/******************************************************************************
 * Requests_Msgs()
 *
 * Arguments: fd - Socket of the server to send the request.
 * Returns: Integer with info on possible errors.
 *
 * Description: Send a message with the protocol to request all messages.
 *****************************************************************************/
int Request_Msgs(int fd);


/* End of: Protect multiple inclusions                              */
#endif
