#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <sys/stat.h>

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

    int clientSocket, ret;
    struct sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        printf("can't establish");
        exit(1);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    ret = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret < 0)
    {
        printf("can't connect");
        exit(1);
    }

    printf("press 1 to send file\npress 2 to receive file\nplease enter your choice: ");

    char ch = getchar();

    printf("file name : ");
    std::string filename;
    std::cin >> filename;

    if (ch == '1')
    {
        long length;
        std::string complete_path = file_path + filename;
        FILE *f = fopen(complete_path.c_str(), "r");
        if (f)
        {
            char *buffer;
            fseek(f, 0, SEEK_END);
            length = ftell(f);
            fseek(f, 0, SEEK_SET);
            buffer = (char *)malloc(length);
            if (buffer)
            {
                fread(buffer, 1, length, f);
            }
            fclose(f);
            filename = "+" + filename;
            filename += "\n";
            char *new_buffer = (char *)filename.c_str();
            strcat(new_buffer, buffer);
            send(clientSocket, new_buffer, strlen(new_buffer), 0);
        }
    }
    else if (ch == '2')
    {
        char *new_buffer = (char *)(filename + "\n").c_str();
        send(clientSocket, new_buffer, strlen(new_buffer), 0);
        char *buffer;

        std::string __name = __file_path + filename;
        char *name = (char *)__name.c_str();

        FILE *f = fopen(name, "w+");
        while (1)
        {
            if (recv(clientSocket, buffer, 1024, 0) > 0)
            {
                if (f != NULL)
                {
                    fprintf(f, "%s", buffer);
                    fclose(f);
                }
                break;
            }
        }
    }
    close(clientSocket);
    return 0;
}
