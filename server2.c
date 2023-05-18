#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX 100

void connection(int cli_serv_conn, int cli_number)
{
    char client_message[MAX];
    for (int i = 0; i < MAX; i++)
    {
        client_message[i] = '\0';
    }
    int i = 1;
    while (i)
    {
        bzero(client_message, MAX);
        int read_val = read(cli_serv_conn, client_message, sizeof(client_message));

        if (read_val == 0)
        {
            printf("Client %d terminated\n\n", cli_number);
            return;
        }

        int l = strlen(client_message);

        if (l == 1)
        {
            printf("From client %d : \n", cli_number);
            write(cli_serv_conn, "ERROR reading", 13);
            continue;
        }
        printf("\nFrom client %d : %s", cli_number, client_message);

        char integer1[10];
        char integer2[10];
        int n1 = 0, n2 = 0, is_val = 0;
        char operator;
        for (int i = 0; i < strlen(client_message); i++)
        {
            if (isdigit(client_message[i]) && !is_val)
            {
                integer1[n1] = client_message[i];
                n1++;
            }
            else if (client_message[i] == '+' || client_message[i] == '-' || client_message[i] == '*' || client_message[i] == '/')
            {
                if (!is_val)
                {
                    is_val = 1;
                    operator= client_message[i];
                }
                else
                {
                    is_val = 0;
                    break;
                }
            }
            else if (isdigit(client_message[i]) && is_val)
            {
                integer2[n2] = client_message[i];
                n2++;
            }
        }
        integer1[n1] = '\0';
        integer2[n2] = '\0';
        if (is_val)
        {
            int output1 = 0;
            double output2 = 0;
            int number1 = atoi(integer1);
            int number2 = atoi(integer2);
            if (operator== '+')
            {
                output1 = number1 + number2;
            }
            else if (operator== '-')
            {
                output1 = number1 - number2;
            }
            else if (operator== '*')
            {
                output1 = number1 * number2;
            }
            else if (operator== '/')
            {
                output2 = (double)number1 / (double)number2;
            }
            char out[10];
            if (operator== '/')
            {
                if (output2 == (int)output2)
                {
                    sprintf(out, "%d", (int)output2);
                }
                else
                {
                    sprintf(out, "%.2f", output2);
                }
                printf("sending : %.2f\n", output2);
                write(cli_serv_conn, out, sizeof(out));
            }
            else
            {
                sprintf(out, "%d", output1);
                printf("sending : %d\n", output1);
                write(cli_serv_conn, out, sizeof(out));
            }
        }
        else
        {
            printf("sending : Invalid Operation\n");
            write(cli_serv_conn, "Invalid Operation", 17);
        }
    }
    return;
}

int main(int argc, char **argv)
{
    int sock_desc, cli_serv_conn, len;
    int portnum = atoi(argv[1]);
    struct sockaddr_in serv_addr, cli_addr;
    pid_t pid;

    // creating socket and veryfying
    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_desc == -1)
    {
        printf("socket creation failed\n");
        exit(0);
    }
    bzero(&serv_addr, sizeof(serv_addr));

    // assigning IP adress and PORT number
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portnum);

    // Binding socket which is created to given IP and verifying the socket binding
    int sock_bind = bind(sock_desc, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (sock_bind < 0)
    {
        printf("socket failed to bind\n");
        exit(0);
    }
    else
    {
        printf("Socket successfully binded!\n");
    }

    // Server is ready for the listening from clients
    if ((listen(sock_desc, 5)) != 0)
    {
        printf("Listen failed\n");
        exit(0);
    }
    else
    {
        printf("Server listening\n");
    }
    len = sizeof(cli_addr);
    int cli_number = 0;

    while (1)
    {

        // Accepting the data from the clients and veryfying
        cli_serv_conn = accept(sock_desc, (struct sockaddr *)&cli_addr, &len);
        if (cli_serv_conn < 0)
        {
            printf("server failed to accept\n");
            exit(0);
        }
        else
        {
            printf("\nserver accepted the client %d \n", cli_number + 1);
            cli_number++;
        }

        // forking new process when client sends request
        pid = fork();
        if (pid == 0)
        {
            close(sock_desc);
            // Function for connection between client and server
            connection(cli_serv_conn, cli_number);
        }
    }
    close(sock_desc);
    return 0;
}

