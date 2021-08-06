// Marcin Bieganek
#include "traceroute.h"

extern int Inet_pton(int af, const char *addr, void *buf);
extern const char *Inet_ntop(int af, const void *restrict src, char *restrict dst, socklen_t size);
extern int Socket(int domain, int type, int protocol);
extern int Setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
extern ssize_t Sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
extern int Select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict exceptfds, struct timeval *restrict timeout);
extern ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
extern int Gettimeofday(struct timeval *restrict tv, struct timezone *restrict tz);

extern u_int16_t compute_icmp_checksum(const void *buff, int length);

struct icmp create_icmp_header(u_int16_t seq) {
    struct icmp header;
    header.icmp_type = ICMP_ECHO;
    header.icmp_code = 0;
    header.icmp_hun.ih_idseq.icd_id = getpid();
    header.icmp_hun.ih_idseq.icd_seq = seq;
    header.icmp_cksum = 0;
    header.icmp_cksum = compute_icmp_checksum((u_int16_t *)&header, sizeof(header));
    return header;
}

double time_diff_msc(struct timeval t1, struct timeval t2) {
    double res = (double)(t1.tv_sec - t2.tv_sec) * 1000.0;
    res += (double)(t1.tv_usec - t2.tv_usec) / 1000.0;
    return res;
}

void send_icmp_packets(int socket, int ttl, struct timeval *send_time, const struct sockaddr *recipent) {
    Setsockopt(socket, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
    struct icmp header = create_icmp_header(ttl);
    Gettimeofday(send_time, NULL);
    for (int i = 0; i < 3; i++) {
        Sendto(socket, &header, sizeof(header), 0, recipent, sizeof(*recipent));
    }
}

int rec_icmp_packets(int socket, int ttl, bool *no_echo_reply, struct timeval *send_time, struct sockaddr_in senders[], double response_time[]) {
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(socket, &descriptors);

    int received_cnt = 0;

    struct timeval tv; tv.tv_sec = 1; tv.tv_usec = 0;

    while (received_cnt < 3) {
        int ready = Select(socket+1, &descriptors, NULL, NULL, &tv);
        if (ready == 0) { // it's timeout
            break;
        } else { // ready > 0
            struct sockaddr_in sender;
            socklen_t          sender_len = sizeof(sender);
            u_int8_t           buffer[IP_MAXPACKET];
            Recvfrom(socket, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);

            struct timeval receive_time;
            Gettimeofday(&receive_time, NULL);

            struct ip* ip_header = (struct ip*) buffer;
            u_int8_t* icmp_packet = buffer + 4 * ip_header->ip_hl;
            struct icmp* icmp_header = (struct icmp*) icmp_packet;
            //printf("\n ttl: %d, type: %d\n", ttl, icmp_header->icmp_type);

            bool received = false;
            if (icmp_header->icmp_type == ICMP_ECHOREPLY) {
                //printf(" id: %d seq: %d\n", icmp_header->icmp_hun.ih_idseq.icd_id, icmp_header->icmp_hun.ih_idseq.icd_seq);

                if (icmp_header->icmp_hun.ih_idseq.icd_id == getpid() && icmp_header->icmp_hun.ih_idseq.icd_seq == ttl) {
                    received = true;
                    *no_echo_reply = false;
                }
            } else if (icmp_header->icmp_type == ICMP_TIME_EXCEEDED) {
                struct ip* org_ip_header = (struct ip*)icmp_header->icmp_dun.id_data;
                u_int8_t* org_icmp_packet = (void *)org_ip_header + 4 * org_ip_header->ip_hl;
                struct icmp* org_icmp_header = (struct icmp*) org_icmp_packet;
                //printf(" id: %d seq: %d\n", org_icmp_header->icmp_hun.ih_idseq.icd_id, org_icmp_header->icmp_hun.ih_idseq.icd_seq);

                if (org_icmp_header->icmp_hun.ih_idseq.icd_id == getpid() && org_icmp_header->icmp_hun.ih_idseq.icd_seq == ttl) {
                    received = true;
                }
            }
            if (received) {
                senders[received_cnt] = sender;
                response_time[received_cnt] = time_diff_msc(receive_time, *send_time);
                received_cnt++;
            }
        }
    }
    return received_cnt;
}

void print_received(int ttl, int received_cnt, struct sockaddr_in senders[], double response_time[]) {
    char senders_ip_str[3][20];

    printf("%2d. ", ttl);
    if (received_cnt == 0) {
        printf("*");
    } else {
        for (int i = 0; i < received_cnt; i++) {
            Inet_ntop(AF_INET, &(senders[i].sin_addr), senders_ip_str[i], sizeof(senders_ip_str[i]));
            bool not_printed_yet = true;
            for (int j = 0; j < i; j++) {
                if (strcmp(senders_ip_str[i], senders_ip_str[j]) == 0)
                    not_printed_yet = false;
            }
            if (not_printed_yet)
                printf("%15s ", senders_ip_str[i]);
        }
        if (received_cnt == 3) {
            double avg_response_time = 0.0;
            for (int i = 0; i < received_cnt; i++) {
                avg_response_time += response_time[i];
                //printf("-- %.3fms -- ", response_time[i]);
            }
            avg_response_time = avg_response_time / 3.0;
            printf("%.3fms", avg_response_time);
        } else {
            printf("???");
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: '%s ip_addr'!\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct sockaddr_in recipent;
    bzero (&recipent, sizeof(recipent));
    recipent.sin_family = AF_INET;
    Inet_pton(AF_INET, argv[1], &recipent.sin_addr);

    int sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    struct timeval send_time;
    bool no_echo_reply = true;

    for (int ttl = 1; ttl <= 30 && no_echo_reply; ttl++) {
        // sending
        send_icmp_packets(sockfd, ttl, &send_time, (struct sockaddr*)&recipent);
        // receiving
        struct sockaddr_in senders[3];
        double response_time[3];
        int received_cnt = rec_icmp_packets(sockfd, ttl, &no_echo_reply, &send_time, senders, response_time);
        // printing
        print_received(ttl, received_cnt, senders, response_time);
    }

    return EXIT_SUCCESS;
}
