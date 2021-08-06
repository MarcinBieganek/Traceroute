#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdbool.h>

int Inet_pton(int af, const char *addr, void *buf);
const char *Inet_ntop(int af, const void *restrict src, char *restrict dst, socklen_t size);
int Socket(int domain, int type, int protocol);
int Setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
ssize_t Sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);
int Select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict exceptfds, struct timeval *restrict timeout);
ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
int Gettimeofday(struct timeval *restrict tv, struct timezone *restrict tz);

u_int16_t compute_icmp_checksum(const void *buff, int length);

struct icmp create_icmp_header(uint16_t seq);
double time_diff_msc(struct timeval t1, struct timeval t2);
void send_icmp_packets(int socket, int ttl, struct timeval *send_time, const struct sockaddr *recipent);
int rec_icmp_packets(int socket, int ttl, bool *no_echo_reply, struct timeval *send_time, struct sockaddr_in senders[], double response_time[]);
void print_received(int ttl, int received_cnt, struct sockaddr_in senders[], double response_time[]);