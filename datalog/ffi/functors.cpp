#include <cstdint>
#include <string.h>

#include "souffle/SouffleInterface.h"
#include "souffle/utility/StringUtil.h"

#if RAM_DOMAIN_SIZE == 64
Make it fail, we expect 32 bits.
Could work with 64 as well, but needs thought.
#endif

/* Note: can check uint32_t to IPv4 using python3 as second reference too:

    >>> import ipaddress
    >>> ipaddress.IPv4Address(4294966272)

*/

/* Note about asserts:

   The RamDomain, when encodes an input symbol (using the symbolTable), can
   have a valid symbol entry with id 0. So no need (and actually must not)
   assert on that.

   When RamDomain encodes a record input (from recordTable), then 0 means a
   nil value, which needs asserting or handling if deemed legal (like as a
   terminator in recursive structures).
*/

namespace internal {
uint32_t broadcast(uint32_t netaddr, uint32_t netmask) {
    return netaddr | ~netmask;
}

uint32_t overlaps(uint32_t na1, uint32_t nm1, uint32_t na2, uint32_t nm2) {
	const int32_t nb1 = broadcast(na1, nm1);
	const int32_t nb2 = broadcast(na2, nm2);
	return !((nb1 < na2) || (nb2 < na1));
}

int32_t maskbits(int32_t netmask) {
	int lsbPos = ffs(netmask);
	assert((lsbPos >= 0) && (lsbPos <= 32) && "LSB should be within limits");
	int32_t res;
	if (lsbPos == 0) {
		// No bits set at all.
		return 0;
	}
	return 32 - lsbPos + 1;

}
}  // namespace internal

extern "C" {
// See https://souffle-lang.github.io/functors about user-defined functions

souffle::RamDomain parseCidr(
		souffle::SymbolTable* symbolTable, souffle::RecordTable* recordTable, 
		souffle::RamDomain txt) {
    assert(symbolTable && "NULL symbol table");
    assert(recordTable && "NULL record table");
	const std::string& raw_addr = symbolTable->decode(txt);
	//std::cout << "raw_addr " << raw_addr << std::endl;
    const std::vector<std::string> parts = souffle::splitString(raw_addr, '/');
	assert(parts.size() == 2 && "CIDR should be a/b");
	souffle::RamUnsigned address = 0;
	int cnt = 0;
	for (auto&& bs : souffle::splitString(parts[0], '.')) {
		assert((++cnt <= 4) && "At most 4 bytes in IPv4");
		souffle::RamUnsigned b = static_cast<souffle::RamUnsigned>(std::stoul(bs));
		assert((b <= 255) && "Address byte part can be at most 255");
        address = (address << 8) | b;
	}
	//std::cout << "address " << address << std::endl;
	souffle::RamUnsigned netmask;
	souffle::RamUnsigned maskBitCount = static_cast<souffle::RamUnsigned>(std::stoul(parts[1]));
	assert((maskBitCount <= 32) && "Netmask can be at most 32");
	//std::cout << "mask bit count " << maskBitCount << std::endl;
	if (maskBitCount == 32) {
		netmask = 0xFFFFFFFF;
	} else if (maskBitCount == 0) {
		netmask = 0;
	} else {
    	netmask = 0xFFFFFFFF ^ ((1 << (32 - maskBitCount)) - 1);
	}
	//std::cout << "netmask " << netmask << std::endl;
	souffle::RamDomain res[2] = {souffle::ramBitCast(address), souffle::ramBitCast(netmask)};
	return recordTable->pack(res, 2);
}

// Returns bool-y value.
souffle::RamDomain cidrOverlaps(
		souffle::SymbolTable* symbolTable, souffle::RecordTable* recordTable,
		souffle::RamDomain c1, souffle::RamDomain c2) {
    assert(symbolTable && "NULL symbol table");
    assert(recordTable && "NULL record table");
	assert(c1 && "NULL arg cidr");
	assert(c2 && "NULL arg cidr");
	const souffle::RamDomain* c1tup = recordTable->unpack(c1, 2);
	const souffle::RamDomain* c2tup = recordTable->unpack(c2, 2);
	return souffle::ramBitCast(internal::overlaps(
		souffle::ramBitCast(c1tup[0]),
		souffle::ramBitCast(c1tup[1]),
		souffle::ramBitCast(c2tup[0]),
		souffle::ramBitCast(c2tup[1])
		));
}

souffle::RamUnsigned maskBits(
		souffle::SymbolTable* symbolTable, souffle::RecordTable* recordTable,
		souffle::RamDomain c) {
    assert(symbolTable && "NULL symbol table");
    assert(recordTable && "NULL record table");
	assert(c && "NULL arg cidr");
	const souffle::RamDomain* ctup = recordTable->unpack(c, 2);
	return internal::maskbits(ctup[1]);
}

souffle::RamDomain printCidr(
		souffle::SymbolTable* symbolTable, souffle::RecordTable* recordTable,
		souffle::RamDomain c) {
    assert(symbolTable && "NULL symbol table");
    assert(recordTable && "NULL record table");
	assert(c && "NULL arg cidr");
	const souffle::RamDomain* ctup = recordTable->unpack(c, 2);
	std::string res;
	uint32_t addr = static_cast<uint32_t>(ctup[0]);
	for (int i = 0; i < 4; i++) {
		uint32_t b = (addr >> 24) & 0xFF;
		res += std::to_string(b);
		if (i < 3) {
			res += ".";
		}
		addr = addr << 8;
	}
	const int maskbits = internal::maskbits(ctup[1]);
	res += "/" + std::to_string(maskbits);
	return symbolTable->encode(res);
}

souffle::RamDomain trueIfEqualsSymbol(
		souffle::SymbolTable* symbolTable, souffle::RecordTable* recordTable,
		souffle::RamDomain s1, souffle::RamDomain s2) {
    assert(symbolTable && "NULL symbol table");
    assert(recordTable && "NULL record table");
	// Well, maybe if interning works correctly, then even the string table
	// ids are equal.. but let's not risk.
	const std::string& sym1 = symbolTable->decode(s1);
	const std::string& sym2 = symbolTable->decode(s2);
	if (sym1 == sym2) {
		return 1;
	}
	return 0;
}

souffle::RamDomain splitIdPrefix(
		souffle::SymbolTable* symbolTable, souffle::RecordTable* recordTable, 
		souffle::RamDomain s) {
    assert(symbolTable && "NULL symbol table");
    assert(recordTable && "NULL record table");
	const std::string& str = symbolTable->decode(s);
	const std::vector<std::string> parts = souffle::splitString(str, '-');
	assert(parts.size() && "Should not be empty");
	return symbolTable->encode(parts[0]);
}

}  // extern "C"
