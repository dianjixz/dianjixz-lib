#include <nng/nng.h>
#include <nng/protocol/reqrep0/rep.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void server() {
    nng_socket sock;
    int rv;

    if ((rv = nng_rep0_open(&sock)) != 0) {
        fprintf(stderr, "nng_rep0_open: %s\n", nng_strerror(rv));
        exit(1);
    }

    if ((rv = nng_listen(sock, "tcp://0.0.0.0:5555", NULL, 0)) != 0) {
        fprintf(stderr, "nng_listen: %s\n", nng_strerror(rv));
        exit(1);
    }

    for (;;) {
        nng_msg *msg;
        if ((rv = nng_recvmsg(sock, &msg, 0)) != 0) {
            fprintf(stderr, "nng_recvmsg: %s\n", nng_strerror(rv));
            continue;
        }

        printf("Received: %s\n", (char *)nng_msg_body(msg));

        const char *reply = "World";
        nng_msg_append(msg, reply, strlen(reply) + 1);

        if ((rv = nng_sendmsg(sock, msg, 0)) != 0) {
            fprintf(stderr, "nng_sendmsg: %s\n", nng_strerror(rv));
            nng_msg_free(msg);
        }
    }
}

int main() {
    server();
    return 0;
}