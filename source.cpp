#include "header.h"

void trace_route(const char *destination, int max_hops, int timeout, const char *interface) {
    printf("Tracing route to %s...\n", destination);
    
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    if (interface) {
        if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, interface, strlen(interface)) < 0) {
            perror("Failed to bind to network interface");
            exit(EXIT_FAILURE);
        }
    }
    
    struct sockaddr_in target_addr = {0};
    target_addr.sin_family = AF_INET;
    if (inet_aton(destination, &target_addr.sin_addr) == 0) {
        struct hostent *host_info = gethostbyname(destination);
        if (!host_info) {
            fprintf(stderr, "Failed to resolve host: %s\n", destination);
            exit(EXIT_FAILURE);
        }
        target_addr.sin_addr = *((struct in_addr *)host_info->h_addr);
        printf("Resolved %s to %s\n", destination, inet_ntoa(target_addr.sin_addr));
    }
    
    for (int ttl = 1; ttl <= max_hops; ttl++) {
        if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
            perror("Failed to set TTL");
            exit(EXIT_FAILURE);
        }
        
        struct icmphdr icmp_hdr = {0};
        icmp_hdr.type = ICMP_ECHO;
        icmp_hdr.un.echo.id = getpid();
        icmp_hdr.un.echo.sequence = ttl;
        icmp_hdr.checksum = checksum(&icmp_hdr, sizeof(icmp_hdr));
        
        struct timeval start, end;
        gettimeofday(&start, NULL);
        
        if (sendto(sock, &icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) {
            perror("Failed to send ICMP packet");
            continue;
        }
        
        struct sockaddr_in recv_addr;
        socklen_t addr_len = sizeof(recv_addr);
        char buffer[PACKET_SIZE];
        
        fd_set readfds;
        struct timeval timeout_val = {timeout, 0};
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        
        if (select(sock + 1, &readfds, NULL, NULL, &timeout_val) > 0) {
            ssize_t bytes_received = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);
            if (bytes_received > 0) {
                gettimeofday(&end, NULL);
                long elapsed_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;
                
                struct icmphdr *icmp_reply = (struct icmphdr *)(buffer + sizeof(struct iphdr));
                struct hostent *host = gethostbyaddr(&recv_addr.sin_addr, sizeof(recv_addr.sin_addr), AF_INET);
                const char *hostname = host ? host->h_name : "Unknown";
                
                printf("Hop %d: %s (%s), time=%ld ms\n", ttl, inet_ntoa(recv_addr.sin_addr), hostname, elapsed_time);
                
                if (icmp_reply->type == ICMP_ECHOREPLY) {
                    printf("Destination reached!\n");
                    break;
                }
            }
        } else {
            printf("Hop %d: * * * (Request timed out)\n", ttl);
        }
        sleep(1);
    }
    
    close(sock);
}
unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short*)b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}
