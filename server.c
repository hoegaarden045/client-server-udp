#include <stdio.h>
#include <sys/socket.h>
#include <linux/net.h>
#include <arpa/inet.h>
#include <stdlib.h>


int max_num = 100;
int num_clients = 0;

int is_connected(struct sockaddr_in *addr, struct sockaddr_in *clients){
	for (int i = 0; i < num_clients; i++){
		if ((addr->sin_addr.s_addr == clients[i].sin_addr.s_addr) && (addr->sin_port == clients[i].sin_port)){
			return i;
		}
	}
	return -1;
}


int main(int argc, char *argv[]){
	
	if (argc < 2){
		printf("Too few arguments\nargument: \"port\"\n");
		exit(1);
	}

    int sd;
    struct sockaddr_in addr;

	typedef struct message{
		char color[1];
		char magic1[7];
		char name[32];
		char magic2[4];
		char mess[1024];
	} message;
	message pck;

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket error");
		return __LINE__;
	}

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[1]));
    addr.sin_addr.s_addr = inet_addr("172.16.82.56");


    if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        perror("bind");
        return __LINE__;
    }

	struct sockaddr_in *clients = malloc(100*sizeof(struct sockaddr_in));
	socklen_t len_clients = sizeof(*clients);
    
    while (1){

		struct sockaddr_in cur_client; 

		if (recvfrom(sd, &pck, 1068, 0, (struct sockaddr*)&cur_client, &len_clients) < 0){
			perror("recvfrom error");
			return __LINE__;
		}

		int index = 0; 

		if ((index = is_connected(&cur_client, clients)) < 0){
			index = num_clients;
			clients[num_clients++] = cur_client;
		}

		if (num_clients == max_num){
			max_num *= 2;
			clients = realloc(clients, max_num);
		}
        
		int send = 0;

		for (int i = 0; i < num_clients; i++){
			len_clients = sizeof(clients[i]);
			if (pck.mess[0] == '\0'){
				break;
			}
			if (index == i){
				continue;
			}

			printf("%s: %s\n", pck.name, pck.mess);

			if ((send = sendto(sd, &pck, 1068, 0, (struct sockaddr*)&clients[i], len_clients)) < 0){
				perror("sendto error");
				return __LINE__;
			}
		}
    }
}

