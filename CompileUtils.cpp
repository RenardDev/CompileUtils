
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

// CompileCryptoStringAES (C++20)
#include "CompileCryptoStringAES.h"

// CompileCryptoArray (C++20)
#include "CompileCryptoArray.h"

// CompileCryptoArrayAES (C++20)
#include "CompileCryptoArrayAES.h"

// ObfuscatedVariable (C++14)
#include "ObfuscatedVariable.h"

__forceinline int own_strcmp(const char* s1, const char* s2) noexcept {
	const unsigned char* p1 = reinterpret_cast<const unsigned char*>(s1);
	const unsigned char* p2 = reinterpret_cast<const unsigned char*>(s2);

	while (*p1 == *p2) {
		if (*p1 == '\0') {
			return 0;
		}

		++p1; ++p2;
	}

	return static_cast<int>(*p1) - static_cast<int>(*p2);
}

constexpr int kArray[] = {
	HASHSTRING32("Hello"),
	0,
	HASHSTRING32(L"World")
};

int main() {
	std::wcout << CRYPTOSTRINGAES(L"Enter password: ").c_str();
	std::string EnteredPassword;
	std::getline(std::cin, EnteredPassword);

	std::cout << CRYPTOSTRING("Your password: ") << EnteredPassword << std::endl;

	if (own_strcmp(EnteredPassword.c_str(), CRYPTOSTRING("SecureLogon32")) == 0) {
		std::wcout << CRYPTOSTRINGAES(L"Access granted!").c_str() << std::endl;
	} else {
		std::wcout << CRYPTOSTRINGAES(L"Access denied! Incorrect password.").c_str() << std::endl;
	}

	auto arr = CRYPTOARRAYAES(kArray);
	ObfuscatedVariable<int> obfInt = arr[1];

	std::getchar();

	return obfInt;
}
