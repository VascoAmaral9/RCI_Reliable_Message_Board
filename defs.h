/******************************************************************************
*  File name: defs.h
*
*  Author: Vasco Amaral and Rui Silva
*
*  Release date: 2017/04/07
*
*  Description: Header file for the general definitions for the project.
*
*  Version: 1.0
*
*  Change log: N/A
*
******************************************************************************/

/* Prevent multiple inclusions                                      */
#ifndef defsHeader
#define defsHeader

/*Inclusion of all the libraries used in the entire project*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>

#define max(A,B) ((A)>=(B) ? (A):(B))

#define MAX_STR_SIZE 64000
#define STR_SIZE 256
#define INDEFINED 0
#define RMB 0
#define FIRST 1
#define CONNECT 1
#define PRINT 2
#define NAME 1
#define NUMBER 2
#define CLIENT 1
#define SERVER 2
#define LIST 1
#define SINGLE 2
#define BACKLOG 10
#define OUT 1
#define IN 2
#define CLOSED 2
#define TIMEOUT 5

/*Error types*/
#define OK 0 /*Exits successfully*/
#define FAILED 1
#define WRONG_ARGUMENTS 2
#define WRONG_NUMBER_ARGUMENTS 3
#define SOCKET 4
#define BIND 5
#define MEMORY 6
#define UDP_SEND 7
#define UDP_RECEIVE 8
#define TCP_CONNECT 9
#define TCP_WRITE 10
#define TCP_READ 11
#define TCP_LISTEN 12
#define TCP_ACCEPT 13
#define SIGPIPE_ERR 14
#define SELECT 15
#define PROTOCOL 16
#define SYSTEM 17
#define TIMER 18

/*
 *  Data type: Item
 *
 *  Description: Pointer to void.
 */
typedef void * Item;

/******************************************************************************
 * Shutdown()
 *
 * Arguments: type - Reason to shutdown the application.
 * Returns: void.
 *
 * Description: Receives an integer meaning the reason to shutdown the
 *              application, prints it and exits.
 *****************************************************************************/
void Shutdown(int type);


/* End of: Protect multiple inclusions                              */
#endif
