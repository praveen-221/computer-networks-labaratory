#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SERVERPORT 34560

void sendFile(int clientfd, char filename[]) {
        char response[512], filecontent[512], timestamp[30], line[100];
        filecontent[0] = '\0';
        int n = 0;
        struct stat file_info;
        response[0] = '\0';
        FILE* fd = fopen(filename, "r");
        if(fd == NULL) {
                strcpy(response, "404");
                send(clientfd, response, strlen(response) + 1, 0);
                return;
        }

        while(fgets(line, sizeof(line), fd)) {
                strcat(filecontent, line);
                //strcat(filecontent, "\n");
        }
        fclose(fd);

                stat(filename, &file_info);
        fprintf(stdout, "Sending file to proxy.\n");
        send(clientfd, filecontent, strlen(filecontent) + 1, 0);
        return;
}
void handleConnection(int clientfd) {
        char buffer[512], timestamp[20], filename[20], lastmod[20], response[100];
        lastmod[0] = '\0';
        struct stat file_info;
        int n;
        n = recv(clientfd, buffer, sizeof(buffer), 0);
        int i = 0, j = 0;
        while(buffer[i] != '$' && buffer[i] != '\n' && buffer[i] != '\0') {
                filename[i] = buffer[i];
                i++;
        }
        filename[i] = '\0';
        
        if(buffer[i] == '\0') {
                sendFile(clientfd, filename);
        }
}
int main() {
        int socketfd = 0, clientfd = 0, length = 0;
        struct sockaddr_in host_addr, client_addr;
        char buffer[1024];

        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(socketfd < 0) {
                fprintf(stderr, "Error in socket creation.\n");
                return -1;
        }

        host_addr.sin_family = AF_INET;
        host_addr.sin_port = ntohs(SERVERPORT);
        inet_pton(AF_INET, "127.0.0.1", &host_addr.sin_addr);

        if(bind(socketfd, (struct sockaddr*)&host_addr, sizeof(host_addr)) < 0) {
                fprintf(stderr, "Error in binding to port.\n");
                return -1;
        }

        if(listen(socketfd, 5) < 0) {
                fprintf(stderr, "Error in listening on port.\n");  
                return -1;
        }
        fprintf(stdout, "Listening on %s:%d.\n", inet_ntoa(host_addr.sin_addr), ntohs(host_addr.sin_port));

        while(1) {
                clientfd = accept(socketfd, (struct sockaddr*)&client_addr, &length);
                if(clientfd < 1) {
                        fprintf(stderr, "Error in accepting connection.\n");
                        return -1;
                }
                fprintf(stdout, "New Connection established.\n");
                handleConnection(clientfd);
                close(clientfd);
        }
        close(socketfd);
        return 0;
}
