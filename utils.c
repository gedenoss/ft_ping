#include "ping.h"
#include <unistd.h>

unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    while (len > 1)
    {
        sum += *buf++;
        len -= 2;
    }
    if (len == 1)
        sum += *(unsigned char*)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}


void print_stats(t_ping *stats_ping)
{
    int lose = 0;
    printf("--- %s ping statistics ---\n",stats_ping->host);

    lose = ((stats_ping->transmitted - stats_ping->received ) * 100) / stats_ping->transmitted;
    printf("%d packets transmitted, %d packets recieved, %d%% packet loss\n",
        stats_ping->transmitted,stats_ping->received,lose);
    double min = (*stats_ping).msbuffer[0], max = (*stats_ping).msbuffer[0], avg = (*stats_ping).msbuffer[0], stddev = (*stats_ping).msbuffer[0];

    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",find_min(min,&stats_ping),find_avg(avg,&stats_ping)
,find_max(max,&stats_ping),find_stddev(stddev,&stats_ping));
}

double find_min(double min,t_ping **math_ping)
{
    int i = 0;
    while(i < ((*math_ping)->count))
    {
        if(min >= (*math_ping)->msbuffer[i])
        {
            min = (*math_ping)->msbuffer[i];
        }
        i++;
    }
    return min;
}

double find_max(double max,t_ping **math_ping)
{
    int i = 0;
    while(i < ((*math_ping)->count))
    {
        if(max < (*math_ping)->msbuffer[i])
            max = (*math_ping)->msbuffer[i];
        i++;
    }
    return max;

}

double find_avg(double avg,t_ping **math_ping)
{
    int i = 0;
    double sum = 0;
    while(i < ((*math_ping)->count))
    {
        sum += (*math_ping)->msbuffer[i];
        i++;
    }
    avg = sum/(*math_ping)->count;
    return avg;
}

double find_stddev(double stddev, t_ping **math_ping)
{
    int i = 0;
    double mean = 0.0;
    double sumsq = 0.0;
    int n = (*math_ping)->count;

    if (n <= 1)
        return 0.0;

    mean = find_avg(0.0, math_ping);
    while (i < n)
    {
        double d = (*math_ping)->msbuffer[i] - mean;
        sumsq += d * d;
        i++;
    }
    stddev = sumsq / n; // variance
    return stddev;
}


void help()
{
    printf("Usage: ping HOST ...\nSend ICMP ECHO_REQUEST packets to network hosts.\n");  
}