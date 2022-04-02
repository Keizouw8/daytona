#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<"napi.h">

short SocketCreate(void){
    short hSocket;
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return hSocket;
}
int BindCreatedSocket(int hSocket, int ServerPort){
    int iRetval=-1;
    int ClientPort = ServerPort;
    struct sockaddr_in  remote= {0};
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = htonl(INADDR_ANY);
    remote.sin_port = htons(ClientPort);
    iRetval = bind(hSocket,(struct sockaddr *)&remote,sizeof(remote));
    return iRetval;
};
class Server{
    public:
        int ServerPort, socket_desc, sock, clientLen, read_size;
        struct sockaddr_in server, client;
        bool err, notstarted = true;
        char client_message[200]= {0};
        char message[100] = {0};
    int test(int ServerPort, void (*callback)(bool, char*)){
        socket_desc = SocketCreate();
        if (socket_desc == -1){
            (*callback)(true, "Error initializing TCP socket.\n");
            return 1;
        }
        if( BindCreatedSocket(socket_desc, ServerPort) < 0){
            (*callback)(true, "Address already in use.\n");
            return 1;
        }
        Server*notstarted = false;
        Server*err = false;
        (*callback)(false, "No errors :)");
        listen(socket_desc, 3);
        return 0;
    }
    int connection(void (*callback)(bool, char*, sendThis)){
        if(err || notstarted){
            while(1){
                clientLen = sizeof(struct sockaddr_in);
                sock = accept(socket_desc,(struct sockaddr *)&client,(socklen_t*)&clientLen);
                if (sock < 0){
                    perror("accept failed");
                    return 1;
                }
                printf("Connection accepted\n");
                memset(client_message, '\0', sizeof client_message);
                memset(message, '\0', sizeof message);
                if( recv(sock, client_message, 200, 0) < 0){
                    printf("recv failed");
                    break;
                }
                sendThis res;
                res.sock = sock;
                res.message = message;
                (*callback)(false, client_message, res);
                close(sock);
                sleep(1);
            }
        }
    }
};

void test(bool err, char* data, sendThis res){
    if(err){
        printf(data);
    }else{
        res.send("you suck");
    }
}

void testr(bool err, char* data){
    if(err){
        printf(data);
    }else{
        printf("Server listening to port 90190\n");
    }
}

int main(){
    Server server;
    server.test(90190, testr);
    server.connection(test);
}