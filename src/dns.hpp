#ifndef DNS_HPPU
#define DNS_HPPU
#include <iostream>

namespace net {
namespace dns {

#pragma pack(push, 1)  // Ensure no padding
typedef union {
  struct {
    uint16_t qr : 1;
    uint16_t opcode : 4;
    uint16_t aa : 1;
    uint16_t tc : 1;
    uint16_t rd : 1;
    uint16_t ra : 1;
    uint16_t z : 3;
    uint16_t rcode : 4;
  } bits; // Named struct to ensure no padding.

  uint16_t value{0};
} flags;

typedef union {
  struct {
    uint16_t id{0};

    net::dns::flags flags{.value{0x00}};

    uint16_t qdcount{0};
    uint16_t ancount{0};
    uint16_t nscount{0};
    uint16_t arcount{0};
  } fields{};

  uint8_t value[12]; // Ensuring the correct size
} header;
#pragma pack(pop) // Restore default packing

} // namespace dns
} // namespace net

#endif // DNS_HPPU
