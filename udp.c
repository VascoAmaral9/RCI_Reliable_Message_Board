/******************************************************************************
 *  File name: udp.c
 *
 *  Author: Vasco Amaral and Rui Silva
 *
 *  Release date: 2017/04/07
 *
 *  Description: C-Implementation of all the functions needed to communicate
 *              through UDP.
 *
 *  Version: 1.0
 *
 *  Change log: N/A
 *
 ******************************************************************************/

#include "udp.h"

struct UDP_in *UDP_SET(int ip, int port, int flag){
  struct UDP_in *U = NULL;
  int ret;

  U = (struct UDP_in *) malloc(sizeof(struct UDP_in));
  if(U == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/
  memset((void*)U,(int)'\0',sizeof(U)); /*Makes sure struct is empty*/

  /*Defines the udp socket*/
  U->fd = socket(AF_INET,SOCK_DGRAM,0);
  if(U->fd == -1)  Shutdown(SOCKET); /*Error*/

  /*Defines the server address*/
  U->addr.sin_family = AF_INET;
  U->addr.sin_port = htons(port);
  /*Sets the address with the server ip, for the client to use*/
  if(flag == CLIENT)
    U->addr.sin_addr.s_addr = htonl(ip);

  /*Sets the address for a server to use and registers it with the system (bind)*/
  else if(flag == SERVER){
    U->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = bind(U->fd,(struct sockaddr*)&U->addr,sizeof(U->addr));
    if(ret == -1)  Shutdown(BIND); /*Error*/
  }

  return U;
}

char *Receive_udp(struct UDP_in *U, int flag){
  int n;
  char *buffer = NULL;
  socklen_t addrlen;
  struct timeval tv;

  tv.tv_sec = TIMEOUT; /*Sets the timer to wait for udp messages*/
  tv.tv_usec = 0;

  addrlen = sizeof(U->addr);

  if(flag == LIST){
    buffer = (char *) malloc(MAX_STR_SIZE*sizeof(char));
    if(buffer == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/
    memset((void*)buffer,(int)'\0',MAX_STR_SIZE); /*Makes sure string is empty*/

    if (setsockopt(U->fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof tv))
        Shutdown(TIMER); /*Error*/

    n = recvfrom(U->fd,buffer,MAX_STR_SIZE,0,(struct sockaddr*)&U->addr,&addrlen);
    if(n == -1) Shutdown(UDP_RECEIVE); /*Error*/
  }

  if(flag == SINGLE){
    buffer = (char *) malloc(STR_SIZE*sizeof(char));
    if(buffer == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/
    memset((void*)buffer,(int)'\0',STR_SIZE); /*Makes sure string is empty*/

    if (setsockopt(U->fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof tv))
        Shutdown(TIMER); /*Error*/

    n = recvfrom(U->fd,buffer,STR_SIZE,0,(struct sockaddr*)&U->addr,&addrlen);
    if(n == -1) Shutdown(UDP_RECEIVE); /*Error*/
  }

  return buffer;
}

void Send_udp(struct UDP_in *U, char *message){
  int n;

  n = sendto(U->fd,message,strlen(message),0,(struct sockaddr*)&U->addr,sizeof(U->addr));
  if(n == -1) Shutdown(UDP_SEND); /*Error*/
}

void REG_IS(char *name, char *ip, int upt, int tpt, struct UDP_in *IS){
  char message[STR_SIZE];

  memset(message,'\0',STR_SIZE); /*Emptys string 'message' */

  /*Agroups all the information to send in a string*/
  if(sprintf(message, "REG %s;%s;%d;%d", name, ip, upt, tpt) < 0) Shutdown(SYSTEM); /*Error*/

  Send_udp(IS, message);
}

void RequestSV_IS(struct UDP_in *IS){
  char message[STR_SIZE];

  memset(message,'\0',STR_SIZE); /*Emptys string 'message' */

  /*Writes the message to send*/
  if(sprintf(message, "GET_SERVERS") < 0) Shutdown(SYSTEM); /*Error*/

  Send_udp(IS, message);
}

void PublishSV_MS(struct UDP_in *udp_sv, char * message){
  char buffer[STR_SIZE];
  memset(buffer, '\0', STR_SIZE);

  /*Writes the message to send*/
  if(sprintf(buffer, "PUBLISH %s", message) < 0) Shutdown(SYSTEM); /*Error*/

  Send_udp(udp_sv, buffer);
}

void RequestM_MS(struct UDP_in *udp_sv, int n){
  char message[STR_SIZE];
  memset(message, '\0', STR_SIZE);

  /*Writes the message to send*/
  if(sprintf(message, "GET_MESSAGES %d", n) < 0) Shutdown(SYSTEM); /*Error*/

  Send_udp(udp_sv, message);
}
