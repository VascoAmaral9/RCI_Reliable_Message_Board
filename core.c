/******************************************************************************
 *  File name: core.c
 *
 *  Author: Vasco Amaral and Rui Silva
 *
 *  Release date: 2017/04/07
 *
 *  Description: C-Implementation of general functions needed to run
 *                both aplications.
 *
 *  Version: 1.0
 *
 *  Change log: N/A
 *
 ******************************************************************************/

#include "core.h"

void Shutdown(int type){
  /*Choses what to do based on the flag "type" received.*/
  switch(type){
    case OK :
      printf("Application closed successfully\n");
      exit(0);
    case WRONG_ARGUMENTS :
      printf("Invalid arguments to run the application, please try again\n");
      break;
    case WRONG_NUMBER_ARGUMENTS :
      printf("Invalid number of arguments to run the application, please try again\n");
      break;
    case SOCKET :
      printf("Someting went wrong defining a socket\n");
      break;
    case BIND :
      printf("Someting went wrong binding a server\n");
      break;
    case MEMORY :
      printf("Error allocating memory\n");
      break;
    case UDP_SEND :
      printf("Error sending a message through udp\n");
      break;
    case UDP_RECEIVE :
      printf("Error receiving a message through udp\n");
      break;
    case TCP_CONNECT :
      printf("Error connecting to server through tcp\n");
      printf("Ignoring server...\n");
      return;
      break;
    case TCP_WRITE :
      printf("Error writing a message through tcp\n");
      break;
    case TCP_READ :
      printf("Error reading a message through tcp\n");
      break;
    case TCP_LISTEN :
      printf("Someting went wrong listening to tcp connections\n");
      break;
    case TCP_ACCEPT :
      printf("Someting went wrong accepting a tcp connection\n");
      printf("Ignoring server...\n");
      return;
      break;
    case SIGPIPE_ERR :
      printf("Someting went wrong protecting against SIGPIPE\n");
      break;
    case SELECT :
      printf("Someting went wrong in the select function\n");
      break;
    case PROTOCOL :
      printf("Message received doesn't follow protocol\n");
      break;
    case SYSTEM :
      printf("Error in a system call\n");
      break;
    case TIMER :
      printf("Error defining the timer\n");
      break;
    default :
      printf("Unknow error\n");
   }

   printf("Application closed\n");
   exit(type);
}

int GetIP(char *name, int flag){
  int ip;
  struct hostent *h = NULL;
  struct in_addr *a = NULL;

  if(flag == NAME){
    h = gethostbyname(name);
    a = (struct in_addr*)h->h_addr_list[0];
    ip = ntohl(a->s_addr);
  }

  else if(flag == NUMBER){
    a = (struct in_addr *) malloc(sizeof(struct in_addr));
    if(a == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/
    inet_aton(name, a);
    ip = ntohl(a->s_addr);
    free(a);
  }

  return ip;
}

void Print_MSGS(struct MS_Strct *MS){
  LinkedList *LL = MS->MSG;

  Print_rec(MS, LL);
}

void Print_rec(struct MS_Strct *MS, LinkedList *LL){
  struct Message *MSG = NULL;
  LinkedList *aux = NULL;

  if(LL != NULL){
    aux = getNextNodeLinkedList(LL);
    Print_rec(MS, aux);
    MSG = getItemLinkedList(LL);
    printf("%s\n", MSG->message);
  }
}

void GetSV_IS(struct MS_Strct *MS, int flag){
  char *buffer = NULL; /*Buffer to store information received from the IS*/
  char c, *aux = NULL; /*Auxiliar variables*/
  int first = FIRST; /*Activates the flag first*/
  /*Temporary variables with the parameters of each message server*/
  int upt, tpt;
  char name[STR_SIZE], ip[STR_SIZE];

  memset(name,'\0',STR_SIZE); /*Emptys string 'name' */

  /*Requests the list of message servers*/
  RequestSV_IS(MS->IS);
  /*Receives the list of message servers*/
  buffer = Receive_udp(MS->IS, LIST);
  aux = buffer;

  /*Makes sure the predefined formation is correct*/
  if(sscanf(buffer, "%s", name) == EOF) Shutdown(SYSTEM); /*Error*/
  if(strcmp(name, "SERVERS") != 0) Shutdown(PROTOCOL); /*Error*/
  /*Eliminates the first line from the buffer*/
  strcpy(buffer, strchr(buffer, '\n')+1); /*We use the '+1' so it doesn't store '\n' */

  /*Makes sure there is something to read*/
  while(sscanf(buffer, "%c", &c) == 1){
    memset(name,'\0',STR_SIZE); /*Emptys string 'name' */
    memset(ip,'\0',STR_SIZE); /*Emptys string 'ip' */

    /*Gets the name of the other message servers*/
    strncpy(name, buffer, strcspn(buffer, ";"));
    /*Eliminates the name, already stored, from the buffer*/
    strcpy(buffer, strchr(buffer, ';')+1); /*We use the '+1' so it doesn't store ';' */

    /*Gets the ip of the other message servers*/
    strncpy(ip, buffer, strcspn(buffer, ";"));
    /*Eliminates the ip, already stored, from the buffer*/
    strcpy(buffer, strchr(buffer, ';')+1); /*We use the '+1' so it doesn't store ';' */

    /*Scans and stores the variables describing the other message servers*/
    if(sscanf(buffer, "%d;%d\n", &upt, &tpt) == EOF) Shutdown(SYSTEM); /*Error*/
    /*Eliminates the entire line, already stored, from the buffer*/
    strcpy(buffer, strchr(buffer, '\n')+1); /*We use the '+1' so it doesn't store '\n' */

    if(strcmp(name, MS->name) != 0 && flag == CONNECT){
      printf("%s;%s;%d;%d\n", name, ip, upt, tpt);
      NewConnection(MS, GetIP(ip, NUMBER), tpt, OUT, first);
      if(MS->Servers != NULL)
        first++; /*Flag first deactivated*/
    }
    else if(strcmp(name, MS->name) != 0 && flag == PRINT)
      printf("%s;%s;%d;%d\n", name, ip, upt, tpt);
  }

  free(aux); /*frees the memory ocupied by the initial string, received from the IS. */
}

void NewConnection(struct MS_Strct *MS, int ip, int port, int flag, int first){
  struct TCP_socket *T = NULL;
  socklen_t addrlen;

  T = (struct TCP_socket *) malloc(sizeof(struct TCP_socket));
  if(T == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/

  printf("Stablishing new connection...\n");

  if(flag == OUT){
    T->fd = Connect_tcp(ip, port);
    if(T->fd == INDEFINED){
      free(T);
      return;
    }
    if(first == FIRST) /*Gets all the messages if this is the first connection*/
      if(SGET_MSGS(MS, T->fd) == FAILED){
        printf("Failed to get messages\n");
        free(T);
        return;
      }
  }

  else if(flag == IN){
    addrlen = sizeof(MS->tcp_sv->addr);
    T->fd = accept(MS->tcp_sv->fd,(struct sockaddr*)&MS->tcp_sv->addr,&addrlen);
    if(T->fd == -1){
      Shutdown(TCP_ACCEPT); /*Error*/
      free(T);
      return;
    }
  }

  MS->Servers = insertUnsortedLinkedList(MS->Servers, T);
}

void BroadCast_MSG(struct MS_Strct *MS, struct Message *MSG){
  char buffer[STR_SIZE];
  char *ptr = &buffer[10]; /*Pointer to position to write in buffer*/
  LinkedList *LL = MS->Servers, *aux = NULL;
  struct TCP_socket *T = NULL;

  memset(buffer,'\0',STR_SIZE); /*Makes sure string is empty*/

  strcpy(buffer, "SMESSAGES\n");
  sprintf(ptr, "%d;%s\n", MSG->LC, MSG->message);

  while(LL != NULL){
    T = getItemLinkedList(LL);
    if(Write_tcp(T->fd, buffer) == CLOSED){ /*Checks if connection as been closed*/
      if(aux == NULL) /*First connection in list case*/
        MS->Servers = LL->next;
      else /*Another connection in the middle of the list case*/
        aux->next = LL->next;
      free(T);
      free(LL);
    }
    aux = LL;
    LL = getNextNodeLinkedList(LL);
  }
}

int SGET_MSGS(struct MS_Strct *MS, int fd){
  char *buffer = NULL; /*Buffer to store the information received*/
  char *aux = NULL, c; /*Auxiliar variables*/
  char message[STR_SIZE];
  int LC;

  printf("Getting all messages already in the network...\n");

  /*Requests the list of messages*/
  if(Request_Msgs(fd) == CLOSED)
    return FAILED;
  /*Receives the list of messages*/
  buffer = Read_tcp(fd, LIST);
  aux = buffer;

  memset(message,'\0',STR_SIZE); /*Emptys string 'message' */

  /*Makes sure the predefined formation is correct*/
  sscanf(buffer, "%s", message);
  if(strcmp(message, "SMESSAGES") != 0) Shutdown(PROTOCOL); /*Error*/
  /*Eliminates the first line from the buffer*/
  strcpy(buffer, strchr(buffer, '\n')+1); /*We use the '+1' so it doesn't store '\n' */

  /*Makes sure there is something to read*/
  while(sscanf(buffer, "%c", &c) == 1){
    if(c == '\n')
      break; /*End of protocol message*/
    memset(message,'\0',STR_SIZE); /*Emptys string 'message' */

    /*Gets the LC of the message*/
    sscanf(buffer, "%d;", &LC);
    /*Eliminates the LC, already stored, from the buffer*/
    strcpy(buffer, strchr(buffer, ';')+1); /*We use the '+1' so it doesn't store ';' */

    /*Gets the message itself*/
    strncpy(message, buffer, strcspn(buffer, "\n"));
    /*Eliminates the message, already stored, from the buffer*/
    strcpy(buffer, strchr(buffer, '\n')+1); /*We use the '+1' so it doesn't store '\n' */

    Publish(MS, message, LC); /*Publishes message in this server*/
    sscanf(buffer, "%c", &c);
  }

  free(aux); /*frees the memory ocupied by the initial string*/
  return OK;
}

void Publish(struct MS_Strct *MS, char *message, int LC){
  struct Message *MSG = NULL;
  LinkedList *LL, *aux = NULL;

  MSG = (struct Message *) malloc(sizeof(struct Message));
  MSG->message = (char *) malloc((strlen(message) + 1) * sizeof(char));
  if(MSG == NULL || MSG->message == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/
  memset((void*)MSG->message,(int)'\0',(strlen(message)+1)); /*Makes sure string is empty*/

  strcpy(MSG->message, message);
  MS->LC = (max(LC, MS->LC) + 1);
  MSG->LC = MS->LC;

  MS->MSG = insertUnsortedLinkedList(MS->MSG, MSG);
  if(LC == RMB){
    printf("Broadcasting new message...\n");
    BroadCast_MSG(MS, MSG);
  }

  /*Verifies the number of messages stored and erases the last one if necessary*/
  MS->m_act++;
  if(MS->m_act > MS->m){
    for(LL = MS->MSG; LL->next != NULL; LL = getNextNodeLinkedList(LL)){
      aux = LL;
    }
    aux->next = NULL;
    MSG = getItemLinkedList(LL);
    free(MSG->message);
    free(MSG);
    free(LL);
    MS->m_act--;
  }
}
