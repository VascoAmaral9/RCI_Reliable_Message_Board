/******************************************************************************
 *  File name: tcp.c
 *
 *  Author: Vasco Amaral and Rui Silva
 *
 *  Release date: 2017/04/07
 *
 *  Description: C-Implementation of all the functions needed to communicate
 *              through TCP.
 *
 *  Version: 1.0
 *
 *  Change log: N/A
 *
 ******************************************************************************/

#include "tcp.h"

int Connect_tcp(int ip, int port){
  int fd, n;
  struct sockaddr_in addr;

  /*Defines the tcp socket*/
  fd = socket(AF_INET,SOCK_STREAM,0);
  if(fd == -1)  Shutdown(SOCKET); /*Error*/

  /*Defines the server address*/
  memset((void*)&addr,(int)'\0',sizeof(addr)); /*Makes sure addr is empty*/
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(ip);
  addr.sin_port = htons(port);

  /*Connects to the server*/
  n = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
  if(n == -1){
    Shutdown(TCP_CONNECT); /*Error*/
    return INDEFINED;
  }

  return fd;
}

struct TCP_in *Listen_tcp(int port){
  struct TCP_in *T = NULL;
  int ret;

  T = (struct TCP_in *) malloc(sizeof(struct TCP_in));
  if(T == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/
  memset((void*)T,(int)'\0',sizeof(T)); /*Makes sure struct is empty*/

  /*Defines the tcp socket*/
  T->fd = socket(AF_INET,SOCK_STREAM,0);
  if(T->fd == -1)  Shutdown(SOCKET); /*Error*/

  /*Defines the server address*/
  T->addr.sin_family = AF_INET;
  T->addr.sin_addr.s_addr = htonl(INADDR_ANY);
  T->addr.sin_port = htons(port);

  ret = bind(T->fd,(struct sockaddr*)&T->addr,sizeof(T->addr));
  if(ret == -1)  Shutdown(BIND); /*Error*/

  ret = listen(T->fd,BACKLOG);
  if(ret == -1)  Shutdown(TCP_LISTEN); /*Error*/

  return T;
}

int Write_tcp(int fd, char *message){
  int nleft, nwritten;
  extern int errno;

  nleft = strlen(message);

  while(nleft > 0){
    nwritten = write(fd, message, nleft);
    if(nwritten == -1){
      if(errno == EPIPE){
        printf("Connection closed by peer\n");
        return CLOSED;
      }
      Shutdown(TCP_WRITE); /*Error*/
    }
    nleft -= nwritten;
    message += nwritten;
  }
  return OK;
}

char *Read_tcp(int fd, int flag){
  int nread = 1;
  char *buffer = NULL;

  if(flag == LIST){
    buffer = (char *) malloc(MAX_STR_SIZE*sizeof(char));
    if(buffer == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/
    memset((void*)buffer,(int)'\0',MAX_STR_SIZE); /*Makes sure string is empty*/
  }

  if(flag == SINGLE){
    buffer = (char *) malloc(STR_SIZE*sizeof(char));
    if(buffer == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/
    memset((void*)buffer,(int)'\0',STR_SIZE); /*Makes sure string is empty*/
  }

  nread = read(fd, buffer, MAX_STR_SIZE);
  if(nread == -1) Shutdown(TCP_READ); /*Error*/

  return buffer;
}

int Request_Msgs(int fd){
  char message[STR_SIZE];
  int ret;

  memset(message,'\0',STR_SIZE); /*Emptys string 'message' */

  /*Writes the message to send*/
  if(sprintf(message, "SGET_MESSAGES\n") < 0) Shutdown(SYSTEM); /*Error*/

  ret = Write_tcp(fd, message);

  return ret;
}
