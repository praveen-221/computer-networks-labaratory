#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#define ROOTPORT 36901      
//#define TLDPORT 36902       //THE TLD AND AUTH ARE TO BE RECEIVED FROM ROOT AND TLD SERVERS RESPECTIVELY
//#define AUTHPORT 36903      //BUT WE HARD CODE AS THIS IS A SIMULATION ONLY FOR UNDERSTANDING PURPOSES.
#define LOCALPORT 36900

int main() {
        int socketfd = 0, localfd = 0;
        int rootfd = 0;
        socklen_t length = sizeof(struct sockaddr_in);
        struct sockaddr_in host_addr, root_addr, client_addr;
        char buffer[512];
        char ip[30];
        int recvbytes, sentbytes;

        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "[-]Error in socket creation.\n");
                return -1;
        } else {
		printf("[+]LocalDNS socket created successfully.\n\n");
	}

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(LOCALPORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                fprintf(stderr, "[-]Error in binding port to socket.\n");
                return -1;
        }
	printf("[+] Socket binded successfully\n");

        while(1) {
                //UDP receive function 
		recvbytes = recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &length);
                if(strncmp(buffer, "exit", sizeof("exit")) == 0)
                        break;
                fprintf(stdout, ">> Request from client : %s\n", buffer);

                //ROOTDNS
                rootfd = socket(AF_INET, SOCK_DGRAM, 0);
                if(rootfd < 0) {
                        fprintf(stderr, "[-]Error in socket creation.\n");
                        return -1;
                }
		printf("[+]Socket creation successful with RootDNS\n");

                root_addr.sin_family = AF_INET;
                root_addr.sin_port = htons(ROOTPORT);
                inet_pton(AF_INET, "127.0.0.1", &root_addr.sin_addr);  
                
                sentbytes = sendto(rootfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&root_addr, length);
                recvbytes = recvfrom(rootfd, ip, sizeof(ip), 0, NULL, NULL);
                fprintf(stdout, "# Auth Server IP for %s: %s.\n", buffer, ip);
                close(rootfd);

                sentbytes = sendto(socketfd, ip, strlen(ip) + 1, 0, (struct sockaddr*)&client_addr, length);
        }
        close(socketfd);
        return 0;
}
