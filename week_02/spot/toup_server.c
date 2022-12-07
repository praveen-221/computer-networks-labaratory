#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 5500

/*struct sockaddr_in {        
 * 	short   sin_family;        
 * 	u_short sin_port;        
 * 	struct  in_addr sin_addr;        
 * 	char    sin_zero[8];
 * };
 * */

int main(){

	int sockfd, ret;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;

	socklen_t addr_size;

	char buffer[1024];
	pid_t childpid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;	//short sin_family set to contain a code for the address family
	serverAddr.sin_port = htons(PORT);	// function htons() converts a port number in host byte order to a port number in network byte order
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//The third field of sockaddr_in is a structure of type struct in_addr which contains only a single field unsigned long s_addr. This field contains the IP address of the host."INADDR_ANY" gets the machine IP in which code is running

	ret = bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", PORT);

	if(listen(sockfd, 10) == 0){
		printf("[+]Listening....\n");
	}else{
		printf("[-]Error in binding.\n");
	}


	newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
	if(newSocket < 0){
		exit(1);
	}
	printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));


	while(1){
		recv(newSocket, buffer, 1024, 0);
		if(strcmp(buffer, ":exit\n") == 0){
			printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
			break;
		}else{
			printf("From Client through port %d: %s\n",ntohs(newAddr.sin_port), buffer);
			//bzero(buffer, sizeof(buffer));
			char res[strlen(buffer)];
			for(int i = 0; ; ++i) {
				//printf("%c:", buffer[i]);
				if(buffer[i] == '\n'){
					break;
				}
				else if(buffer[i] == ' ') {
					res[i] = buffer[i];
					continue;
				} 
				else {
					res[i] = toupper(buffer[i]);
				}
			}

			printf("The message for the Client: \t%s\n", res);
			//scanf("%s", &buffer);
			//fgets(buffer, 1024, stdin);
			send(newSocket, res, strlen(res), 0);
			bzero(buffer, sizeof(buffer));
			bzero(res, sizeof(res));
		}
	}


	close(newSocket);
	return 0;
}
