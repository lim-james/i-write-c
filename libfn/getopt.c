#define _GNU_SOURCE

#include <getopt.h>
#include <unistd.h>
#include <stdio.h>

int main(int argsc, char** argsv) {
    const struct option options[] = {
        { .name = "server", .has_arg = no_argument, .flag = NULL, .val = 's' }, 
        { .name = "client", .has_arg = no_argument, .flag = NULL, .val = 'c' }, 
        { .name = "addr", .has_arg = required_argument, .flag = NULL, .val = 'a' }, 
        { .name = "port", .has_arg = required_argument, .flag = NULL, .val = 'p' }, 
        { 0 },
    };
     
    int opt;

    while ((opt = getopt_long(argsc, argsv, "sca:p:", options, NULL)) != -1) {
        switch (opt) {
            case 's': printf("Server\n"); break;
            case 'c': printf("Client\n"); break;
            case 'a': printf("Addr %s\n", optarg); break;
            case 'p': printf("Port %s\n", optarg); break;
            default:  break;
        }
    }

    while (optind < argsc) {
        printf("op: %s\n", argsv[optind]);
        ++optind;
    }

    return 0;
}
