
// STL
#include <iostream>
#include <string>

// CompileTimeStamp (C++14 or less)
#define COMPILETIMESTAMP_USE_64BIT
#include "CompileTimeStamp.h"

// CompileHash (C++14)
#include "CompileHashString.h"

// CompileCryptoString (C++14)
#include "CompileCryptoString.h"

// CompileStackString (C++14)
#include "CompileStackString.h"

// CompileCryptoStringAES (C++20)
#include "CompileCryptoStringAES.h"

// CompileCryptoArray (C++20)
#include "CompileCryptoArray.h"

// CompileCryptoArrayAES (C++20)
#include "CompileCryptoArrayAES.h"

// ObfuscatedVariable (C++14)
#include "ObfuscatedVariable.h"

// RV32I (C++20)
#include "RV32I.h"

constexpr const char SOURCE_LOWERCASE[] = R"(
    addi  t4, x0, 65
    addi  t5, x0, 91
    addi  t6, x0, 32
.loop:
    lb    t0, 0(a0)
    beq   t0, x0, .done
    blt   t0, t4, .store
    bltu  t0, t5, .make
    jal   x0, .store
.make:
    add   t0, t0, t6
.store:
    sb    t0, 0(a0)
    addi  a0, a0, 1
    jal   x0, .loop
.done:
    jal   x0, exit
exit:
)";

#define RV32I_LOWERCASE(LIT) ([] {                                        \
		constexpr char lit[] = LIT;                                       \
		constexpr auto bytes = []<std::size_t M>(const char(&s)[M]) {     \
			std::array<unsigned char, M> b {};                            \
			for (std::size_t i = 0; i < M; ++i) {                         \
				b[i] = static_cast<unsigned char>(s[i]);                  \
			}                                                             \
			return b;                                                     \
		} (lit);                                                          \
		constexpr auto ctx = [] {                                         \
			rv32i::Context c {};                                          \
			return c;                                                     \
		} ();                                                             \
		constexpr auto out = rv32i::Run(SOURCE_LOWERCASE, bytes, ctx);    \
		static_assert(out.m_bOK, "lowercase VM runtime error");           \
		return []<std::size_t M>(const std::array<unsigned char, M>& d) { \
			std::array<char, M> s {};                                     \
			for (std::size_t i = 0; i < M; ++i) {                         \
				s[i] = static_cast<char>(d[i]);                           \
			}                                                             \
			return s;                                                     \
		} (out.m_Execution.m_Data);                                       \
	} ())

constexpr auto lowered = RV32I_LOWERCASE("HELLO, WORLD!");
static_assert(lowered[0] == 'h' && lowered[1] == 'e' && lowered[12] == '!', "lowercase failed");

__forceinline int own_strcmp(const char* s1, const char* s2) noexcept {
	const unsigned char* p1 = reinterpret_cast<const unsigned char*>(s1);
	const unsigned char* p2 = reinterpret_cast<const unsigned char*>(s2);

	while (*p1 == *p2) {
		if (*p1 == '\0') {
			return 0;
		}

		++p1;
		++p2;
	}

	return static_cast<int>(*p1) - static_cast<int>(*p2);
}

constexpr unsigned int kArray[] = {
	HASHSTRING32("Hello"),
	0,
	HASHSTRING32(L"World")
};

int main() {
	std::wcout << CRYPTOSTRINGAES(L"Enter password: ").c_str();
	std::string EnteredPassword;
	std::getline(std::cin, EnteredPassword);

	std::cout << STACKSTRING("Your password: ") << EnteredPassword << std::endl;

	if (own_strcmp(EnteredPassword.c_str(), CRYPTOSTRING("SecureLogon32")) == 0) {
		std::wcout << CRYPTOSTRINGAES(L"Access granted!").c_str() << std::endl;
	} else {
		std::wcout << CRYPTOSTRINGAES(L"Access denied! Incorrect password.").c_str() << std::endl;
	}

	auto arr = CRYPTOARRAY(kArray);
	ObfuscatedVariable<unsigned int> obfInt = arr[1];

	std::getchar();

	return obfInt;
}
