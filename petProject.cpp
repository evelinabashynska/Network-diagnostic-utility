#include "header.h"
#include "source.cpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    trace_route(argv[1]);
    return EXIT_SUCCESS;
}