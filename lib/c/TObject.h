#ifndef TOBJECT_H_
#define TOBJECT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>

#include <thrift/c/protocol/thrift_protocol.h>

__BEGIN_DECLS

typedef struct TObject_s TObject;

/**
 * Thrift (C) Object
 *
 * This is a reference-counted object structure for implementing the various
 * Thrift features in C.
 *
 * @refCount number of open references to the object
 * @dtor a destructor function to clean up any associated resources
 * @active true if the object is valid
 * @dynamic true if the object was dynamically allocated
 * @type ThriftTypes for common use-cases
 * @itemSize the size of an individual item (e.g. i8 is 1)
 * @itemCount the number of individual items (e.g. a single i8 is 1)
 * @b a single @bool value
 * @i8 a single @int8_t value
 * @i16 a single @int16_t value
 * @i32 a single @int32_t value
 * @i64 a single @int64_t value
 * @d a single @double value
 * @str a storage for small strings
 *
 * The @type array is organized as follows: type[0] is used for encoding the
 * scalar, parametric, or nested types. E.g. T_I08 is a scalar type; T_LIST,
 * T_SET, and T_MAP, are parametric types; T_STRUCT is a nested type. For
 * something like a map<i8,string>, type[0] = T_MAP, type[1] = T_I08, and
 * type[2] = T_STRING.
 */
struct TObject_s {
	size_t refCount;
	void (*dtor)(TObject *self);
	struct {
		bool active : 1;
		bool dynamic : 1;
	};
	ThriftType type[3];
	size_t itemSize;
	size_t itemCount;
	union {
		bool b;
		int8_t i8;
		int16_t i16;
		int32_t i32;
		int64_t i64;
		double d;
		char str[64];
	};
	void *ext[2];
};

__END_DECLS

#endif /* TOBJECT_H_ */
