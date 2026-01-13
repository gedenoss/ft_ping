#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include "ping.h"



void pkt_init(t_ping *p_i, struct icmphdr *p_k)
{
    memset(p_k, 0, sizeof(*p_k)); 
    //creation du packet avec la icmp
    p_k->type = ICMP_ECHO;
    p_k->un.echo.id   = getpid() & 0xFFFF;
    p_k->un.echo.sequence  = p_i->seq;
    p_k->checksum = 0;
    p_k->checksum = checksum(p_k, sizeof(*p_k));

}

void send_ping(t_ping *s_p,struct icmphdr *p_k)
{

    struct sockaddr_in ip;
    ip.sin_family = AF_INET;
    ip.sin_addr = s_p->ip_addr;

    ssize_t send = sendto(s_p->sock,p_k,sizeof(*p_k),0,(struct sockaddr *)&ip,sizeof(ip));
    if(send < 0)
    {
        perror("send\n");
    }
    else
    {
        s_p->transmitted++;
    }

}

void recv_ping(t_ping *r_p)
{
    
}


