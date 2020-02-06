#ifndef READALL_H_
#define READALL_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

ssize_t readall( int fd, void *buf, size_t avail );

__END_DECLS

#endif /* READALL_H_ */
