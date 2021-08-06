// Marcin Bieganek
#include "traceroute.h"

int Inet_pton(int af, const char *addr, void *buf) {
    int inet_pton_res = inet_pton(af, addr, buf);
    if (inet_pton_res < 0) {
        fprintf(stderr, "inet_pton error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (inet_pton_res == 0) {
        fprintf(stderr, "Given argument: %s is not valid IPv4 network address!\n", addr);
        exit(EXIT_FAILURE);
    }
    return inet_pton_res;
}

const char *Inet_ntop(int af, const void *restrict src, char *restrict dst, socklen_t size) {
    const char *res = inet_ntop(af, src, dst, size);
    if (res == NULL) {
        fprintf(stderr, "inet_ntop error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return res;
}

int Socket(int domain, int type, int protocol) {
    int sockfd = socket(domain, type, protocol);
    if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		exit(EXIT_FAILURE);
	}
    return sockfd;
}

int Setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len) {
    int res = setsockopt(socket, level, option_name, option_value, option_len);
    if (res < 0) {
        fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
        exit(EXIT_FAILURE); 
    }
    return res;
}

ssize_t Sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len) {
    ssize_t bytes_sent = sendto(socket, message, length, flags, dest_addr, dest_len);
    if (bytes_sent < 0) {
        fprintf(stderr, "sendto error: %s\n", strerror(errno));
        exit(EXIT_FAILURE); 
    }
    return bytes_sent;
}

int Select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds, fd_set *restrict exceptfds, struct timeval *restrict timeout) {
    int ready = select(nfds, readfds, writefds, exceptfds, timeout);
    if (ready < 0) {
        fprintf(stderr, "select error: %s\n", strerror(errno));
        exit(EXIT_FAILURE); 
    }
    return ready;
}

ssize_t Recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    ssize_t packet_len = recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
    if (packet_len < 0) {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
        exit(EXIT_FAILURE); 
    }
    return packet_len;
}

int Gettimeofday(struct timeval *restrict tv, struct timezone *restrict tz) {
    int res = gettimeofday(tv, tz);
    if (res < 0) {
        fprintf(stderr, "gettimeofday error: %s\n", strerror(errno));
        exit(EXIT_FAILURE); 
    }
    return res;
}
