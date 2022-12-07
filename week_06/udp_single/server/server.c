#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#define PORT 36922 
#define MAXLINE 1024 

int main() { 
	int sockfd; 
	char buffer[MAXLINE]; 
	char hello[MAXLINE]; 
	struct sockaddr_in servaddr, cliaddr; 

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("[-]socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("[+] Socket Created\n");

	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 

	servaddr.sin_family    = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(PORT); 

	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
	{ 
		perror("[-]bind failed"); 
		exit(EXIT_FAILURE);
	} 
	printf("[+] Binded Successfully\n");

	while(1) {
		bzero(&buffer,MAXLINE);
	        bzero(&hello, MAXLINE);	
		int len, n; 
		len = sizeof(cliaddr);  

		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len); 
		buffer[n] = '\0'; 
		printf("Client : %s\n", buffer);
		if(strcmp(buffer, "exit") == 0) {
			printf("[+] Exiting...\n");
			break;
		}
	        printf("Messsage to client: ");
		scanf("%[^\n]s", hello);	
		getchar();
		sendto(sockfd, (char *)hello, strlen(hello), MSG_CONFIRM, (struct sockaddr *) &cliaddr, len); 
		printf("[+] Message sent.\n");
	}	

	return 0; 
}
