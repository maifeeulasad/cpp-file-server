#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <string>
#include <iostream>

#define PORT 4444

int main()
{
    std::string __file_path = "./files/";
    const char *file_path = __file_path.c_str();

    struct stat st = {0};
    if (stat(file_path, &st) == -1)
    {
        mkdir(file_path, 0777);
    }

    int sockfd, ret;
    struct sockaddr_in serverAddr;

    int newSocket;
    struct sockaddr_in newAddr;

    socklen_t addr_size;

    char buffer[1024];
    pid_t childpid;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("server creation error");
        exit(1);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret < 0)
    {
        printf("bind error");
        exit(1);
    }

    if (listen(sockfd, 10) != 0)
    {
        printf("bind error");
    }

    while (1)
    {
        newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size);
        if (newSocket < 0)
        {
            exit(1);
        }
        
        if ((childpid = fork()) == 0)
        {
            close(sockfd);

            while (1)
            {
                recv(newSocket, buffer, 1024, 0);
                if (strlen(buffer) == 0)
                {
                    continue;
                }
                std::string __tem_str = std::string(buffer);
                if (__tem_str.length() > 0)
                {
                    if (__tem_str[0] == '+')
                    {

                        std::string __file_name = __tem_str.substr(1, __tem_str.find("\n"));
                        std::string __file_content = __tem_str.substr(__tem_str.find("\n") + 1);
                        std::string __name = __file_path + __file_name.substr(0, __file_name.length() - 1);
                        const char *name = __name.c_str();
                        FILE *f = fopen(name, "w+");
                        if (f != NULL)
                        {
                            fprintf(f, "%s", __file_content.c_str());
                            fclose(f);
                            send(newSocket, buffer, strlen(buffer), 0);
                            bzero(buffer, sizeof(buffer));
                        }
                    }
                    else
                    {
                        std::string __file_name = __tem_str.substr(0, __tem_str.find("\n"));
                        char *__tem_buffer;
                        FILE *f;
                        int length;

                        f = fopen((__file_path + __file_name).c_str(), "r");
                        if (f != nullptr)
                        {
                            fseek(f, 0, SEEK_END);
                            length = ftell(f);
                            fseek(f, 0, SEEK_SET);
                            __tem_buffer = (char *)malloc(length);
                            if (__tem_buffer)
                            {
                                fread(__tem_buffer, 1, length, f);
                            }
                            fclose(f);
                        }
                        send(newSocket, __tem_buffer, strlen(__tem_buffer), 0);
                        bzero(__tem_buffer, sizeof(__tem_buffer));
                    }
                }
                bzero(buffer, sizeof(buffer));
                send(newSocket, buffer, strlen(buffer), 0);
            }
        }
    }
    close(newSocket);
    return 0;
}
