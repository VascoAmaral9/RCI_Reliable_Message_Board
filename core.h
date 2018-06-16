/******************************************************************************
*  File name: core.h
*
*  Author: Vasco Amaral and Rui Silva
*
*  Release date: 2017/04/07
*
*  Description: Header file for the implementation of general functions
* 				     needed to run both applications.
*
*  Version: 1.0
*
*  Change log: N/A
*
******************************************************************************/

/* Prevent multiple inclusions                                      */
#ifndef coreHeader
#define coreHeader

#include "udp.h"
#include "tcp.h"
#include "LinkedList.h"

/******************************************************************************
 *  Data type: Message
 *
 *  Description: Message and it's logic time.
 ******************************************************************************/
struct Message {
  int LC;
  char *message;
};

/******************************************************************************
 *  Data type: MS_Strct
 *
 *  Description: Main structure to run the Message Server containing all the info.
 ******************************************************************************/
struct MS_Strct {
  char name[STR_SIZE];
  char ip[STR_SIZE];
  int upt;
  int tpt;
  int siip;
  int sipt;
  int m;
  int r;
  int m_act;  /*Number of messages stored*/
  int LC; /*Logic clock*/
  struct TCP_in *tcp_sv; /*TCP server for incoming connections*/
  struct UDP_in *udp_sv; /*UDP server for incoming messages*/
  struct UDP_in *IS;  /*Socket and adress of the identitys server*/
  LinkedList *MSG; /*List of messages*/
  LinkedList *Servers; /*List of connected servers*/
};

/******************************************************************************
 *  Data type: RMB_Strct
 *
 *  Description: Main structure to run the Message Server containing all the info.
 ******************************************************************************/
struct RMB_Strct{
  int siip;
  int sipt;

  int upt; /*udp message server port*/
  int ip; /*ip from the message server host*/

  struct UDP_in *udp_sv; /*message server socket*/
  struct UDP_in *IS; /*identitys server socket*/
};

/******************************************************************************
 * GetIP()
 *
 * Arguments: name - Server name or ip in string formation.
 *            flag - Indicates if name is the server name or ip in string formation.
 * Returns: ip - Ip address in host byte order.
 *
 * Description: Receives the server name/ip and returns it's ip in host byte order.
 *****************************************************************************/
int GetIP(char *name, int flag);

/******************************************************************************
 * Print_MSGS()
 *
 * Arguments: MS - Main structure of the application msgserv.
 * Returns: void.
 *
 * Description: Prints all the messages stored in the terminal.
 *****************************************************************************/
void Print_MSGS(struct MS_Strct *MS);

/******************************************************************************
 * Print_rec()
 *
 * Arguments: MS - Main structure of the application msgserv.
 *            LL - Pointer to LinkedList in the current position.
 * Returns: void.
 *
 * Description: Auxiliar recursive function to print from the end to the begining.
 *****************************************************************************/
void Print_rec(struct MS_Strct *MS, LinkedList *LL);

/******************************************************************************
 * GetSV_IS()
 *
 * Arguments: MS - Main structure containing (among others) the address and
 *                 socket to communicate with the identitys server.
 *            flag - Indicates if it is to print all servers or to connect.
 * Returns: void.
 *
 * Description: Gets the message servers registered in the IS and either prints
 *              them or stablishes a TCP connection.
 *****************************************************************************/
void GetSV_IS(struct MS_Strct *MS, int flag);

/******************************************************************************
 * NewConnection()
 *
 * Arguments: MS - Main structure containing (among others) the address and
 *                 socket to communicate with the identitys server.
 *            ip - Ip address in host byte order.
 *			  port - Port well know of server that accept TCP connects
 *			  flag - Indicates if it is to print all servers or to connect.
 *			  first - Indicates if it is the first connection
 * Returns: void.
 *
 * Description: Do all the TCP process for connection. In case of first connection
 * 				the MS request the messages form another messeges server.
 *****************************************************************************/
void NewConnection(struct MS_Strct *MS, int ip, int port, int flag, int first);

/******************************************************************************
 * BroadCast_MSG()
 *
 * Arguments: MS - Main structure containing (among others) the address and
 *                 socket to communicate with the identitys server.
 *            MSG - Main structure containing the message and his logical time
 *			  
 * Returns: void.
 *
 * Description: Do all the TCP process for connection. In case of first connection
 * 				the MS request the messages form another messeges server.
 *****************************************************************************/
void BroadCast_MSG(struct MS_Strct *MS, struct Message *MSG);

/******************************************************************************
 * SGET_MSGS()
 *
 * Arguments: MS - Main structure containing (among others) the address and
 *                 socket to communicate with the identitys server.
 *            fd - Socket to communicate.
 *			  
 * Returns: int - Flag with statment process fail or ok
 *
 * Description: Request and saves all the messages in the network
 *****************************************************************************/
int SGET_MSGS(struct MS_Strct *MS, int fd);

/******************************************************************************
 * Publish()
 *
 * Arguments: MS - Main structure containing (among others) the address and
 *                 socket to communicate with the identitys server.
 *            message - Main structure containing the message and his logical time
 *            LC - Logical 
 *			  
 * Returns: void.
 *
 * Description: Publish a message in the network
 *****************************************************************************/
void Publish(struct MS_Strct *MS, char *message, int LC);

/* End of: Protect multiple inclusions                              */
#endif
