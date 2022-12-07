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
#define TLDPORT 36902
extern int errno;

/*					STRTOK function for separating strings with delimiters
 * strtok is part of the C library and what it does is splitting a C null-delimited string into tokens separated by any 
 * delimiter you specify.  The first call to strtok must pass the C string to tokenize, and subsequent calls must specify 
 * NULL as the first argument, which tells the function to continue tokenizing the string you passed in first. The return 
 * value of the function returns a C string that is the current token retrieved. So first call --> first token, 
 * second call (with NULL specified) --> second token, and so on.  When there are no tokens left to retrieve, 
 * strtok returns NULL, meaning that the string has been fully tokenized.
 */

int main() {
        int socketfd = 0, clientfd = 0, sentbytes, recvbytes, tldfd = 0;
        socklen_t length = sizeof(struct sockaddr_in);
        struct sockaddr_in host_addr, client_addr, tld_addr;
        char buffer[512];

        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "[-]Error in socket creation.\n");
                return -1;
        } else {
		printf("[+]Socket created successfully\n\n");
	}

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(ROOTPORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                fprintf(stderr, "[-]Error in binding port to socket.\n");
                return -1;
        }
        
        while(1) {
                recvbytes = recvfrom(socketfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &length);
		if(strncmp(buffer, "exit", sizeof("exit")) == 0)
			break;

                fprintf(stdout, ">> Request received for : %s.\n", buffer);
                FILE* fd = fopen("rootdns.txt", "r");
                if(!fd) {
                        fprintf(stderr, "[-]Could not access DNS records.\n\n");
                        sendto(socketfd, "ERROR", strlen("ERROR") + 1, 0, (struct sockaddr*)&client_addr, length);
                        continue;
                }
                char linebuff[40], filebuff[400], ip[20], tempbuff[40], lastbuff[40];
                char *temp, *iptemp;
                int flag = 0;
                linebuff[0] = '\0';
                lastbuff[0] = '\0';
                filebuff[0] = '\0';
                ip[0] = '\0';
		//buffer = com
		
		// Get tld from buffer (org)
		char root[20];
		int i = 0, j = 0;
		while(buffer[i++] != '.');
		while(buffer[i++] != '.');
		while(buffer[i] != ' ' && buffer[i] != '\0') {
			root[j++] = buffer[i++];
		}
		root[j] = '\0';
		//printf("\n->%s\n",root); 

                while( fgets(linebuff, sizeof(linebuff), fd) ) {
			//read file record linebuff org 10.5.12.423
                        strcpy(tempbuff, linebuff);
			//printf("tempbuff: %s\n", linebuff);
                        temp = strtok(tempbuff, " ");
			//printf("temp: %s\n", temp);
			//temp = org deleimit at " "
                        if(flag == 0 && strncmp(temp, root, strlen(temp)) == 0) {
                                flag = 1;
                                strcpy(lastbuff, linebuff);
				//iptemp contains ip since the prev strtok gave the ptr to start of ip and 
				//'NULL' specified strtok will match the string ending with the 2nd argument or NULL
                                iptemp = strtok(NULL, "\n");
				//printf("ipbefore: %s\n",iptemp);
                                for(i = 0; *iptemp != '\0'; i++, iptemp++)
                                        ip[i] = *iptemp;
                                ip[i] = '\0';
				//printf("ipafter: %s\n",ip);
                        }
                        else {
                                strcat(filebuff, linebuff);
                        }
                }
                fclose(fd);
                if(flag == 0) {
                        sentbytes = sendto(socketfd, "3", strlen("3") + 1, 0, (struct sockaddr*)&client_addr, length);
                }
                else {
                        int fdes = open("rootdns.txt", O_WRONLY);
                        strcat(filebuff, lastbuff);
			//printf("filebuff: %s\n", filebuff);
                        write(fdes, filebuff, strlen(filebuff));
                        close(fdes);
                        fprintf(stdout, "# Response : %s.\n\n", ip);
			
			tldfd = socket(AF_INET, SOCK_DGRAM, 0);
			if(tldfd < 0){
				fprintf(stderr, "[-]Error in socket creation with TLD");
				return -1;
			}
			printf("[+] Socket creation successful with TLD\n");

			tld_addr.sin_family = AF_INET;
			tld_addr.sin_port = htons(TLDPORT);
			inet_pton(AF_INET, "127.0.0.1", &tld_addr.sin_addr);

			sentbytes = sendto(tldfd, buffer, strlen(buffer)+1, 0, (struct sockaddr*)&tld_addr, length);
			recvbytes = recvfrom(tldfd, ip, sizeof(ip), 0, NULL, NULL);
			fprintf(stdout, "# Response from TLD server: %s\n", ip);
			close(tldfd);

                        sentbytes = sendto(socketfd, ip, strlen(ip) + 1, 0, (struct sockaddr*)&client_addr, length);
                }

        }
        close(socketfd);
        return 0;
}
