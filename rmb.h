/******************************************************************************
*  File name: rmb.h
*
*  Author: Vasco Amaral and Rui Silva
*
*  Release date: 2017/04/07
*
*  Description: Header file for the implementation of important functions in
* 				     the rmb application.
*
*  Version: 1.0
*
*  Change log: N/A
*
******************************************************************************/

/* Prevent multiple inclusions                                      */
#ifndef rmbHeader
#define rmbHeader

#include "core.h"


/******************************************************************************
* ReadArgumentsRMB()
*
* Arguments: argc - number of arguments read from the terminal.
*            argv - string that holds the program arguments.
* Returns: R - data type with the sockets, ips and ports to rmb's udp
               communication.
*
* Description: Process the arguments and initilizes the rmb data type.
*****************************************************************************/
struct RMB_Strct *ReadArgumentsRMB(int argc, char **argv);

/******************************************************************************
* Start_RMB()
*
* Arguments: R - rmb data type
*
* Returns: void
*
* Description: set the udp socket with identitys server to choose a message
               server to communicate with and set that specific socket.
*****************************************************************************/
void Start_RMB(struct RMB_Strct *R);

/******************************************************************************
* RunRMB()
*
* Arguments: R - rmb data type
*
* Returns: void
*
* Description: Calls the main functions to run the rmb application on an endless loop.
*****************************************************************************/
void RunRMB(struct RMB_Strct *R);

/******************************************************************************
* GetSV_MS()
*
* Arguments: R - rmb data type
             flag - Indicates if it is to connect with a server or to print them all.
*
* Returns: void
*
* Description: do the request "GET_SERVERS" to the identitys server and prints them
               or just do this request to connect with one message server.
*****************************************************************************/
void GetSV_MS(struct RMB_Strct *R, int flag);

/******************************************************************************
* Get_Messages()
*
* Arguments: R - rmb data type
*
* Returns: void
*
* Description: do the request "GET_MESSAGES" to the messages server connected with
               and print the messages on the terminal.
*****************************************************************************/
void Get_Messages(struct RMB_Strct *R, int n);


/* End of: Protect multiple inclusions                              */
#endif
