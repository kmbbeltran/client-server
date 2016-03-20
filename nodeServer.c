#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>

#define BUFSIZE 1024
char breakc[BUFSIZE];

int main( int args, char *argv[] ) {
  strcpy(breakc, "start");
  char portnum[BUFSIZE];
  int temp;
  printf("Please enter port you would like to listen on: ");
  fgets(portnum, BUFSIZE, stdin);   //get password
  sscanf(portnum, "%d", &temp);

// create socket
  int servsock = socket( AF_INET, SOCK_STREAM, 0 );
  if(servsock == -1){
    printf("socket could not be created\n");
    return 1;
  }

  printf("socket created\n");

  // create server socket address
  struct sockaddr_in servaddr;
  memset( &servaddr, 0, sizeof(servaddr) );
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(temp);
  servaddr.sin_addr.s_addr = INADDR_ANY;

  // configure servsock using servaddr
  if(bind( servsock, (struct sockaddr *)&servaddr, sizeof(servaddr) ) < 0){
    printf("bind failed\n");
    return 1;
  }
  printf("bind completed\n");

  while(1){
    strcpy(breakc, "start");
    printf("top breakc: %s\n", breakc);
    // make servsock listen for incoming client connections
    listen( servsock, 5 );
    printf("listening....\n");

    // accept client connection and log connection information
    struct sockaddr_in cliaddr;
    int cliLen;
    cliLen = sizeof( struct sockaddr_in );
    int clisock = accept( servsock, (struct sockaddr *)&cliaddr, (socklen_t*) &cliLen );
    if(clisock < 0){
      printf("Accept failed\n");
      return 1;
    }
    printf("connection accepted\n");

    char msg[BUFSIZE];
    char pass[BUFSIZE];
    char password[BUFSIZE] = "barbeau\n";

    send(clisock, "Please enter password: ", BUFSIZE, 0); 
    printf("Waiting for password\n\n");
    recv(clisock, pass, BUFSIZE, 0); 

    if(strcmp(pass, password) != 0){
          close( servsock );
          printf("socket closed\n");

    }else{
        while(strcmp(breakc, "quit") != 0) {

        // clear all buffers before each iteration
        memset(msg, 0, BUFSIZE);
    
        printf("waiting for client...\n");

        // get message from client
        if(recv(clisock, msg, BUFSIZE, 0) < 0){
         printf("receive failed\n");
        }

        if(strcmp(msg, "quit\n") == 0){
          strcpy(breakc, "quit");
          printf("if breakc: %s\n", breakc);
        }
        printf("THEM: %s\n", msg);
        memset(msg, 0, BUFSIZE);

        printf("YOU: ");
        fgets(msg, BUFSIZE, stdin);
        send(clisock, msg, BUFSIZE, 0);
      }
    }
    close(clisock);
    sleep(1);
  }
  // close socket, once finished
  close( servsock );
  printf("socket closed\n");
  return 0;
}
