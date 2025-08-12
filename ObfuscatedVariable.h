#pragma once

#ifndef _OBFUSCATEDVARIABLE_H_
#define _OBFUSCATEDVARIABLE_H_

// Advanced
#include <intrin.h>

// CompileTimeStamp
#define COMPILETIMESTAMP_USE_64BIT
#include "CompileTimeStamp.h"

// ----------------------------------------------------------------
// General definitions
// ----------------------------------------------------------------

template <typename T>
class ObfuscatedVariable {
public:
	ObfuscatedVariable(const T InitialValue);

	T GetData() const;
	void SetData(const T Value);

	operator T() const;
	void operator=(const T Value);

private:
	void RandomizeXORMask();

private:
	T m_Data;
	T m_XORMask;
};

// ----------------------------------------------------------------
// ObfuscatedVariable
// ----------------------------------------------------------------

template <typename T>
ObfuscatedVariable<T>::ObfuscatedVariable(const T InitialValue) {
	SetData(InitialValue);
}

template <typename T>
T ObfuscatedVariable<T>::GetData() const {
	T DataCopy = m_Data;

	for (size_t i = 0; i < sizeof(T); ++i) {
		reinterpret_cast<unsigned char*>(&DataCopy)[i] ^= reinterpret_cast<const unsigned char*>(&m_XORMask)[i];
	}

	return DataCopy;
}

template <typename T>
void ObfuscatedVariable<T>::SetData(const T Value) {
	RandomizeXORMask();

	for (size_t i = 0; i < sizeof(T); ++i) {
		reinterpret_cast<unsigned char*>(&m_Data)[i] = reinterpret_cast<const unsigned char*>(&Value)[i] ^ reinterpret_cast<const unsigned char*>(&m_XORMask)[i];
	}
}

template <typename T>
ObfuscatedVariable<T>::operator T() const {
	return GetData();
}

template <typename T>
void ObfuscatedVariable<T>::operator=(const T Value) {
	SetData(Value);
}

template <typename T>
void ObfuscatedVariable<T>::RandomizeXORMask() {
	const auto GetEntropyByte = [](size_t unIndex) -> unsigned char {
		const unsigned long long unTS1 = __rdtsc();
		const unsigned long long unTS2 = __UNIX_TIMESTAMP_SECONDS__;

		unsigned long long unState = (unIndex * 0x9E3779B97F4A7C15) ^ (unTS1 * unTS2);

		unState ^= unState >> 33;
		unState *= 0xFF51AFD7ED558CCD;
		unState ^= unState >> 33;
		unState *= 0xC4CEB9FE1A85EC53;
		unState ^= unState >> 33;

		unState ^= unTS2;

		unState ^= unState << 37;
		unState *= 0x9E3779B1;
		unState ^= unState << 25;
		unState *= 0x85EBCA77;
		unState ^= unState << 38;

		return static_cast<unsigned char>((unState ^ (unState >> 32) ^ (unState >> 48)) & 0xFF);
	};

	for (size_t i = 0; i < sizeof(T); ++i) {
		reinterpret_cast<unsigned char*>(&m_XORMask)[i] = GetEntropyByte(i);
	}
}

#endif // !_OBFUSCATEDVARIABLE_H_
