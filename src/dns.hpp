#ifndef DNS_HPPU
#define DNS_HPPU
#include <iostream>

namespace net {
namespace dns {

#pragma pack(push, 1)  // Ensure no padding
/**
 * @brief DNS header flags field.
 * @details
 * The DNS flags field is a 16-bit field with the following values:
 *
 * | 0  | 1 | 2 | 3 |4 | 5  | 6  | 7  | 8  |9|10|11|12|13|14|15|
 * | QR |    OPCODE    | AA | TC | RD | RA |   Z   |   RCODE   |
 *
 * Note that the QR is the most significant bit in the 16-bit field
 * while the RCODE is the least significant 4 bits. Therefore, the
 * union follows the LSB ordering so it will start with the RCODE
 * and end with the QR.
 *
 */
typedef union flags {
  struct {
    uint16_t rcode : 4;
    uint16_t z : 3;
    uint16_t ra : 1;
    uint16_t rd : 1;
    uint16_t tc : 1;
    uint16_t aa : 1;
    uint16_t opcode : 4;
    uint16_t qr : 1;
  } bits; // Named struct to ensure no padding.

  uint16_t value{0};
} flags;

typedef union {
  struct {
    uint16_t id{0};

    net::dns::flags flags{.value{0x0000}};

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
