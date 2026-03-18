#include "ping.h"




void send_ping(t_ping *s_p,struct icmphdr *p_k2)
{
    
    struct sockaddr_in ip;
    ip.sin_family = AF_INET;
    ip.sin_addr = s_p->ip_addr;

    gettimeofday(&s_p->start_time, NULL);

    ssize_t send = sendto(s_p->sock,p_k2,sizeof(*p_k2),0,(struct sockaddr *)&ip,sizeof(ip));
    if(send < 0)
    {
        perror("ping: sending packet");
        exit(EXIT_FAILURE);
    }
    else
    {
        s_p->transmitted++;
        s_p->seq++;
    }
}

void recv_ping(t_ping *r_p)
{

    char buffer[1500];

    while (!stop) // read until we get a valid echo reply or timeout
    {
        ssize_t rec = recv(r_p->sock, buffer, 1500, 0); // may receive our own echo request on localhost

        if (rec < 0) {
            // if (errno == EAGAIN || errno == EWOULDBLOCK) { // socket timeout reached
            //     int seq = r_p->seq - 1; // last sent sequence
            //     printf("Request timeout for icmp_seq=%d\n", seq); // match ping output style
            //     return;
            // }
            if (errno == EINTR) {
                continue;
            }
            //perror("recv");
            return;
        }

        int header_len = (buffer[0] & 0x0F) * 4; //buff[0] = 1 byte = 8 octect = 4 oct version(ipv4) + 4 oct ihl (internet header leght) , & 0x0F masque 0000 1111 pour garder suelement ihl * 4 car hil nombre de mot de 32 bit qu on passse en octet exemple 5 mot de 32 bit -> * 4 = 20 oct
        int type = buffer[header_len];
        int id = *(uint16_t *)(buffer + header_len + 4); // *(unit16_t *) lire 16 bit soit 2 oct soit seulement l id qui se situe a l adresse de buffer + header + 4 oct

        //pour l affichage 
        uint16_t revseq = ntohs(*(uint16_t *)(buffer + header_len + 6));
        int ttl = (unsigned char)buffer[8];
        int bytes = rec - header_len;

        if (type == 8) { // ignore our own ICMP echo request on loopback de localhost
            continue;
        }

        if(type == 0 && id == (getpid() & 0xFFFF))
        {
            gettimeofday(&r_p->end_time, NULL);

            double ms = (r_p->end_time.tv_sec - r_p->start_time.tv_sec) * 1000.0 +
                    (r_p->end_time.tv_usec - r_p->start_time.tv_usec) / 1000.0;
            
            //tab des ms pour les stat
            r_p->msbuffer=realloc(r_p->msbuffer,(r_p->count + 1) * sizeof(double));
            r_p->msbuffer[r_p->count] = ms;
            r_p->count++;
            //fin du tab            
            
            r_p->received++;
            printf("%d bytes from %s: icmp_seq=%u ttl=%d time=%.3f ms\n",
               bytes, inet_ntoa(r_p->ip_addr), revseq, ttl, ms);
            return;
        }
        else if(type == 3)
        {
            fprintf(stderr, "Destination Unreachable\n"); // -v diff
            return;
        }
        else if(type == 11)
        {
            fprintf(stderr, "Time Exceeded (TTL expired)\n"); // v diff
            return;
        }
    }
    //r_p->seq++;
}