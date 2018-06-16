/******************************************************************************
 *  File name: msgserv.c
 *
 *  Author: Vasco Amaral and Rui Silva
 *
 *  Release date: 2017/04/07
 *
 *  Description: C-Implementation of main application msgserv.
 *
 *  Version: 1.0
 *
 *  Change log: N/A
 *
 ******************************************************************************/
#include "msgserv.h"

int main(int argc, char **argv){
  struct MS_Strct *MS = NULL; /*Message Server main struct*/
  void (*old_handler)(int);  /*interrupt handler*/

  /*Protects agains the SIGPIPE signal*/
  if((old_handler = signal(SIGPIPE, SIG_IGN)) == SIG_ERR)
    Shutdown(SIGPIPE_ERR); /*Error*/

  /*Reads the arguments passed by the user and stores them in the MS structure*/
  MS = ReadArgumentsMS(argc, argv);

  /*Inicializes the variables in MS and makes the inicial procedures, such as
    registering the server, get all other messages and servers, and connecting with them*/
  Start_MS(MS);

  /*Runs the message server on an endless loop*/
  Run_MS(MS);

  exit(0);
}

struct MS_Strct *ReadArgumentsMS(int argc, char **argv){
  struct MS_Strct *MS = NULL;
  int i = 9; /*Auxiliar*/

  /*Makes sure the number of arguments is correct*/
  if(argc != 9 && argc != 11 && argc != 13 && argc != 15 && argc != 17)
    Shutdown(WRONG_NUMBER_ARGUMENTS);

  /*Alocates memory for the main structure MS*/
  MS = (struct MS_Strct *) malloc(sizeof(struct MS_Strct));
	if(MS == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/
  memset((void*)&MS->name,(int)'\0',sizeof(MS->name)); /*Makes sure strings are empty*/
  memset((void*)&MS->ip,(int)'\0',sizeof(MS->ip));

  /*Atributes the predefined value to the optional arguments*/
  MS->siip = GetIP("tejo.tecnico.ulisboa.pt", NAME);
  MS->sipt = 59000;
  MS->m = 200;
  MS->r = 10;

  /*Reads the mandatory arguments*/
  if(strcmp(argv[1], "-n") != 0)
    Shutdown(WRONG_ARGUMENTS);
  strcpy(MS->name, argv[2]);

  if(strcmp(argv[3], "-j") != 0)
    Shutdown(WRONG_ARGUMENTS);
  strcpy(MS->ip, argv[4]);

  if(strcmp(argv[5], "-u") != 0)
    Shutdown(WRONG_ARGUMENTS);
  MS->upt = atoi(argv[6]);

  if(strcmp(argv[7], "-t") != 0)
    Shutdown(WRONG_ARGUMENTS);
  MS->tpt = atoi(argv[8]);

  /*Reads the optional arguments*/
  while(i < argc){
    if(!strcmp (argv[i], "-i"))
      MS->siip = GetIP(argv[i+1], NUMBER);

    else if(!strcmp (argv[i], "-p"))
      MS->sipt = atoi(argv[i+1]);

    else if(!strcmp (argv[i], "-m"))
      MS->m = atoi(argv[i+1]);

    else if(!strcmp (argv[i], "-r"))
      MS->r = atoi(argv[i+1]);

    else
      Shutdown(WRONG_ARGUMENTS);
    i+=2;
  }

  return MS;
}

void Start_MS(struct MS_Strct *MS){
  char input[STR_SIZE];

  memset(input,'\0',STR_SIZE); /*Makes sure string is empty*/

  printf("Please join the network or exit the application.\n");
  printf("Command: ");

  while(fgets(input, STR_SIZE, stdin) != NULL){
    sscanf(input, "%s", input); /*Only stores the first command*/
    if(strcmp(input, "exit") == 0){
      free(MS);
      Shutdown(OK); /*Exits successfully*/
    }

    else if(strcmp(input, "join") == 0){
      printf("Joining the network...\n");
      break;
    }

    else
      printf("Command not acceptable before joining, try again:\n");
  }

  MS->IS = UDP_SET(MS->siip, MS->sipt, CLIENT); /*Defines the address and socket of the IS*/
  /*Defines the address and socket of the udp server, and binds with system*/
  MS->udp_sv = UDP_SET(INDEFINED, MS->upt, SERVER);
  /*Defines the address and socket of the tcp server, binds with system and listens to connections*/
  MS->tcp_sv = Listen_tcp(MS->tpt);
  /*Initiates all other variables contained in the main structure of the Message Server*/
  MS->MSG = initLinkedList();
  MS->Servers = initLinkedList();
  MS->m_act = 0;
  MS->LC = 0;

  REG_IS(MS->name, MS->ip, MS->upt, MS->tpt, MS->IS);
  GetSV_IS(MS, CONNECT);

}

void ReadTerm(struct MS_Strct *MS){
  char input[STR_SIZE];
  struct Message *MSG = NULL;
  struct TCP_socket *T = NULL;
  LinkedList *LL = MS->Servers;

  memset(input,'\0',STR_SIZE); /*Makes sure string is empty*/

  if(fgets(input, STR_SIZE, stdin) == NULL) Shutdown(SYSTEM); /*Error*/
  sscanf(input, "%s", input); /*Only stores the first command*/

  /*Verifies wich command was sent*/
  if(strcmp(input, "exit") == 0){
    /*Closes all sockets and frees all memory*/
    close(MS->IS->fd);
    close(MS->udp_sv->fd);
    close(MS->tcp_sv->fd);
    free(MS->IS);
    free(MS->udp_sv);
    free(MS->tcp_sv);
    while(LL != NULL){
      T = getItemLinkedList(LL);
      close(T->fd);
      free(T);
      LL = getNextNodeLinkedList(LL);
    }
    LL = MS->MSG;
    while(LL != NULL){
      MSG = getItemLinkedList(LL);
      free(MSG->message);
      free(MSG);
      LL = getNextNodeLinkedList(LL);
    }
    free(MS);
    Shutdown(OK); /*Exits successfully*/
  }

  else if(strcmp(input, "join") == 0)
    printf("Already joined the network.\n");

  else if(strcmp(input, "show_servers") == 0){
    printf("SERVERS\n");
    GetSV_IS(MS, PRINT);
  }

  else if(strcmp(input, "show_messages") == 0){
    printf("MESSAGES\n");
    Print_MSGS(MS);
  }

  else
    printf("Invalid command!\n");

}

void ReadRMB(struct MS_Strct *MS){
  char *buffer = NULL; /*Buffer to store the information received*/
  char command[STR_SIZE], message[STR_SIZE];
  int n_msgs;

  memset(command,'\0',STR_SIZE); /*Makes sure strings are empty*/
  memset(message,'\0',STR_SIZE);

  buffer = Receive_udp(MS->udp_sv, SINGLE);
  sscanf(buffer, "%s", command);

  /*Verifies wich command was sent*/
  if(strcmp(command, "PUBLISH") == 0){
    /*Eliminates the command, already stored, from the message*/
    strcpy(message, strchr(buffer, ' ')+1); /*We use the '+1' so it doesn't store ' ' */
    printf("Request to publish by RMB application\n");
    Publish(MS, message, RMB);
  }

  else if(strcmp(command, "GET_MESSAGES") == 0){
    sscanf(buffer, "%s %d", command, &n_msgs);
    printf("Request to get messages by RMB application\n");
    Send_MSGS_RMB(MS, n_msgs);
  }

  else
    Shutdown(PROTOCOL); /*Error*/;

  free(buffer); /*frees the memory ocupied by the initial string*/
}

void ReadOMS(struct MS_Strct *MS, int fd){
  char *buffer = NULL; /*Buffer to store the information received*/
  char command[STR_SIZE], message[STR_SIZE];
  int LC;

  memset(command,'\0',STR_SIZE); /*Makes sure strings are empty*/
  memset(message,'\0',STR_SIZE);

  buffer = Read_tcp(fd, SINGLE);
  sscanf(buffer, "%s", command);

  /*Verifies wich command was sent*/
  if(strcmp(command, "SMESSAGES") == 0){
    /*Eliminates the command, already stored, from the message*/
    strcpy(message, strchr(buffer, '\n')+1); /*We use the '+1' so it doesn't store '\n' */
    sscanf(message, "%d;%[^\n]", &LC, message);
    printf("Message sent by another message server\n");
    Publish(MS, message, LC);
  }

  else if(strcmp(command, "SGET_MESSAGES") == 0){
    printf("Request to get messages by another message server\n");
    Send_MSGS_OMS(MS, fd);
  }

  else
    Shutdown(PROTOCOL); /*Error*/

  free(buffer); /*frees the memory ocupied by the initial string*/
}

void Send_MSGS_RMB(struct MS_Strct *MS, int n_msgs){
  char buffer[MAX_STR_SIZE];
  char *ptr = &buffer[9]; /*Pointer to position to write in buffer*/
  LinkedList *LL = MS->MSG;
  int n = 0;

  memset(buffer,'\0',MAX_STR_SIZE); /*Makes sure string is empty*/

  strcpy(buffer, "MESSAGES\n");

  ptr = Send_MSGS_RMB_rec(MS, LL, &n, n_msgs, ptr);

  printf("Sending messages to RMB...\n");
  Send_udp(MS->udp_sv, buffer);
}

char *Send_MSGS_RMB_rec(struct MS_Strct *MS, LinkedList *LL, int *n, int n_msgs, char *ptr){
  struct Message *MSG = NULL;
  LinkedList *aux = NULL;

  if(LL != NULL && *n < n_msgs){
    aux = getNextNodeLinkedList(LL);
    *n+=1;
    ptr = Send_MSGS_RMB_rec(MS, aux, n, n_msgs, ptr);
    MSG = getItemLinkedList(LL);
    /*Writes in buffer the message and '\n'*/
    strcpy(ptr, MSG->message);
    strcpy(ptr + (strlen(MSG->message)), "\n");
    /*Sums the lenght of the message to the pointer +1 so it writes after the '\n'*/
    ptr += (strlen(MSG->message) + 1);
  }

  return ptr;
}

void Send_MSGS_OMS(struct MS_Strct *MS, int fd){
  char buffer[MAX_STR_SIZE];
  char *ptr = &buffer[10]; /*Pointer to position to write in buffer*/
  LinkedList *LL = MS->MSG;

  memset(buffer,'\0',MAX_STR_SIZE); /*Makes sure string is empty*/

  strcpy(buffer, "SMESSAGES\n");

  ptr = Send_MSGS_OMS_rec(MS, LL, ptr, buffer);

  sprintf(ptr, "\n");

  printf("Sending messages to a message server...\n");
  if(Write_tcp(fd, buffer) == CLOSED) /*Ignores if connection closed*/
    printf("Failed to send messages");
}

char *Send_MSGS_OMS_rec(struct MS_Strct *MS, LinkedList *LL, char *ptr, char *buffer){
  struct Message *MSG = NULL;
  LinkedList *aux = NULL;

  if(LL != NULL){
    aux = getNextNodeLinkedList(LL);
    ptr = Send_MSGS_OMS_rec(MS, aux, ptr, buffer);
    MSG = getItemLinkedList(LL);
    /*Writes in buffer the LC, the message and '\n'*/
    sprintf(ptr, "%d;%s\n", MSG->LC, MSG->message);
    ptr = &buffer[strlen(buffer)];
  }

  return ptr;
}

void Run_MS(struct MS_Strct *MS){
  fd_set rfds; /*Defines the set rfds, only one cause we will only read info here*/
  struct timeval tv; /*Defines the struct containing the timeout value*/
  int maxfd, ret = 0; /*Max file descriptor, needed to run select() and it's return value*/
  /*Auxiliar variables to go through the list of servers*/
  LinkedList *LL = MS->Servers;
  struct TCP_socket *T = NULL;

  tv.tv_sec = MS->r; /*Waits up to the value of r defined in the initial parameters*/
  tv.tv_usec = 0;

  printf("Running the message server...\n");

  while(1){
    /*Sets all file descriptors to pay attention in rfds*/
    FD_ZERO(&rfds); /*Clears the set rfds*/
    FD_SET(0, &rfds); /*Watchs stdin*/
    FD_SET(MS->udp_sv->fd, &rfds); /*Watches our udp server*/
    FD_SET(MS->tcp_sv->fd, &rfds); /*Watches our tcp server for connection requests*/
    maxfd = max(MS->udp_sv->fd, MS->tcp_sv->fd);
    /*Goes through all connected servers*/
    LL = MS->Servers;
    while(LL != NULL){
      T = getItemLinkedList(LL);
      FD_SET(T->fd, &rfds); /*Watches every message server connected*/
      maxfd = max(maxfd, T->fd);
      LL = getNextNodeLinkedList(LL);
    }

    /*Runs select until either one of the file descriptors is ready or the timeout expires*/
    ret = select(maxfd+1, &rfds, (fd_set*)NULL, (fd_set*)NULL, &tv);

    if(ret == -1)
      Shutdown(SELECT); /*Error*/

    /*Case in wich the timeout expired*/
    else if(ret == 0){
      REG_IS(MS->name, MS->ip, MS->upt, MS->tpt, MS->IS); /*Refreshes the registeration*/
      /*Only restarts the timeout if it has expired*/
      tv.tv_sec = MS->r; /*Waits up to the value of r defined in the initial parameters*/
      tv.tv_usec = 0;
    }

    /*Case in wich stdin has something to be read*/
    else if(FD_ISSET(0, &rfds))
      ReadTerm(MS); /*Reads and executes what is asked in the terminal*/

    /*Case in wich the udp server has something to be read*/
    else if(FD_ISSET(MS->udp_sv->fd, &rfds))
      ReadRMB(MS); /*Reads and executes what is asked through the RMB app*/

    /*Case in wich the tcp server has other server trying to connect*/
    else if(FD_ISSET(MS->tcp_sv->fd, &rfds))
      NewConnection(MS, INDEFINED, INDEFINED, IN, INDEFINED); /*Accepts pending connection*/

    /*Case in wich some server sent something*/
    else{
      LL = MS->Servers;
      /*Goes through all connected servers*/
      while(LL != NULL){
        T = getItemLinkedList(LL);
        if(FD_ISSET(T->fd, &rfds)){ /*Found the server that sent something*/
          ReadOMS(MS, T->fd); /*Reads and executes what is asked through other MS*/
          break; /*Stops going through the list*/
        }
        LL = getNextNodeLinkedList(LL);
      }
    }
  }
}
