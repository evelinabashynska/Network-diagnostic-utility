#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>

#define MAX_HOPS 30
#define TIMEOUT 3
#define PACKET_SIZE 64

void trace_route(const char *destination);

unsigned short checksum(void *b, int len);