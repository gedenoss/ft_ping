#include "ping.h"


static void dump_ip_icmp_verbose(const char *buffer, ssize_t rec)
{
    struct iphdr *ip;
    int header_len;
    unsigned short frag;
    unsigned int flags;
    unsigned int off;
    char src[INET_ADDRSTRLEN];
    char dst[INET_ADDRSTRLEN];

    if (rec < (ssize_t)sizeof(struct iphdr))
        return;
    ip = (struct iphdr *)buffer;
    header_len = ip->ihl * 4;
    if (rec < header_len)
        return;

    printf("IP Hdr Dump:\n");
    for (int i = 0; i + 1 < header_len; i += 2)
        printf(" %02x%02x", (unsigned char)buffer[i], (unsigned char)buffer[i + 1]);
    if (header_len % 2 == 1)
        printf(" %02x00", (unsigned char)buffer[header_len - 1]);
    printf(" \n");

    frag = ntohs(ip->frag_off);
    flags = (frag >> 13) & 0x7;
    off = frag & 0x1FFF;
    inet_ntop(AF_INET, &ip->saddr, src, sizeof(src));
    inet_ntop(AF_INET, &ip->daddr, dst, sizeof(dst));
    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n");
    printf(" %u  %u  %02x %04x %04x   %u %04x  %02x  %02x %04x %-15s %-15s %zu\n",
        (unsigned int)ip->version,
        (unsigned int)ip->ihl,
        (unsigned int)ip->tos,
        (unsigned int)ntohs(ip->tot_len),
        (unsigned int)ntohs(ip->id),
        flags,
        off,
        (unsigned int)ip->ttl,
        (unsigned int)ip->protocol,
        (unsigned int)ntohs(ip->check),
        src,
        dst,
        (size_t)(rec - header_len));

    if (rec >= header_len + (ssize_t)sizeof(struct icmphdr))
    {
        struct icmphdr *icmp = (struct icmphdr *)(buffer + header_len);
        printf("ICMP: type %u, code %u, size %zu, id 0x%04x, seq 0x%04x\n",
            (unsigned int)icmp->type,
            (unsigned int)icmp->code,
            (size_t)(rec - header_len),
            (unsigned int)ntohs(icmp->un.echo.id),
            (unsigned int)ntohs(icmp->un.echo.sequence));
    }
}




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

void recv_ping(bool verbose,t_ping *r_p)
{

    char buffer[1500];
    char sender_ip[INET_ADDRSTRLEN]; // ip routeur qui envoie l erreur

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
        struct iphdr *ip_hdr = (struct iphdr *)buffer;
        int type = buffer[header_len];
        int id = *(uint16_t *)(buffer + header_len + 4); // *(unit16_t *) lire 16 bit soit 2 oct soit seulement l id qui se situe a l adresse de buffer + header + 4 oct

        //pour l affichage 
        uint16_t revseq = ntohs(*(uint16_t *)(buffer + header_len + 6));
        int ttl = (unsigned char)buffer[8];
        int bytes = rec - header_len;

        if (inet_ntop(AF_INET, &ip_hdr->saddr, sender_ip, sizeof(sender_ip)) == NULL)
            snprintf(sender_ip, sizeof(sender_ip), "0.0.0.0");

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
           
            if(verbose == 1)
            {
                fprintf(stderr, "%zd bytes from debian13 (%s): Destination Host Unreachable\n",
                    rec, sender_ip);
                dump_ip_icmp_verbose(buffer, rec);
            }
            else
                fprintf(stderr, "%zd bytes from debian13 (%s): Destination Host Unreachable\n",
                    rec, sender_ip);
            return;
        }
        else if(type == 11)
        {
            if(verbose == 1)
            {
                fprintf(stderr, "%zd bytes from _gateway (%s): Time to live exceeded\n",
                    rec, sender_ip); // -v diff
                dump_ip_icmp_verbose(buffer, rec);

            }
            else
                fprintf(stderr, "%zd bytes from _gateway (%s): Time to live exceeded\n",
                    rec, sender_ip); // -v diff
            stop = 1;
            return;
        }
    }
    //r_p->seq++;
}