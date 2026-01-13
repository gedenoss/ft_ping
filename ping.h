#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

typedef struct s_ping
{
    char *host;
    int seq;
    int transmitted;
    int received;
    struct in_addr ip_addr; // ip a la norm
    struct timeval start_time;
    struct timeval end_time;
    int sock; 
} t_ping;

void pkt_init(t_ping *p_i, struct icmphdr *p_k);
void send_ping(t_ping *s_p, struct icmphdr *p_k);
void recv_ping(t_ping *r_p);
unsigned short checksum(void *b, int len);


