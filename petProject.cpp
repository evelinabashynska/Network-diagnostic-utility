#include "header.h"

int main(int argc, char *argv[]) {
    int max_hops = 30;
    int timeout = 3;
    char *interface = NULL;
    
    int opt;
    while ((opt = getopt(argc, argv, "m:t:i:")) != -1) {
        switch (opt) {
            case 'm': max_hops = atoi(optarg); break;
            case 't': timeout = atoi(optarg); break;
            case 'i': interface = optarg; break;
            default:
                fprintf(stderr, "Usage: %s [-m max_hops] [-t timeout] [-i interface] <destination>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    if (optind >= argc) {
        fprintf(stderr, "Missing destination argument\n");
        exit(EXIT_FAILURE);
    }
    
    trace_route(argv[optind], max_hops, timeout, interface);
    return EXIT_SUCCESS;
}
