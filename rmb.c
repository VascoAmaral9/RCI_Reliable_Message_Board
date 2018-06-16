/******************************************************************************
 *  File name: rmb.c
 *
 *  Author: Vasco Amaral and Rui Silva
 *
 *  Release date: 2017/04/07
 *
 *  Description: C-Implementation of all the functions needed run the RMB
 *
 *  Version: 1.0
 *
 *  Change log: N/A
 *
 ******************************************************************************/

#include "rmb.h"

int main(int argc, char **argv){
  struct RMB_Strct *R = NULL;

  /*Reads the arguments passed by the user and stores them in the RMB structure*/
  R = ReadArgumentsRMB(argc, argv);
  /*Inicializes the variables in RMB and makes the inicial procedures, such as
    request the identitys server for information to connect an arbitrary MS */
  Start_RMB(R);
  /*Runs the reliable message board on an endless loop*/
  RunRMB(R);

  exit(0);
}

struct RMB_Strct *ReadArgumentsRMB(int argc, char **argv){
  struct RMB_Strct *R = NULL;
  int i = 1; /*Auxiliar*/

  /*Makes sure the number of arguments is correct*/
  if(argc != 1 && argc != 3 && argc != 5)
    Shutdown(WRONG_NUMBER_ARGUMENTS);

  /*Alocates memory for the main structure RMB*/
  R = (struct RMB_Strct *) malloc(sizeof(struct RMB_Strct));
	if(R == NULL) Shutdown(MEMORY); /* Check memory allocation errors*/

  /*Atributes the predefined value to the optional arguments*/
  R->siip = GetIP("tejo.tecnico.ulisboa.pt", NAME);
  R->sipt = 59000;

  /*Reads the optional arguments*/
  while(i < argc){
    if(!strcmp (argv[i], "-i"))/*looks for ip indicatior*/
      R->siip = GetIP(argv[i+1], NUMBER);

    else if(!strcmp (argv[i], "-p"))/*looks for port indicatior*/
      R->sipt = atoi(argv[i+1]);

    else
      Shutdown(WRONG_ARGUMENTS);
    i+=2;
  }

  return R;
}

void Start_RMB(struct RMB_Strct *R){

  printf("Waiting for connection with message server...\n");
  /*Defines the address and socket of the IS*/
  R->IS = UDP_SET(R->siip, R->sipt, CLIENT);
  printf("Connected with IS...\n");

 /*Resquest the messages servers registed on identitys server*/
  GetSV_MS(R, CONNECT);

  /*Defines the address and socket of MS's udp_server*/
  R->udp_sv = UDP_SET(R->ip, R->upt, CLIENT);
  printf("Connected with a message server...\n");

}

void GetSV_MS(struct RMB_Strct *R, int flag){
  char *buffer = NULL; /*Buffer to store information received from the IS*/

  char ip[STR_SIZE];
  memset(ip, '\0', STR_SIZE);
  int upt;

  /*Requests the list of message servers*/
  RequestSV_IS(R->IS);
  /*Receives the list of message servers*/
  buffer = Receive_udp(R->IS, LIST);

  if(flag == PRINT)
    printf("%s", buffer);
  else if(flag == CONNECT){
    sscanf(buffer, "%*[^;];%[^;];%d", ip, &upt);/*get information to connect with a MS*/
    R->ip = GetIP(ip, NUMBER);
    R->upt = upt;
  }

  free(buffer); /*frees the memory ocupied by the initial string, received from the IS. */
}

void Get_Messages(struct RMB_Strct *R, int n){

  char *buffer = NULL; /*Buffer to store information received from the MS*/

  /*Requests the list of messages*/
  RequestM_MS(R->udp_sv, n);
  /*Receives the list of messages*/
  buffer = Receive_udp(R->udp_sv, LIST);
  printf("%s", buffer);

  free(buffer); /*frees the memory ocupied by the initial string, received from the MS. */
}

void RunRMB(struct RMB_Strct *R){
  /*Declaration of variables*/
  char input[STR_SIZE];
  char command[STR_SIZE];
  char buffer[STR_SIZE];
  int n;
  char c;

  while(1){
    memset(input, '\0', STR_SIZE);
    memset(command, '\0', STR_SIZE);
    memset(buffer, '\0', STR_SIZE);

    if(fgets(input, STR_SIZE, stdin) == NULL) Shutdown(SYSTEM); /*Error*/
    sscanf(input, "%s %[^\n]", command, buffer); /*Only separates the first command from the input*/

    /*Verifies wich command was sent*/
    if(strcmp(command, "exit") == 0){
      free(R->IS);
      free(R->udp_sv);
      free(R);
      Shutdown(OK); /*Exits successfully*/
    }

    else if(strcmp(command, "show_servers") == 0){
      if(buffer[0] == '\0')
        GetSV_MS(R, PRINT); /*Resquest the messages servers registed on identitys server*/
      else
        printf("Command 'show_servers' must be used alone.\n");
    }

    else if(strcmp(command, "publish") == 0){
      if(buffer[0] != '\0')
        PublishSV_MS(R->udp_sv, buffer); /*Publish the string buffer on the messages server*/
	  else
        printf("Command 'publish' must be followed by the pretend message.\n");
    }

    else if(strcmp(command, "show_latest_messages") == 0){
      if(sscanf(buffer, "%d %c", &n, &c) == 1){
        Get_Messages(R, n);/*Resquest the messages server with the last n messages on the SM*/
      }else printf("Command 'show_latest_messages' must be only followed by the number of pretended messages.\n");
    }
    else
      printf("Invalid command.\n");
  }
}
