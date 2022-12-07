#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define LOCALDNS 36900

/*
 * DNS Return Message 	 DNS Response Code  	 Function  
 * NOERROR 		RCODE:0 	   DNS Query completed successfully  
 * FORMERR 		RCODE:1 	   DNS Query Format Error  
 * SERVFAIL 		RCODE:2 	   Server failed to complete the DNS request  
 * NXDOMAIN 		RCODE:3 	   Domain name does not exist.    
 * NOTIMP 		RCODE:4 	   Function not implemented  
 * REFUSED 		RCODE:5 	   The server refused to answer for the query  
 * YXDOMAIN 		RCODE:6 	   Name that should not exist, does exist  
 * XRRSET 		RCODE:7 	   RRset that should not exist, does exist  
 * NOTAUTH 		RCODE:8 	   Server not authoritative for the zone  
 * NOTZONE	 	RCODE:9 	   Name not in zone
 */

int main() {
        int socketfd = 0, sentbytes, recvbytes;
        struct sockaddr_in host_addr;
        char input[20], buffer[20];

        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "[-]Error in socket creation.\n");
                return -1;
        } else {
		printf("[+] Socket created successfully\n\n");
	}

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(LOCALDNS);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        while(1) {
                fprintf(stdout, "[%] Enter the HostName: ");
                scanf("%s", input);
                sentbytes = sendto(socketfd, input, strlen(input) + 1, 0, (struct sockaddr*)&host_addr, sizeof(host_addr));
                if(strncmp(input, "exit", sizeof("exit")) == 0)
                        break;
                recvbytes = recvfrom(socketfd, buffer, sizeof(buffer), 0, NULL, NULL);
                printf("# SERVER IP OF %s : %s\n\n", input, buffer);
        }
        
        close(socketfd);
        return 0;
}
