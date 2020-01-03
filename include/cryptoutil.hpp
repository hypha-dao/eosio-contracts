#include <eosio/crypto.hpp>
using namespace eosio;

namespace cryptoutil {

uint64_t hash (std::string str) {
    uint64_t id = 0;
    checksum256 h = sha256(const_cast<char*>(str.c_str()), str.size());
    auto hbytes = h.extract_as_byte_array();
    for(int i=0; i<4; i++) {
        id <<=8;
        id |= hbytes[i];
    }
    return id;
}

}