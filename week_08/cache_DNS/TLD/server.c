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
#define TLDPORT 36902
#define AUTHPORT 36903
extern int errno;
int main() {
        int socketfd = 0, clientfd = 0, sentbytes, recvbytes, authfd = 0;
        socklen_t length = sizeof(struct sockaddr_in);
        struct sockaddr_in host_addr, client_addr, auth_addr;
        char buffer[512], tld[30];

        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "[-]Error in socket creation.\n");
                return -1;
        } else {
		printf("[+]Socket created successfully.\n\n");
	}

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = htons(TLDPORT);
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
                FILE* fd = fopen("tlddns.txt", "r");
                if(!fd) {
                        fprintf(stderr, "[-]Could not access DNS records.\n");
                        sendto(socketfd, "ERROR", strlen("ERROR") + 1, 0, (struct sockaddr*)&client_addr, length);
                        continue;
                }
                char linebuff[40], filebuff[400], ip[20], tempbuff[40], lastbuff[40];
                char *temp, *iptemp;
                int flag = 0, i = 0, j = 0;
                linebuff[0] = '\0';
                lastbuff[0] = '\0';
                filebuff[0] = '\0';
                ip[0] = '\0';

		while(buffer[i++] != '.');
		while(buffer[i] != '.'){
			tld[j++] = buffer[i++];
		}
		tld[j++] = buffer[i++];
		while(buffer[i] != ' ' && buffer[i] != '\0'){
			tld[j++] = buffer[i];
			i++;
		}
		tld[j] = '\0';
		//printf("\n-> TLD: %s\n", tld);

		while( fgets(linebuff, sizeof(linebuff), fd) ) {
                        strcpy(tempbuff, linebuff);
                        temp = strtok(tempbuff, " ");
                        if(flag == 0 && strncmp(temp, tld, strlen(temp)) == 0) {
                                flag = 1;
                                strcpy(lastbuff, linebuff);
                                iptemp = strtok(NULL, "\n");
                                for(i = 0; *iptemp != '\0'; i++, iptemp++)
                                        ip[i] = *iptemp;
                                ip[i] = '\0';
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
                        int fdes = open("tlddns.txt", O_WRONLY);
                        strcat(filebuff, lastbuff);
                        write(fdes, filebuff, strlen(filebuff));
                        close(fdes);
                        fprintf(stdout, "# Response : %s.\n\n", ip);

			authfd = socket(AF_INET, SOCK_DGRAM, 0);
			if(authfd < 0){
				fprintf(stderr, "[-]Error in socket creation with auth server");
				return -1;
			}
			printf("[+]Auth socket created successfully\n");

			auth_addr.sin_family = AF_INET;
			auth_addr.sin_port = htons(AUTHPORT);
			inet_pton(AF_INET, "127.0.0.1", &auth_addr.sin_addr);

			sentbytes = sendto(authfd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&auth_addr, length);
			recvbytes = recvfrom(authfd, ip, sizeof(ip), 0, NULL, NULL);
			fprintf(stdout, "# Response from Auth server: %s\n", ip);
			close(authfd);

                        sentbytes = sendto(socketfd, ip, strlen(ip) + 1, 0, (struct sockaddr*)&client_addr, length);
                }

        }
        close(socketfd);
        return 0;
}
