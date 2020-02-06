#include <string.h>

#include "ctx.h"

void ctx_init( struct ctx *ctx ) {
	memset( ctx, 0, sizeof( *ctx ) );
	ctx->fd[ SERVER ] = -1;
	ctx->fd[ CLIENT ] = -1;
}
