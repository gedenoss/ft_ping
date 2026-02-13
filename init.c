#include "ping.h"


void pkt_init(t_ping *p_i, struct icmphdr *p_k)
{
    memset(p_k, 0, sizeof(*p_k)); 
    //creation du packet avec la icmp
    p_k->type = ICMP_ECHO;
    p_k->un.echo.id = getpid() & 0xFFFF;
    p_k->un.echo.sequence = htons(p_i->seq);  // host to network short
    p_k->checksum = 0;
    p_k->checksum = checksum(p_k, sizeof(*p_k)); // en vrai raf de la verif

}

void ping_init(t_ping *p_init, char *av)
{
    p_init->host = av;
    p_init->seq = 0;
    p_init->transmitted = 0;
    p_init->received = 0;
    p_init->sock = 0;
    p_init->msbuffer = NULL;
    p_init->count = 0;
}