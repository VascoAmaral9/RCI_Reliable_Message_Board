/******************************************************************************
*  File name: msgserv.h
*
*  Author: Vasco Amaral and Rui Silva
*
*  Release date: 2017/04/07
*
*  Description: Header file for the implementation of important functions in
* 				     the msgserv application.
*
*  Version: 1.0
*
*  Change log: N/A
*
******************************************************************************/

/* Prevent multiple inclusions                                      */
#ifndef msgservHeader
#define msgservHeader

#include "core.h"


struct MS_Strct *ReadArgumentsMS(int argc, char **argv);
void Start_MS(struct MS_Strct *MS);
void ReadTerm(struct MS_Strct *MS);
void ReadRMB(struct MS_Strct *MS);
void ReadOMS(struct MS_Strct *MS, int fd);
void Send_MSGS_RMB(struct MS_Strct *MS, int n_msgs);
char *Send_MSGS_RMB_rec(struct MS_Strct *MS, LinkedList *LL, int *n, int n_msgs, char *ptr);
void Send_MSGS_OMS(struct MS_Strct *MS, int fd);
char *Send_MSGS_OMS_rec(struct MS_Strct *MS, LinkedList *LL, char *ptr, char *buffer);
void Run_MS(struct MS_Strct *MS);


/* End of: Protect multiple inclusions                              */
#endif
