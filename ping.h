#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>


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
    // 
    double *msbuffer;
    int count;
} t_ping;



void pkt_init(t_ping *p_i, struct icmphdr *p_k);
void ping_init(t_ping *p_init,char *av);
void send_ping(t_ping *s_p, struct icmphdr *p_k);
void recv_ping(bool verbose,t_ping *r_p);
void print_stats(t_ping *stats_ping);
void intHandler(int dummy);
void print_stats(t_ping *stats_ping);
unsigned short checksum(void *b, int len);
void help();
double find_min(double min,t_ping **math_ping);
double find_max(double max,t_ping **math_ping);
double find_avg(double avg,t_ping **math_ping);
double find_stddev(double stddev,t_ping **math_ping);

extern volatile sig_atomic_t stop;
