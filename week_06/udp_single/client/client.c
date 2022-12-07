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
	struct sockaddr_in servaddr; 

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("[-] socket creation failed"); 
		exit(EXIT_FAILURE); 
	}
	printf("[+] Socket Created\n");

	memset(&servaddr, 0, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY;

	//while(1) {
		bzero(&buffer, MAXLINE);
		bzero(&hello, MAXLINE);
		printf("Enter message to server: ");
		scanf("%[^\n]s", hello);
		getchar();
		int n, len; 

		sendto(sockfd, (char *)hello, strlen(hello), MSG_CONFIRM, (struct sockaddr *) &servaddr, sizeof(servaddr)); 
		printf("[+] Message sent.\n"); 

		if(strcmp(hello, "exit") == 0) {
			close(sockfd);
			return 0;
		}

		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
		buffer[n] = '\0'; 
		printf("Server : %s\n", buffer); 
	//}

	close(sockfd); 
	return 0; 
}
