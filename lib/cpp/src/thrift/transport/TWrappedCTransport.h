#ifndef _THRIFT_TRANSPORT_TSERVERWINPIPES_H_
#define _THRIFT_TRANSPORT_TSERVERWINPIPES_H_ 1

#include <thrift/c/transport/t_transport.h>
#include <thrift/transport/TTransport.h>

namespace apache {
namespace thrift {
namespace transport {

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
std::string to_string(const std::vector<uint8_t>& v) {
  std::stringstream ss;

  for (size_t i = 0, n = v.size(); i < n; ++i) {
    if (isprint(v[i])) {
      ss << " " << v[i];
    } else {
      ss << std::hex << std::setw(2) << std::setfill('0') << int(v[i]);
    }

    if (i < n - 1) {
      ss << ", ";
    }
  }

  return ss.str();
}

class TWrappedCTransport : public TTransport {
public:
  TWrappedCTransport() : TWrappedCTransport(nullptr){};
  TWrappedCTransport(t_transport* xport) : xport(xport) {
    if (!t_transport_is_valid(xport)) {
      throw std::invalid_argument("C transport is invalid");
    }
  }
  virtual ~TWrappedCTransport() override = default;

  virtual bool isOpen() const override { return xport->is_open(xport); }
  virtual bool peek() override { return xport->peek(xport); }
  virtual void open() override {
    if (0 != xport->open(xport)) {
      throw TTransportException();
    }
  }
  virtual void close() override {
    if (0 != xport->close(xport)) {
      throw TTransportException();
    }
  }
  virtual uint32_t read_virt(uint8_t* buf, uint32_t len) override {
    int32_t r;

    r = xport->read(xport, buf, len);
    if (r < 0) {
      throw TTransportException();
    }

    return uint32_t(r);
  }
  virtual uint32_t readAll_virt(uint8_t* buf, uint32_t len) override {
    int32_t r;

    r = xport->read_all(xport, buf, len);
    if (r < 0) {
      throw TTransportException();
    }

    return uint32_t(r);
  }
  virtual uint32_t readEnd() override {
    int32_t r;

    r = xport->read_end(xport);
    if (r < 0) {
      throw TTransportException();
    }

    return uint32_t(r);
  }
  virtual void write_virt(const uint8_t* buf, uint32_t len) override {
    if (xport->write(xport, buf, len) < 0) {
      throw TTransportException();
    } else {
      auto x = std::vector<uint8_t>(buf, buf + len);
      std::cout << "buf[" << len << "]: " << to_string(x) << std::endl;
    }
  }
  virtual uint32_t writeEnd() override {
    int32_t r;

    r = xport->write_end(xport);
    if (r < 0) {
      throw TTransportException();
    }

    return uint32_t(r);
  }
  virtual void flush() override {
    if (0 != xport->flush(xport)) {
      throw TTransportException();
    }
  }
  virtual const uint8_t* borrow_virt(uint8_t* buf, uint32_t* len) override {
    int r;

    r = xport->borrow(xport, &buf, len);
    if (r < 0 || nullptr == buf) {
      throw TTransportException();
    }

    return buf;
  }

  virtual void consume_virt(uint32_t len) override {
    if (0 != xport->consume(xport, len)) {
      throw TTransportException();
    }
  }

  virtual const std::string getOrigin() const override {
    const char* origin;

    origin = xport->get_origin(xport);

    return std::string(origin);
  }

protected:
  t_transport* const xport;
};

} // namespace transport
} // namespace thrift
} // namespace apache

#endif /* _THRIFT_TRANSPORT_TWRAPPEDCTRANSPORT_H_ */
