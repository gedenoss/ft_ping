#include "ping.h"

volatile sig_atomic_t stop;

void intHandler(int dummy) {

    (void)dummy;
    stop = 1;
}

int main(int argc,char **argv)
{
    bool verbose = 0;
    int i = 1;
    int j = 1;
    int k = 1;
    int ip = 1;
    signal(SIGINT, intHandler);
    
    
    if(argc == 1)
    {
        printf("ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
        return 1;
    }
    while(i < argc)
    {
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "--usage") == 0)
        {
            help();
            return 1;
        }
        i++;
    }
    while(k < argc)
    {
        if(argv[k][0] != '-')
        {
            ip = k;
            break;
        }
        if(argv[k][0] == '-' && (argv[k][1] != 'v'))
        {
            printf("ping: invalid option -- '%c'\nTry 'ping --help' or 'ping --usage' for more information.\n", argv[k][1]);
            return 1;
        }
        k++;
    }
    while(j < argc)
    {
        if(strcmp(argv[j], "-v") == 0)
        {
            verbose = 1;
            break;
        }
        j++;
    }

    t_ping ping;
    ping_init(&ping,argv[ip]);


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


    //pour creer un ttl expired
    int ttl = 1; 
    if (setsockopt(ping.sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt IP_TTL");
    }

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
    
    printf("PING %s (%s): 8 data bytes",ping.host,inet_ntoa(ping.ip_addr));
    if(verbose == 1)
    {
        int id = getpid() & 0xFFFF;
        printf(", id 0x%04x = %d\n", id, id);
    }
    else
    {
        printf("\n");
    }

    while (!stop)
    {
        
        pkt_init(&ping,&pkt);
    
        send_ping(&ping,&pkt);
    
        recv_ping(verbose,&ping);
        sleep(1);
    }
    //msg de fin
    print_stats(&ping);
    free(ping.msbuffer);
    return 0;
}