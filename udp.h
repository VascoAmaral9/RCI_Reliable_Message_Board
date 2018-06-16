/******************************************************************************
*  File name: udp.h
*
*  Author: Vasco Amaral and Rui Silva
*
*  Release date: 2017/04/07
*
*  Description: Header file for the implementation of all the functions
* 				     needed to communicate through UDP.
*
*  Version: 1.0
*
*  Change log: N/A
*
******************************************************************************/

/* Prevent multiple inclusions                                      */
#ifndef udpHeader
#define udpHeader

#include "defs.h"

/******************************************************************************
 *  Data type: UDP_in
 *
 *  Description: adress and socket to communicate with a udp server.
 ******************************************************************************/
struct UDP_in {
  int fd;
  struct sockaddr_in addr;
};


/******************************************************************************
 * UDP_SET()
 *
 * Arguments: ip, port - Ip and port of the server to define the adress.
 *            flag - Indicates if it is for server or for client use.
 * Returns: Pointer to structure UDP_in with socket and adress.
 *
 * Description: Defines the UDP adress and socket either for a client or a
 *              server to use, in this case it uses bind to register the server
 *              address and port with the system, too.
 *****************************************************************************/
struct UDP_in *UDP_SET(int ip, int port, int flag);

/******************************************************************************
* Receive_udp()
*
* Arguments: U - Adress and socket to communicate with the client/server sending the message.
*            flag - Indicates if it's receiving a big list, or just a line.
* Returns: buffer - string received from the identitys server.
*
* Description: Receives a udp message and returns it.
*****************************************************************************/
char *Receive_udp(struct UDP_in *U, int flag);

/******************************************************************************
* Send_udp()
*
* Arguments: U - Adress and socket to communicate with the server to send the message.
*            message - Message to be sent.
* Returns: void.
*
* Description: Sends a udp message to the server specified.
*****************************************************************************/
void Send_udp(struct UDP_in *U, char *message);

/******************************************************************************
 * REG_IS()
 *
 * Arguments: name, ip, upt, tpt - parameters describing our message server.
 *            IS - Adress and socket to communicate with the identitys server.
 * Returns: void.
 *
 * Description: Sends a udp message registering the message server in the identitys server.
 *****************************************************************************/
void REG_IS(char *name, char *ip, int upt, int tpt, struct UDP_in *IS);

/******************************************************************************
 * RequestSV_IS()
 *
 * Arguments: IS - Adress and socket to communicate with the identitys server.
 * Returns: void.
 *
 * Description: Sends a udp message requesting a list of servers already registered.
 *****************************************************************************/
void RequestSV_IS(struct UDP_in *IS);

/******************************************************************************
 * PublishSV_MS()
 *
 * Arguments: udp_sv - Adress and socket to communicate with the identitys server.
 *            message - string that holding message to be transmited.
 * Returns: void.
 *
 * Description: Sends a udp message publishing the string message in the MS.
 *****************************************************************************/
void PublishSV_MS(struct UDP_in *udp_sv, char * message);

/******************************************************************************
 * RequestM_MS()
 *
 * Arguments: udp_sv - Adress and socket to communicate with the identitys server.
 *            n - number of messages to resquest the messages server.

 * Returns: void.
 *
 * Description: Sends a udp message requesting the list of messages.
 *****************************************************************************/
void RequestM_MS(struct UDP_in *udp_sv, int n);

/* End of: Protect multiple inclusions                              */
#endif
