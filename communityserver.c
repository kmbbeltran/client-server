#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
	
#define PORT 5000
#define BUFSIZE 1024

char* IPs[5] = {NULL, NULL, NULL, NULL, NULL}; //arrray of ip addresses
int cli_ports[5]; //array of ports
int ls_ports[5]; 
char* username[5]; 
//memset(IPs, '\0', sizeof((char*)*15));

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void rm_client(int i){

	cli_ports[i-4] = 0;
	IPs[i-4] = NULL; 
	ls_ports[i-4] = 0; 
	username[i-4] = NULL; 

	int k; 
	for(k=0; k<5; k++){
	   printf("Username: %s ", username[k]); 
       printf("IPs: %s ", IPs[k]);
       printf("Ports: %d ", cli_ports[k]);
       printf("Client Ports: %d ", ls_ports[k]);
       printf("\n");
	}
	   
}

		
void transfer(int i, fd_set *test, int sockfd, int max)
{
	int nbytes_recvd, j;
	char recv_buf[BUFSIZE], buf[BUFSIZE];

	if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
		if (nbytes_recvd == 0) {
			printf("socket %d hung up\n", i);
            rm_client(i); 

		}else {
			error("recv");
		}
		close(i);
		FD_CLR(i, test);
	}else { 
		//fflush(stdout);
		printf("recv_buf: %s\n", recv_buf);
		memset(recv_buf, '\0', BUFSIZE);
		//fflush(stdout);
		int t = 0;
		for(; t < 5; t++){
			if(IPs[t] != NULL){
				/*fflush(stdout);
				send(i, username[t], BUFSIZE - (strlen(username[t]) -1), 0);*/ 
				char str[BUFSIZE];
				/*send(i, username[t], BUFSIZE, 0);
				send(i, IPs[t], BUFSIZE, 0);*/
				char* str1 = malloc(16);
				snprintf(str1, 16, "%d", ls_ports[t]);
				//printf("this is client's port: %s\n", str1);
				//send(i, str, BUFSIZE, 0);
				strcpy(str, username[t]);
				strcat(str, "\n");
				strcat(str, IPs[t]);
				strcat(str, "\n");
				strcat(str, str1);
				printf("sending to client: %s\n", str);
				send(i, str, BUFSIZE, 0);
				//send(i, "hello everyone!\n", BUFSIZE, 0);
			}
		}

	}
        printf("\n");	
}
		
void newConnection(fd_set *test, int *max, int sockfd, struct sockaddr_in *client_addr)
{
	socklen_t addrlen;
	int newsockfd;
	char password[BUFSIZE] = "password\n";
	char authenticate[BUFSIZE] = "Please enter password: ";
	char ipAddr[BUFSIZE]; 
	char portnum[BUFSIZE]; 
	char name[BUFSIZE];
	
	addrlen = sizeof(struct sockaddr_in);
	newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen);
	if(newsockfd == -1) {
		error("accept");
		exit(1);
	}else {
	    send(newsockfd, authenticate, BUFSIZE, 0);  // sends enter password
		memset(authenticate, '\0', BUFSIZE);
		//printf("authenticate: %s\n", authenticate);
		//fflush(stdout);
		recv(newsockfd, authenticate, BUFSIZE, 0);  // get password
		//printf("authenticate: %s\n", authenticate);
		//printf("----1---\n");
		int check = strcmp(password, authenticate);
		//printf("----2---\n");
		printf("check: %d\n", check);
		if(check == 0){
			printf("user is verified\n");


			send(newsockfd, "Welcome inside, Enter your local port number: ", BUFSIZE, 0);
          //  fflush(stdout); 
            memset(portnum, '\0', BUFSIZE);
            recv(newsockfd, portnum, BUFSIZE, 0);
            printf("client port number: %s\n", portnum);

            send(newsockfd, "Enter username: ", BUFSIZE, 0); 
            //fflush(stdout); 
            memset(name, '\0', BUFSIZE);
            recv(newsockfd, name, BUFSIZE, 0);



		}else{
			
			send(newsockfd, "Wrong password", 255, 0);
			close(newsockfd);
			FD_CLR(newsockfd, test);
		}





		FD_SET(newsockfd, test);
		if(newsockfd > *max){
			*max = newsockfd;
		}
		char *ip = inet_ntoa(client_addr->sin_addr);
		int ports = ntohs(client_addr->sin_port);
		printf("new connection from %s on port %d \n\n",ip, ports);

		int itr = 0;
		for(; itr < 5; itr++){
			if(IPs[itr] == NULL){
                username[itr] = malloc(BUFSIZE+1); 
                strncpy(username[itr], name, (strlen(name)- 1)); 
                printf("Username: %s\n", username[itr]);  

				IPs[itr] = malloc(strlen(ip)+1);
				strncpy(IPs[itr], ip, strlen(ip));

				cli_ports[itr] = ports;
				printf("this is IPs: %s\n", IPs[itr]);
				printf("port is: %d\n\n", cli_ports[itr]);

				int temp;
			    sscanf(portnum, "%d", &temp);
			    ls_ports[itr] = temp;  
                printf("Clients Port: %d\n", ls_ports[itr]);
				break;
			}
		}
	

	}
}
	
int main()
{
	fd_set test;
	fd_set test2;
	int max, i, ops, sockOps, bnd, sel;
	int sockfd= 0;
	struct sockaddr_in serv_addr, client_addr;
	
	FD_ZERO(&test);
	FD_ZERO(&test2);
	ops = 1;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0); //create a internet TCP socket
	if(sockfd == -1){
		error("Socket Error\n");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET; //address of type internet
	serv_addr.sin_port = htons(PORT); //set port number
	serv_addr.sin_addr.s_addr = INADDR_ANY; //set to any address
	memset(serv_addr.sin_zero, '\0', sizeof serv_addr.sin_zero); //zero out the sin_zero

	sockOps = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &ops, sizeof(int)); //
	if(sockOps == -1){
		error("setsockopt Error\n");
	}

	bnd = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
	if(bnd == -1){
		error("Bind Error\n");
	}

	listen(sockfd, 5);
	printf("Listening for connections on the port %d\n", PORT);



	FD_SET(sockfd, &test);
	
	max = sockfd;
	while(1){

		test2 = test;
		sel = select(max+1, &test2, NULL, NULL, NULL);
		if(sel == -1){
			error("select");
		}
		
		for (i = 0; i <= max; i++){
			if (FD_ISSET(i, &test2)){
				if (i == sockfd){
					printf("i == sockfd\n");
					newConnection(&test, &max, sockfd, &client_addr);
				}
				else{
					printf("I != sockfd\n\n");
					//fflush(stdout);
					transfer(i, &test, sockfd, max);
				}
			}
		}
	}
	return 0;
}
