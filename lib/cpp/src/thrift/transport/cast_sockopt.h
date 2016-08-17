#ifndef THRIFT_TRANSPORT_CAST_SOCKOPT_H_
#define THRIFT_TRANSPORT_CAST_SOCKOPT_H_

#ifndef SOCKOPT_CAST_T
#ifndef _WIN32
#define SOCKOPT_CAST_T void
#else
#define SOCKOPT_CAST_T char
#endif // _WIN32
#endif

template <class T>
static inline const SOCKOPT_CAST_T* const_cast_sockopt(const T* v) {
  return reinterpret_cast<const SOCKOPT_CAST_T*>(v);
}

template <class T>
static inline SOCKOPT_CAST_T* cast_sockopt(T* v) {
  return reinterpret_cast<SOCKOPT_CAST_T*>(v);
}

#endif /* THRIFT_TRANSPORT_CAST_SOCKOPT_H_ */
