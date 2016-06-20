#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
	
#define BUFSIZE 1024
char breakc[BUFSIZE]; 

void authenticate(int *sockfd){
	char message[BUFSIZE];
	char pass[BUFSIZE];
	char portnum[BUFSIZE];


	recv(*sockfd, message, BUFSIZE, 0); // message: enter password
	printf("%s", message);
	fgets(pass, BUFSIZE, stdin);   //get password
	send(*sockfd, pass, BUFSIZE, 0);  // send password
	memset(message, '\0', BUFSIZE);
	fflush(stdout); 
	recv(*sockfd, message, BUFSIZE, 0);   // receive welcome inside  

	printf("%s", message); // Welcome inside
	fgets(portnum, BUFSIZE, stdin); // get port number
    	send(*sockfd, portnum, BUFSIZE, 0); // send port number

    	char unameMsg[BUFSIZE];
    	recv(*sockfd, unameMsg, BUFSIZE, 0);
    	printf("%s", unameMsg);
    	char username[BUFSIZE];
    	fgets(username, BUFSIZE, stdin);
	send(*sockfd, username, BUFSIZE, 0);
}
		
void send_recv(int i, int sockfd)
{
	char send_buf[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;

	char username[BUFSIZE];
	char ip[BUFSIZE];
	char port[BUFSIZE]; 
	
	if (i == 0){
		fgets(send_buf, BUFSIZE, stdin);
		if (strcmp(send_buf , "quit\n") == 0) {
			strcpy(breakc, "quit");
		}else
			send(sockfd, send_buf, strlen(send_buf), 0);
	}else {
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		printf("%s\n" , recv_buf);

		if(strcmp(recv_buf, "Wrong password") == 0){
			close(sockfd);
			strcpy(breakc, "quit");
		}
		fflush(stdout);
	}
}
		
		
void connect_request(int *sockfd, struct sockaddr_in *server_addr)
{
	int portnum;
	char portChar[BUFSIZE];
	char ipAddr[BUFSIZE];

	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		strcpy(breakc, "quit");
	}

	printf("Enter the ip address to connect to\n");
	fflush(stdout);
	fgets(ipAddr, BUFSIZE, stdin);

	printf("Enter port number to of server\n");
	fflush(stdout);
	fgets(portChar, BUFSIZE, stdin);
	sscanf(portChar, "%d", &portnum);
	printf("--------\n");

	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(portnum);
	server_addr->sin_addr.s_addr = inet_addr(ipAddr);
	memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);
	
	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		strcpy(breakc, "quit");
	}
	authenticate(sockfd);

}
	
int main1()
{
	int sockfd, fdmax, i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;
	
	while(strcmp(breakc, "quit") != 0){
		connect_request(&sockfd, &server_addr);
		FD_ZERO(&master);
		FD_ZERO(&read_fds);
    	FD_SET(0, &master);
    	FD_SET(sockfd, &master);
		fdmax = sockfd;
		while(strcmp(breakc, "quit") != 0){
			read_fds = master;
			if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
				perror("select");
				strcpy(breakc, "quit");
			}

			for(i=0; i <= fdmax; i++ ){
				if(FD_ISSET(i, &read_fds)){
					send_recv(i, sockfd);
				}
			}
		}
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}

void connect_request2(int *sock, struct sockaddr_in *server_addr)
{
	int portnum;
	char portChar[BUFSIZE];
	char ipAddr[BUFSIZE];
  char message[BUFSIZE];
  char pass[BUFSIZE]; 

	if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		strcpy(breakc, "quit");
	}

	printf("Enter the ip address to connect to: ");
	fflush(stdout);
	fgets(ipAddr, BUFSIZE, stdin);;

	printf("Enter port number to of server: ");
	fflush(stdout);
	fgets(portChar, BUFSIZE, stdin);
	sscanf(portChar, "%d", &portnum);
	printf("--------\n");

	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(portnum);
	server_addr->sin_addr.s_addr = inet_addr(ipAddr);
	memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);
	
	if(connect(*sock, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		strcpy(breakc, "quit");
	}

  memset(message, '\0', BUFSIZE);
  recv(*sock, message, BUFSIZE, 0); // message: enter password
  printf("%s\n", message);
  memset(pass, '\0', BUFSIZE);
  fgets(pass, BUFSIZE, stdin);   //get password
  send(*sock, pass, BUFSIZE, 0);  // send password
  memset(message, '\0', BUFSIZE);
}

int main2() {

  // create socket
  int clisock;
  struct sockaddr_in server_addr;

    // variables
  char msg[BUFSIZE];
 
  // keep communication with server 
  while(strcmp(breakc, "quit") != 0){

      // connect to contact's local server
      connect_request2(&clisock, &server_addr);
      while(strcmp(breakc, "quit") != 0){
	  	printf("YOU: ");
		fgets(msg, BUFSIZE, stdin);
  		printf("msg: %s\n", msg);
  		printf("strcmp: %d\n", strcmp(msg, "quit\n"));
		if(strcmp(msg, "quit\n") == 0){
    		printf("in the if\n");
	  		strcpy(breakc, "quit");
		}
		send(clisock, msg, BUFSIZE, 0);
		if(recv(clisock, msg, BUFSIZE, 0) < 0){
	  		printf("receive failed\n");
	  		strcpy(breakc, "quit");
		}
		printf("THEM: %s\n", msg);
      }
    }

  
  // close socket, once finished
  close( clisock );
  return 0;
} // end main

int main(){
	char sel[BUFSIZE];
	int sel2; 
	strcpy(breakc, "start");
	while(1){
    	// print menu
    	printf("Please pick an option from the menu\n");
    	printf("-----------------------------------\n");
    	printf("[1] --- Connect community server\n");
    	printf("[2] --- Start a private chat\n");
    	printf("[3] --- Exit\n");
    	printf("Selection: ");

    	// get user's selection
    	fgets(sel, BUFSIZE, stdin);
    	sscanf(sel, "%d", &sel2);

    		switch(sel2){
    			case 1 :
    				while(strcmp(breakc, "quit") != 0){
    					main1(); 
    				}
    				strcpy(breakc, "start");
    				break;

    			case 2 :
    				while(strcmp(breakc, "quit") != 0){ main2();}
    				strcpy(breakc, "start");
    				break;
    			default :
    				exit(0); 		
    		}
	}
}
