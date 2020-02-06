#ifndef THRIFT_H_
#define THRIFT_H_

#define THRIFT_BINARY_PROTOCOL_VERSION_1 0x80010000
#define THRIFT_BINARY_PROTOCOL_VERSION_MASK 0xffff0000

/**
 * Enumerated definition of the types that the Thrift protocol supports.
 * Take special note of the T_END type which is used specifically to mark
 * the end of a sequence of fields.
 */
typedef enum {
  T_STOP   = 0,
  T_VOID   = 1,
  T_BOOL   = 2,
  T_BYTE   = 3,
  T_I08    = 3,
  T_I16    = 6,
  T_I32    = 8,
  T_U64    = 9,
  T_I64    = 10,
  T_DOUBLE = 4,
  T_STRING = 11,
  T_UTF7   = 11,
  T_STRUCT = 12,
  T_MAP    = 13,
  T_SET    = 14,
  T_LIST   = 15,
  T_UTF8   = 16,
  T_UTF16  = 17
} ThriftType;

/**
 * Enumerated definition of the message types that the Thrift protocol
 * supports.
 */
typedef enum {
  T_CALL      = 1,
  T_REPLY     = 2,
  T_EXCEPTION = 3,
  T_ONEWAY    = 4
} ThriftMessageType;

typedef enum {
  THRIFT_C_ERROR_SUCCESS,
  THRIFT_C_ERROR_UNKNOWN_METHOD,
  THRIFT_C_ERROR_INVALID_MESSAGE_TYPE,
  THRIFT_C_ERROR_WRONG_METHOD_NAME,
  THRIFT_C_ERROR_BAD_SEQUENCE_ID,
  THRIFT_C_ERROR_MISSING_RESULT,
  THRIFT_C_ERROR_INTERNAL_ERROR,
  THRIFT_C_ERROR_PROTOCOL_ERROR,
  THRIFT_C_ERROR_INVALID_TRANSFORM,
  THRIFT_C_ERROR_INVALID_PROTOCOL,
  THRIFT_C_ERROR_UNSUPPORTED_CLIENT_TYPE,
  THRIFT_C_ERROR_NOT_IMPLEMENTED,
} ThriftError;

#ifndef THRIFT_C_DEFAULT_BINARY_SIZE
#define THRIFT_C_DEFAULT_BINARY_SIZE 64
#endif /* THRIFT_C_DEFAULT_BINARY_SIZE */

#ifndef THRIFT_C_DEFAULT_STRING_SIZE
#define THRIFT_C_DEFAULT_STRING_SIZE 64
#endif /* THRIFT_C_DEFAULT_STRING_SIZE */

#ifndef THRIFT_C_DEFAULT_LIST_SIZE
#define THRIFT_C_DEFAULT_LIST_SIZE 64
#endif /* THRIFT_C_DEFAULT_LIST_SIZE */

#ifndef THRIFT_C_DEFAULT_MAP_SIZE
#define THRIFT_C_DEFAULT_MAP_SIZE 64
#endif /* THRIFT_C_DEFAULT_MAP_SIZE */

#ifndef THRIFT_C_DEFAULT_SET_SIZE
#define THRIFT_C_DEFAULT_SET_SIZE 64
#endif /* THRIFT_C_DEFAULT_SET_SIZE */

typedef struct {
    uint32_t len;
    char str[ THRIFT_C_DEFAULT_STRING_SIZE ];
} ThriftString;

typedef struct {
    size_t len;
    uint8_t bin[ THRIFT_C_DEFAULT_BINARY_SIZE ];
} ThriftBinary;


typedef struct ThriftProtocol_s ThriftProtocol;
struct ThriftProtocol_s {
  int (*write_message_begin)(ThriftProtocol *protocol, const char *name,
                                 const ThriftMessageType message_type,
                                 const uint32_t seqid);
  int (*write_message_end)(ThriftProtocol *protocol);
  int (*write_struct_begin)(ThriftProtocol *protocol, const char *name);
  int (*write_struct_end)(ThriftProtocol *protocol);
  int (*write_field_begin)(ThriftProtocol *protocol, const char *name,
                               const ThriftType field_type,
                               const int16_t field_id);
  int (*write_field_end)(ThriftProtocol *protocol);
  int (*write_field_stop)(ThriftProtocol *protocol);
  int (*write_map_begin)(ThriftProtocol *protocol,
                             const ThriftType key_type,
                             const ThriftType value_type,
                             const size_t size);
  int (*write_map_end)(ThriftProtocol *protocol);
  int (*write_list_begin)(ThriftProtocol *protocol,
                              const ThriftType element_type,
                              const size_t size);
  int (*write_list_end)(ThriftProtocol *protocol);
  int (*write_set_begin)(ThriftProtocol *protocol,
                             const ThriftType element_type,
                             const size_t size);
  int (*write_set_end)(ThriftProtocol *protocol);
  int (*write_bool)(ThriftProtocol *protocol, const bool value);
  int (*write_byte)(ThriftProtocol *protocol, const int8_t value);
  int (*write_i16)(ThriftProtocol *protocol, const int16_t value);
  int (*write_i32)(ThriftProtocol *protocol, const int32_t value);
  int (*write_i64)(ThriftProtocol *protocol, const int64_t value);
  int (*write_double)(ThriftProtocol *protocol, const double value);
  int (*write_string)(ThriftProtocol *protocol, const char *str);
  int (*write_binary)(ThriftProtocol *protocol, const void *buf,
                          const size_t len);

  int (*read_message_begin)(ThriftProtocol *thrift_protocol, char **name,
                                ThriftMessageType *message_type,
                                int32_t *seqid);
  int (*read_message_end)(ThriftProtocol *protocol);
  int (*read_struct_begin)(ThriftProtocol *protocol, char **name);
  int (*read_struct_end)(ThriftProtocol *protocol);
  int (*read_field_begin)(ThriftProtocol *protocol, char **name,
                              ThriftType *field_type, int16_t *field_id);
  int (*read_field_end)(ThriftProtocol *protocol);
  int (*read_map_begin)(ThriftProtocol *protocol, ThriftType *key_type,
                            ThriftType *value_type, size_t *size);
  int (*read_map_end)(ThriftProtocol *protocol);
  int (*read_list_begin)(ThriftProtocol *protocol, ThriftType *element_type,
                             size_t *size);
  int (*read_list_end)(ThriftProtocol *protocol);
  int (*read_set_begin)(ThriftProtocol *protocol, ThriftType *element_type,
                            size_t *size);
  int (*read_set_end)(ThriftProtocol *protocol);
  int (*read_bool)(ThriftProtocol *protocol, bool *value);
  int (*read_byte)(ThriftProtocol *protocol, int8_t *value);
  int (*read_i16)(ThriftProtocol *protocol, int16_t *value);
  int (*read_i32)(ThriftProtocol *protocol, int32_t *value);
  int (*read_i64)(ThriftProtocol *protocol, int64_t *value);
  int (*read_double)(ThriftProtocol *protocol, double *value);
  int (*read_string)(ThriftProtocol *protocol, char **str);
  int (*read_binary)(ThriftProtocol *protocol, gpointer *buf,
                         size_t *len);
};


#endif /* THRIFT_H_ */
