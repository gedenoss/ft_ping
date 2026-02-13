#include "ping.h"

volatile sig_atomic_t stop;

void intHandler(int dummy) {

    (void)dummy;
    stop = 1;
}

int main(int argc,char **argv)
{
    signal(SIGINT, intHandler);
    
    if(argc == 1)
    {
        printf("ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
        return 1;
    }
    if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "--usage") == 0)
    {
        help();
        return 1;
    }
    t_ping ping;
    ping_init(&ping,argv[1]);

////////////packet
    struct icmphdr pkt;
////////////socket
    ping.sock = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP); //(ipv4,raw,imcp)
    if(ping.sock < 0)
    {
        perror("socket\n");
        return 1;
    }
    struct timeval tv = {4, 0};  // 4s
    setsockopt(ping.sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)); //configuere notre socket pour affecter le timeout du recv

 ///////////converstion char vers struct ip

    if (inet_pton(AF_INET, ping.host, &ping.ip_addr) != 1)
    {
        struct addrinfo hint;
        struct addrinfo *result; //pointeur car getaddrinfo dois renvoyer un liste a ladresse de ce pointeur

        memset(&hint, 0, sizeof(hint));
        hint.ai_family = AF_INET;
        hint.ai_socktype = SOCK_RAW;
        hint.ai_protocol = IPPROTO_ICMP;

        if(getaddrinfo(ping.host,NULL,&hint,&result) != 0)
        {
             fprintf(stderr, "ping: unknown host: %s\n", ping.host);
             return 1;
        }
        ping.ip_addr = ((struct sockaddr_in *)result->ai_addr)->sin_addr; // (struct sockaddr_in *) pour cast result->ai_addr en sockaddr_in car on est en ipv4 et '*' car le cast doit etre vers un pointeur car ai_addr est deja un pointeur 
        freeaddrinfo(result);
    }
///////////boucle
    
    printf("PING %s (%s): 8 data bytes\n",ping.host,inet_ntoa(ping.ip_addr));
    while (!stop)
    {
        
        pkt_init(&ping,&pkt);
    
        send_ping(&ping,&pkt);
    
        recv_ping(&ping);
        sleep(1);
    }
    //msg de fin
    print_stats(&ping);
    free(ping.msbuffer);
    return 0;
}