#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>

typedef struct message{
    char color[1];
	char magic1[7];
	char name[32];
	char magic2[4];
	char mess[1024];
} message;


int main(int argc, char *argv[]){
   
	if (argc < 5){
		printf("Too few arguments\narguments: \"IP\" \"port\" \"client's name\" \"color\"\n");
		exit(1);
	}

	fd_set master;

	int sd;
    int port = atoi(argv[2]);

	char magic1[] = {0x00, 0x00, 0x00, 0xFF, 0xEE, 0xDD, 0xCC};
	char magic2[] = {0x98, 0x76, 0x54, 0x32};

	if ((sd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket error");
		return __LINE__;
	}
	
	message pack;

	strncpy(pack.color, argv[4], 1);
	memcpy(pack.magic1, magic1, 7);
	strncpy(pack.name, argv[3], 32);
	memcpy(pack.magic2, magic2, 4);
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(argv[1]);

	socklen_t len = sizeof(addr);
	sendto(sd, &pack, 1068, 0, (struct sockaddr*)&addr, len);  	

	char data[1068];
	while (1){
        FD_ZERO(&master);
        FD_SET(sd, &master);
        FD_SET(0, &master);

        if (select(sd+1, &master, NULL, NULL, NULL) == -1){
            perror("select error");
            return __LINE__;
        }
        else if (FD_ISSET(sd, &master)){
		    recvfrom(sd, &data, 1068, 0, (struct sockaddr*)&addr, &len);
		    printf("\033[%02xm%s\033[0m:%s", *(int*)(data), data+8, data+44);
        }
        else if (FD_ISSET(0, &master)){
            if (fgets(pack.mess, sizeof(pack.mess), stdin) == NULL){
                fprintf(stderr, "fgets error\n");
                continue;
            }
            sendto(sd, &pack, 1068, 0, (struct sockaddr*)&addr, len);  	
		}
    }
}

