#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include "ping.h"

int main(int argc,char **argv)
{
    if(argc == 1)
    {
        printf("ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
        return 1;
    }

    t_ping ping;
    ping.host = argv[1];
    ping.seq = 0;
    ping.transmitted = 0;
    ping.received = 0;


////////////packet
    struct icmphdr pkt;
////////////socket
    ping.sock = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP); //(ipv4,raw,imcp)
    if(ping.sock < 0)
    {
        perror("socket\n");
        return 1;
    }

 ///////////converstion char vers struct ip

    if (inet_pton(AF_INET, ping.host, &ping.ip_addr) != 1)
    {
        fprintf(stderr, "ping: invalid IP address: %s\n", ping.host);
        return 1;
    }
///////////boucle
    pkt_init(&ping,&pkt);
    send_ping(&ping,&pkt);
    recv_ping(&ping);
}