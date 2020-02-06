#ifndef CTX_H_
#define CTX_H_

#include <netinet/in.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

enum {
    SERVER,
    CLIENT,
};

struct ctx {
    int fd[2];
    struct sockaddr_in sa[2];
};

void ctx_init( struct ctx *ctx );

__END_DECLS

#endif /* CTX_H_ */
