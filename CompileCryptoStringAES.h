#pragma once

#ifndef _COMPILECRYPTOSTRINGAES_H_
#define _COMPILECRYPTOSTRINGAES_H_

#include <type_traits>
#include <array>

// CompileTimeStamp
#define COMPILETIMESTAMP_USE_64BIT
#include "CompileTimeStamp.h"

// ----------------------------------------------------------------
// General definitions
// ----------------------------------------------------------------

#if defined(_MSC_VER)
#define _CRYPTOSTRINGAES_NO_INLINE __declspec(noinline)
#define _CRYPTOSTRINGAES_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define _CRYPTOSTRINGAES_NO_INLINE __attribute__((noinline))
#define _CRYPTOSTRINGAES_FORCE_INLINE __attribute__((always_inline))
#else
#define _CRYPTOSTRINGAES_NO_INLINE
#define _CRYPTOSTRINGAES_FORCE_INLINE inline
#endif

// ----------------------------------------------------------------
// CryptoStringAES
// ----------------------------------------------------------------

namespace CryptoStringAES {

	template<class _Ty>
	using clean_type = typename std::remove_const_t<std::remove_reference_t<_Ty>>;

	using block16 = std::array<unsigned char, 16>;
	using block32 = std::array<unsigned char, 32>;

	constexpr static std::array<unsigned char, 9> kAAD {{ 'R', 'e', 'n', 'a', 'r', 'd', 'D', 'e', 'v' }}; // RenardDev

	constexpr static std::array<unsigned char, 256> kIBS {{
		0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB,
		0x76, 0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4,
		0x72, 0xC0, 0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71,
		0xD8, 0x31, 0x15, 0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2,
		0xEB, 0x27, 0xB2, 0x75, 0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6,
		0xB3, 0x29, 0xE3, 0x2F, 0x84, 0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB,
		0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF, 0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45,
		0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8, 0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5,
		0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2, 0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44,
		0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73, 0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A,
		0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB, 0xE0, 0x32, 0x3A, 0x0A, 0x49,
		0x06, 0x24, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79, 0xE7, 0xC8, 0x37, 0x6D, 0x8D,
		0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08, 0xBA, 0x78, 0x25, 0x2E,
		0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A, 0x70, 0x3E, 0xB5,
		0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E, 0xE1, 0xF8,
		0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF, 0x8C,
		0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
	}};

	constexpr static std::array<unsigned int, 15> kRCON {{
		0x00000000, 0x01000000, 0x02000000,
		0x04000000, 0x08000000, 0x10000000,
		0x20000000, 0x40000000, 0x80000000,
		0x1B000000, 0x36000000, 0x6C000000,
		0xD8000000, 0xAB000000, 0x4D000000
	}};

	class AESState {
	public:
		constexpr AESState() noexcept = default;
		constexpr explicit AESState(const block16& in) noexcept : m_State(in) {}

	public:
		constexpr void SubBytes() noexcept {
			for (unsigned char i = 0; i < 16; ++i) {
				m_State[i] = kIBS[m_State[i]];
			}
		}

		constexpr void ShiftRows() noexcept {
			unsigned char unT = m_State[1]; m_State[1] = m_State[5]; m_State[5] = m_State[9]; m_State[9] = m_State[13]; m_State[13] = unT;
			unT = m_State[2]; m_State[2] = m_State[10]; m_State[10] = unT; unT = m_State[6]; m_State[6] = m_State[14]; m_State[14] = unT;
			unT = m_State[3]; m_State[3] = m_State[15]; m_State[15] = m_State[11]; m_State[11] = m_State[7]; m_State[7] = unT;
		}

		constexpr void MixColumns() noexcept {
			for (unsigned char c = 0; c < 4; ++c) {
				const unsigned char i = 4 * c;

				const unsigned char a0 = m_State[i];
				const unsigned char a1 = m_State[i + 1];
				const unsigned char a2 = m_State[i + 2];
				const unsigned char a3 = m_State[i + 3];

				m_State[i]     = static_cast<unsigned char>(GFMul(0x02, a0) ^ GFMul(0x03, a1) ^ a2 ^ a3);
				m_State[i + 1] = static_cast<unsigned char>(a0 ^ GFMul(0x02, a1) ^ GFMul(0x03, a2) ^ a3);
				m_State[i + 2] = static_cast<unsigned char>(a0 ^ a1 ^ GFMul(0x02, a2) ^ GFMul(0x03, a3));
				m_State[i + 3] = static_cast<unsigned char>(GFMul(0x03, a0) ^ a1 ^ a2 ^ GFMul(0x02, a3));
			}
		}

		constexpr void AddRoundKeyWords(const unsigned int RK[4]) noexcept {
			for (unsigned char c = 0; c < 4; ++c) {
				const unsigned int unWord = RK[c];

				m_State[4 * c]     ^= static_cast<unsigned char>((unWord >> 24) & 0xFF);
				m_State[4 * c + 1] ^= static_cast<unsigned char>((unWord >> 16) & 0xFF);
				m_State[4 * c + 2] ^= static_cast<unsigned char>((unWord >>  8) & 0xFF);
				m_State[4 * c + 3] ^= static_cast<unsigned char>( unWord        & 0xFF);
			}
		}

	private:
		constexpr unsigned char GFMul(unsigned char unA, unsigned char unB) noexcept {
			unsigned char unR = 0;
			for (unsigned char i = 0; i < 8; ++i) {
				if (unB & 1) {
					unR ^= unA;
				}

				const bool bHi = (unA & 0x80) != 0;

				unA <<= 1;

				if (bHi) {
					unA ^= 0x1B;
				}

				unB >>= 1;
			}

			return unR;
		}


	public:
		block16 m_State {};
	};

	class AES256KeySchedule {
	public:
		constexpr AES256KeySchedule() noexcept = default;
		constexpr explicit AES256KeySchedule(const block32& key) noexcept {
			for (unsigned char i = 0; i < 8; ++i) {
				m_Words[i] = _be32(&key[4 * i]);
			}

			for (unsigned char i = 8; i < 60; ++i) {
				unsigned int unT = m_Words[i - 1];
				if (i % 8 == 0) {
					const unsigned int unRot = (unT << 8) | (unT >> 24);
					const unsigned int unSub = (static_cast<unsigned int>(kIBS[(unRot >> 24) & 0xFF]) << 24) |
											   (static_cast<unsigned int>(kIBS[(unRot >> 16) & 0xFF]) << 16) |
											   (static_cast<unsigned int>(kIBS[(unRot >>  8) & 0xFF]) <<  8) |
											   (static_cast<unsigned int>(kIBS[ unRot        & 0xFF]));

					unT = unSub ^ kRCON[i / 8];

				} else if (i % 8 == 4) {
					const unsigned int unSub = (static_cast<unsigned int>(kIBS[(unT >> 24) & 0xFF]) << 24) |
											   (static_cast<unsigned int>(kIBS[(unT >> 16) & 0xFF]) << 16) |
											   (static_cast<unsigned int>(kIBS[(unT >>  8) & 0xFF]) <<  8) |
											   (static_cast<unsigned int>(kIBS[ unT        & 0xFF]));

					unT = unSub;
				}

				m_Words[i] = m_Words[i - 8] ^ unT;
			}
		}

	private:
		constexpr unsigned int _be32(const unsigned char* pBuffer) noexcept {
			return (static_cast<unsigned int>(pBuffer[0]) << 24) |
				   (static_cast<unsigned int>(pBuffer[1]) << 16) |
				   (static_cast<unsigned int>(pBuffer[2]) <<  8) |
					static_cast<unsigned int>(pBuffer[3]);
		}

	public:
		std::array<unsigned int, 60> m_Words {};
	};

	constexpr block16 AES256EncryptBlock(const block16& in, const AES256KeySchedule& ks) noexcept {
		AESState state(in);
		state.AddRoundKeyWords(&ks.m_Words[0]);

		for (unsigned char unR = 1; unR <= 13; ++unR) {
			state.SubBytes();
			state.ShiftRows();
			state.MixColumns();
			state.AddRoundKeyWords(&ks.m_Words[4 * unR]);
		}

		state.SubBytes();
		state.ShiftRows();
		state.AddRoundKeyWords(&ks.m_Words[56]);

		return state.m_State;
	}

	constexpr block16 GF128MulBE(const block16& X, const block16& Y) noexcept {
		block16 Z {};
		block16 V = Y;

		for (unsigned char unBit = 0; unBit < 128; ++unBit) {
			const unsigned char unBI = unBit / 8;
			const char nB7 = 7 - (unBit % 8);
			if (((X[unBI] >> nB7) & 1) != 0) {
				for (unsigned char i = 0; i < 16; ++i) {
					Z[i] ^= V[i];
				}
			}

			const bool bLSB = (V[15] & 1) != 0;

			for (char j = 15; j > 0; --j) {
				V[j] = static_cast<unsigned char>((V[j] >> 1) | ((V[j - 1] & 1) << 7));
			}

			V[0] >>= 1;
		
			if (bLSB) {
				V[0] ^= 0xE1;
			}
		}

		return Z;
	}

	constexpr void GHashAccamulate(const block16& H, const unsigned char* pData, std::size_t unDataLength, block16& accumulator) noexcept {
		const std::size_t unBlocks = (unDataLength + 15) / 16;
		block16 buffer {};

		for (std::size_t b = 0; b < unBlocks; ++b) {
			for (unsigned char i = 0; i < 16; ++i) {
				const std::size_t unIndex = b * 16 + i;
				buffer[i] = (unIndex < unDataLength) ? pData[unIndex] : 0;
			}

			for (unsigned char i = 0; i < 16; ++i) {
				accumulator[i] ^= buffer[i];
			}

			accumulator = GF128MulBE(accumulator, H);
		}
	}

	template <std::size_t unDataSize>
	struct GCMResult {
		std::size_t m_CipherTextSize = unDataSize;
		std::array<unsigned char, unDataSize> m_CipherText {};
		block16 m_Tag {};
		constexpr GCMResult() noexcept = default;
	};

	constexpr void inc32_inplace(block16& ctr) noexcept {
		for (char i = 15; i >= 12; --i) {
			if (++ctr[i] != 0) {
				break;
			}
		}
	}

	template <std::size_t unDataSize, std::size_t unAADSize = 0>
	constexpr GCMResult<unDataSize> GCMEncrypt(const unsigned char* pPlainText, const unsigned char* pAAD, const block32& key, const std::array<unsigned char, 12>& iv) noexcept {
		GCMResult<unDataSize> out {};

		const block16 zero {};
		const AES256KeySchedule ks(key);
		const block16 H = AES256EncryptBlock(zero, ks);

		block16 J0 {};
		for (unsigned char i = 0; i < 12; ++i) {
			J0[i] = iv[i];
			J0[15] = 1;
		}

		block16 ctr = J0;
		block16 keystream {};
		std::size_t unOutIndex = 0;

		for (std::size_t unOffset = 0; unOffset < unDataSize; unOffset += 16) {
			inc32_inplace(ctr);
			keystream = AES256EncryptBlock(ctr, ks);

			const std::size_t unChunk = (unOffset + 16 <= unDataSize) ? 16 : (unDataSize - unOffset);
			for (std::size_t i = 0; i < unChunk; ++i) {
				out.m_CipherText[unOutIndex + i] = static_cast<unsigned char>(pPlainText[unOffset + i] ^ keystream[i]);
			}

			unOutIndex += unChunk;
		}

		out.m_CipherTextSize = unOutIndex;

		block16 S {};

		if constexpr (unAADSize > 0) {
			GHashAccamulate(H, pAAD, unAADSize, S);
		}

		if (out.m_CipherTextSize) {
			GHashAccamulate(H, out.m_CipherText.data(), out.m_CipherTextSize, S);
		}

		block16 lenblock {};

		const unsigned long long unAADBits = static_cast<unsigned long long>(unAADSize) * 8ull;
		const unsigned long long unCipherBits = static_cast<unsigned long long>(out.m_CipherTextSize) * 8ull;

		for (unsigned char i = 0; i < 8; ++i) {
			lenblock[i] = static_cast<unsigned char>((unAADBits >> (56 - 8 * i)) & 0xFF);
		}

		for (unsigned char i = 0; i < 8; ++i) {
			lenblock[8 + i] = static_cast<unsigned char>((unCipherBits >> (56 - 8 * i)) & 0xFF);
		}

		for (unsigned char i = 0; i < 16; ++i) {
			S[i] ^= lenblock[i];
		}

		S = GF128MulBE(S, H);

		const block16 ENC_J0 = AES256EncryptBlock(J0, ks);
		for (unsigned char i = 0; i < 16; ++i) {
			out.m_Tag[i] = static_cast<unsigned char>(ENC_J0[i] ^ S[i]);
		}

		return out;
	}

	constexpr int compare_block16_ct(const block16& a, const block16& b) noexcept {
		unsigned char unDiff = 0;
		for (unsigned char i = 0; i < 16; ++i) {
			unDiff |= a[i] ^ b[i];
		}

		return unDiff;
	}

	_CRYPTOSTRINGAES_FORCE_INLINE bool GCMDecrypt(const unsigned char* pCipherText, std::size_t unCipherTextSize, const unsigned char* pAAD, std::size_t unAADSize, const block16& tag, const block32& key, const std::array<unsigned char, 12>& iv, unsigned char* pPlainText) noexcept {
		const block16 zero {};
		const AES256KeySchedule ks(key);
		const block16 H = AES256EncryptBlock(zero, ks);

		block16 J0 {};
		for (unsigned char i = 0; i < 12; ++i) {
			J0[i] = iv[i];
			J0[15] = 1;
		}

		block16 ctr = J0;
		block16 keysteam {};
		for (std::size_t unOffset = 0; unOffset < unCipherTextSize; unOffset += 16) {
			inc32_inplace(ctr);
			keysteam = AES256EncryptBlock(ctr, ks);

			const std::size_t unChunk = (unOffset + 16 <= unCipherTextSize) ? 16 : (unCipherTextSize - unOffset);
			for (std::size_t i = 0; i < unChunk; ++i) {
				pPlainText[unOffset + i] = static_cast<unsigned char>(pCipherText[unOffset + i] ^ keysteam[i]);
			}
		}

		block16 S {};
		if (unAADSize > 0) {
			GHashAccamulate(H, pAAD, unAADSize, S);
		}

		GHashAccamulate(H, pCipherText, unCipherTextSize, S);

		block16 lenblock {};

		const unsigned long long unAADBits = static_cast<unsigned long long>(unAADSize) * 8ull;
		const unsigned long long unCipherBits = static_cast<unsigned long long>(unCipherTextSize) * 8ull;

		for (unsigned char i = 0; i < 8; ++i) {
			lenblock[i] = static_cast<unsigned char>((unAADBits >> (56 - 8 * i)) & 0xFF);
		}

		for (unsigned char i = 0; i < 8; ++i) {
			lenblock[8 + i] = static_cast<unsigned char>((unCipherBits >> (56 - 8 * i)) & 0xFF);
		}

		for (unsigned char i = 0; i < 16; ++i) {
			S[i] ^= lenblock[i];
		}

		S = GF128MulBE(S, H);

		const block16 ENC_J0 = AES256EncryptBlock(J0, ks);

		block16 computed_tag {};
		for (unsigned char i = 0; i < 16; ++i) {
			computed_tag[i] = static_cast<unsigned char>(ENC_J0[i] ^ S[i]);
		}

		return compare_block16_ct(computed_tag, tag) == 0;
	}

	constexpr unsigned int rotl32(unsigned int x, unsigned int n) noexcept {
		return (x << n) | (x >> (32u - n));
	}

	constexpr unsigned long long rotr64(unsigned long long x, unsigned int n) noexcept {
		return (x >> n) | (x << (64u - n));
	}

	constexpr unsigned long long hash64(unsigned long long x) noexcept {
		x ^= x >> 33;
		x *= 0xFF51AFD7ED558CCDULL;
		x ^= x >> 33;
		x *= 0xC4CEB9FE1A85EC53ULL;
		x ^= x >> 33;
		return x;
	}

	struct State {
		unsigned int s0;
		unsigned int s1;
		unsigned int s2;
		unsigned int s3;
		unsigned long long s4;
	};

	constexpr State UpdateState(State state, unsigned long long unX) noexcept {
		state.s0 += static_cast<unsigned int>(unX);
		state.s1 ^= state.s0;
		state.s0 = rotl32(state.s0, 5);
		state.s2 += state.s1;
		state.s3 ^= state.s2;
		state.s2 = rotl32(state.s2, 7);
		state.s0 += state.s3;
		state.s1 ^= state.s0;
		state.s0 = rotl32(state.s0, 13);
		state.s2 += state.s1;
		state.s3 ^= state.s2;
		state.s2 = rotl32(state.s2, 17);
		state.s4 = rotr64(state.s4 + hash64(unX), 31);
		state.s0 ^= static_cast<unsigned int>(state.s4);
		state.s1 ^= static_cast<unsigned int>(state.s4 >> 32);
		return state;
	}

	constexpr static State initial = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0x8F534B8F1D4ED905 };
	constexpr static State s1 = UpdateState(initial, __UNIX_TIMESTAMP_SECONDS__);
	constexpr static State s2 = UpdateState(     s1, __UNIX_TIMESTAMP_MINUTES__);
	constexpr static State s3 = UpdateState(     s2, __UNIX_TIMESTAMP_HOURS__  );
	constexpr static State s4 = UpdateState(     s3, __UNIX_TIMESTAMP_DAYS__   );
	constexpr static State s5 = UpdateState(     s4, __UNIX_TIMESTAMP_MONTHS__ );
	constexpr static State s6 = UpdateState(     s5, __UNIX_TIMESTAMP_YEARS__  );
	constexpr static State s7 = UpdateState(     s6, 0xDEADFACE);

	constexpr static std::array<unsigned char, 32> kBaseKey = {
		static_cast<unsigned char>( s6.s0        & 0xFF),
		static_cast<unsigned char>((s6.s0 >> 8)  & 0xFF),
		static_cast<unsigned char>((s6.s0 >> 16) & 0xFF),
		static_cast<unsigned char>((s6.s0 >> 24) & 0xFF),
		static_cast<unsigned char>( s6.s1        & 0xFF),
		static_cast<unsigned char>((s6.s1 >> 8)  & 0xFF),
		static_cast<unsigned char>((s6.s1 >> 16) & 0xFF),
		static_cast<unsigned char>((s6.s1 >> 24) & 0xFF),
		static_cast<unsigned char>( s6.s2        & 0xFF),
		static_cast<unsigned char>((s6.s2 >> 8)  & 0xFF),
		static_cast<unsigned char>((s6.s2 >> 16) & 0xFF),
		static_cast<unsigned char>((s6.s2 >> 24) & 0xFF),
		static_cast<unsigned char>( s6.s3        & 0xFF),
		static_cast<unsigned char>((s6.s3 >> 8)  & 0xFF),
		static_cast<unsigned char>((s6.s3 >> 16) & 0xFF),
		static_cast<unsigned char>((s6.s3 >> 24) & 0xFF),
		static_cast<unsigned char>( s7.s0        & 0xFF),
		static_cast<unsigned char>((s7.s0 >> 8)  & 0xFF),
		static_cast<unsigned char>((s7.s0 >> 16) & 0xFF),
		static_cast<unsigned char>((s7.s0 >> 24) & 0xFF),
		static_cast<unsigned char>( s7.s1        & 0xFF),
		static_cast<unsigned char>((s7.s1 >> 8)  & 0xFF),
		static_cast<unsigned char>((s7.s1 >> 16) & 0xFF),
		static_cast<unsigned char>((s7.s1 >> 24) & 0xFF),
		static_cast<unsigned char>( s7.s2        & 0xFF),
		static_cast<unsigned char>((s7.s2 >> 8)  & 0xFF),
		static_cast<unsigned char>((s7.s2 >> 16) & 0xFF),
		static_cast<unsigned char>((s7.s2 >> 24) & 0xFF),
		static_cast<unsigned char>( s7.s3        & 0xFF),
		static_cast<unsigned char>((s7.s3 >> 8)  & 0xFF),
		static_cast<unsigned char>((s7.s3 >> 16) & 0xFF),
		static_cast<unsigned char>((s7.s3 >> 24) & 0xFF)
	};

	template<unsigned long long unLine, unsigned long long unCounter>
	class AdditionalKeyArrayAES {
	public:
		constexpr AdditionalKeyArrayAES() noexcept {
			constexpr unsigned long long unA = hash64(rotr64(unLine,          13));
			constexpr unsigned long long unB = hash64(rotr64(unA ^ unCounter, 31));
			constexpr unsigned long long unC = hash64(rotr64(unB + unLine,    17));
			constexpr unsigned long long unD = hash64(rotr64(unC ^ unCounter, 47));
			constexpr unsigned long long unE = hash64(rotr64(unD ^ unLine,    23));
			constexpr unsigned long long unF = hash64(rotr64(unE + unCounter, 37));

			const unsigned long long kSelection[6] = { unA, unB, unC, unD, unE, unF };

			for (unsigned char i = 0; i < 8; ++i) {
				const unsigned long long unX = (i < 5) ? kSelection[i] : (unF * i);
				const unsigned char unBase = static_cast<unsigned char>(i << 3);

				m_Data[unBase]     = static_cast<unsigned char>( unX        & 0xFFu);
				m_Data[unBase + 1] = static_cast<unsigned char>((unX >>  8) & 0xFFu);
				m_Data[unBase + 2] = static_cast<unsigned char>((unX >> 16) & 0xFFu);
				m_Data[unBase + 3] = static_cast<unsigned char>((unX >> 24) & 0xFFu);
				m_Data[unBase + 4] = static_cast<unsigned char>((unX >> 32) & 0xFFu);
				m_Data[unBase + 5] = static_cast<unsigned char>((unX >> 40) & 0xFFu);
				m_Data[unBase + 6] = static_cast<unsigned char>((unX >> 48) & 0xFFu);
				m_Data[unBase + 7] = static_cast<unsigned char>((unX >> 56) & 0xFFu);
			}
		}

	public:
		unsigned char m_Data[64] {};
	};

	template<unsigned long long unLine, unsigned long long unCounter>
	constexpr void BuildAESKey(unsigned char out[32]) noexcept {
		AdditionalKeyArrayAES<unLine, unCounter> extra {};

		for (unsigned char i = 0; i < 32; ++i) {
			out[i] = static_cast<unsigned char>(kBaseKey[i] ^ extra.m_Data[(i * 7) & 63] ^ extra.m_Data[(i * 13 + 5) & 63]);
		}
	}

	template<unsigned long long unLine, unsigned long long unCounter>
	constexpr void BuildGCMIV(unsigned char iv[12]) noexcept {
		constexpr unsigned long long unX = hash64(0xA1B2C3D4E5F60789ULL ^ (unLine * 0x9E3779B97F4A7C15ULL) ^ (unCounter << 1));
		constexpr unsigned long long unY = hash64(0xCAFEBABEDEADBEEFULL ^ (unX ^ (unLine << 17)));

		iv[ 0] = static_cast<unsigned char>((unX >> 56) & 0xFF);
		iv[ 1] = static_cast<unsigned char>((unX >> 48) & 0xFF);
		iv[ 2] = static_cast<unsigned char>((unX >> 40) & 0xFF);
		iv[ 3] = static_cast<unsigned char>((unX >> 32) & 0xFF);
		iv[ 4] = static_cast<unsigned char>((unX >> 24) & 0xFF);
		iv[ 5] = static_cast<unsigned char>((unX >> 16) & 0xFF);
		iv[ 6] = static_cast<unsigned char>((unX >>  8) & 0xFF);
		iv[ 7] = static_cast<unsigned char>( unX        & 0xFF);
		iv[ 8] = static_cast<unsigned char>((unY >> 24) & 0xFF);
		iv[ 9] = static_cast<unsigned char>((unY >> 16) & 0xFF);
		iv[10] = static_cast<unsigned char>((unY >>  8) & 0xFF);
		iv[11] = static_cast<unsigned char>( unY        & 0xFF);
	}

	template<typename T, std::size_t N>
	struct ByteIO;

	template<typename T>
	struct ByteIO<T, 1> {
		constexpr static std::array<unsigned char, 1> to(T Value) noexcept {
			std::array<unsigned char, 1> out {{ static_cast<unsigned char>(Value) }};
			return out;
		}

		constexpr static T from(const std::array<unsigned char, 1>& bytes) noexcept {
			return static_cast<T>(bytes[0]);
		}
	};

	template<typename T>
	struct ByteIO<T, 2> {
		constexpr static std::array<unsigned char, 2> to(T Value) noexcept {
			const unsigned short unX = static_cast<unsigned short>(Value);

			std::array<unsigned char, 2> out {{
				static_cast<unsigned char>( unX       & 0xFF),
				static_cast<unsigned char>((unX >> 8) & 0xFF)
			}};

			return out;
		}

		constexpr static T from(const std::array<unsigned char, 2>& bytes) noexcept {
			const unsigned short unX = static_cast<unsigned short>(bytes[0]) |
									  (static_cast<unsigned short>(bytes[1]) << 8);
			return static_cast<T>(unX);
		}
	};

	template<typename T>
	struct ByteIO<T, 4> {
		constexpr static std::array<unsigned char, 4> to(T Value) noexcept {
			const unsigned int unX = static_cast<unsigned int>(Value);

			std::array<unsigned char, 4> out {{
				static_cast<unsigned char>( unX        & 0xFF),
				static_cast<unsigned char>((unX >>  8) & 0xFF),
				static_cast<unsigned char>((unX >> 16) & 0xFF),
				static_cast<unsigned char>((unX >> 24) & 0xFF)
			}};

			return out;
		}

		constexpr static T from(const std::array<unsigned char, 4>& bytes) noexcept {
			const unsigned int unX = static_cast<unsigned int>(bytes[0])        |
									(static_cast<unsigned int>(bytes[1]) <<  8) |
									(static_cast<unsigned int>(bytes[2]) << 16) |
									(static_cast<unsigned int>(bytes[3]) << 24);
			return static_cast<T>(unX);
		}
	};

	template <typename T>
	constexpr std::array<unsigned char, sizeof(T)> ToBytes(T Value) noexcept {
		static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "Unsupported character size");
		return ByteIO<T, sizeof(T)>::to(Value);
	}

	template <typename T>
	constexpr T FromBytes(const std::array<unsigned char, sizeof(T)>& bytes) noexcept {
		static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4, "Unsupported character size");
		return ByteIO<T, sizeof(T)>::from(bytes);
	}

	template <unsigned long long unLength, typename T, unsigned long long unLine = 0, unsigned long long unCounter = 0>
	class CryptoStringAES {
	private:
		static constexpr std::size_t kLength = static_cast<std::size_t>(unLength);
		static constexpr std::size_t kPlainBytes = kLength * sizeof(T);

		struct EncryptedBlob {
			unsigned char m_IV[12] {};
			unsigned char m_CipherText[kPlainBytes] {};
			unsigned char m_Tag[16] {};
		};

		static constexpr EncryptedBlob MakeBlob(const T* pData) noexcept {
			std::array<unsigned char, 32> rawkey {};
			BuildAESKey<unLine, unCounter>(rawkey.data());

			std::array<unsigned char, 12> iv {};
			BuildGCMIV<unLine, unCounter>(iv.data());

			block32 key {};
			for (unsigned char i = 0; i < 32; ++i) {
				key[i] = rawkey[i];
			}

			std::array<unsigned char, kPlainBytes> plaintext {};
			for (std::size_t i = 0; i < kLength; ++i) {
				auto bytes = ToBytes<T>(pData[i]);

				for (std::size_t b = 0; b < sizeof(T); ++b) {
					plaintext[i * sizeof(T) + b] = bytes[b];
				}
			}

			const auto encrypted = GCMEncrypt<kPlainBytes, kAAD.size()>(plaintext.data(), kAAD.data(), key, iv);

			EncryptedBlob blob {};
			
			for (unsigned char i = 0; i < 12; ++i) {
				blob.m_IV[i] = iv[i];
			}

			for (std::size_t i = 0; i < kPlainBytes; ++i) {
				blob.m_CipherText[i] = encrypted.m_CipherText[i];
			}

			for (unsigned char i = 0; i < 16; ++i) {
				blob.m_Tag[i] = encrypted.m_Tag[i];
			}

			return blob;
		}

		static constexpr EncryptedBlob kBlobFrom(const T* pData) noexcept { return MakeBlob(pData); }

	public:
		class DecryptedString {
		public:
			_CRYPTOSTRINGAES_FORCE_INLINE explicit DecryptedString(const CryptoStringAES& self) noexcept {
				std::array<unsigned char, 32> rawkey {};
				BuildAESKey<unLine, unCounter>(rawkey.data());

				block32 key{};
				for (unsigned char i = 0; i < 32; ++i) {
					key[i] = rawkey[i];
				}

				std::array<unsigned char, 12> iv {};
				for (unsigned char i = 0; i < 12; ++i) {
					iv[i] = self.m_EncryptedBlob.m_IV[i];
				}

				block16 tag {};
				for (unsigned char i = 0; i < 16; ++i) {
					tag[i] = self.m_EncryptedBlob.m_Tag[i];
				}

				std::array<unsigned char, kPlainBytes> plaintext {};
				if (!GCMDecrypt(self.m_EncryptedBlob.m_CipherText, kPlainBytes, kAAD.data(), kAAD.size(), tag, key, iv, plaintext.data())) {
					Clear();
					return;
				}

				for (std::size_t i = 0; i < kLength; ++i) {
					std::array<unsigned char, sizeof(T)> bytes {};
					for (std::size_t j = 0; j < sizeof(T); ++j) {
						bytes[j] = plaintext[i * sizeof(T) + j];
					}

					m_pBuffer[i] = FromBytes<T>(bytes);
				}
			}

			_CRYPTOSTRINGAES_FORCE_INLINE ~DecryptedString() noexcept {
				Clear();
			}

			DecryptedString(const DecryptedString&) = delete;
			DecryptedString& operator=(const DecryptedString&) = delete;

			_CRYPTOSTRINGAES_FORCE_INLINE DecryptedString(DecryptedString&& other) noexcept {
				for (std::size_t i = 0; i < kLength; ++i) {
					m_pBuffer[i] = other.m_pBuffer[i];
				}

				other.Clear();
			}

			_CRYPTOSTRINGAES_FORCE_INLINE DecryptedString& operator=(DecryptedString&& other) noexcept {
				if (this != &other) {
					for (std::size_t i = 0; i < kLength; ++i) {
						m_pBuffer[i] = other.m_pBuffer[i];
					}

					other.Clear();
				}

				return *this;
			}

			_CRYPTOSTRINGAES_FORCE_INLINE T* get() noexcept { return m_pBuffer; }
			_CRYPTOSTRINGAES_FORCE_INLINE operator T* () noexcept { return get(); }
			_CRYPTOSTRINGAES_FORCE_INLINE const T* c_str() const noexcept { return m_pBuffer; }
			_CRYPTOSTRINGAES_FORCE_INLINE operator const T* () const noexcept { return c_str(); }

		private:
			_CRYPTOSTRINGAES_FORCE_INLINE void Clear() noexcept {
				volatile T* pData = m_pBuffer;
				for (std::size_t i = 0; i < kLength; ++i) {
					pData[i] = T {};
				}
			}

			T m_pBuffer[kLength] {};
		};

		_CRYPTOSTRINGAES_FORCE_INLINE constexpr CryptoStringAES(T* pData) noexcept : m_EncryptedBlob(kBlobFrom(pData)) {}
		_CRYPTOSTRINGAES_FORCE_INLINE DecryptedString Decrypt() const noexcept { return DecryptedString(*this); }

	private:
		EncryptedBlob m_EncryptedBlob {};
	};
}

#define _CRYPTOSTRINGAES(S)                                                                                                                                                                                     \
	([]() -> auto {                                                                                                                                                                                             \
		constexpr std::size_t unLength = std::extent_v<std::remove_reference_t<decltype(S)>>;                                                                                                                   \
		constexpr auto Encrypted = CryptoStringAES::CryptoStringAES<unLength, CryptoStringAES::clean_type<decltype(S[0])>, __LINE__, __COUNTER__>(const_cast<CryptoStringAES::clean_type<decltype(S[0])>*>(S)); \
		return Encrypted.Decrypt();                                                                                                                                                                             \
	} ())

#define CRYPTOSTRINGAES(S) _CRYPTOSTRINGAES(S)

#undef _CRYPTOSTRINGAES_FORCE_INLINE
#undef _CRYPTOSTRINGAES_NO_INLINE

#endif // _COMPILECRYPTOSTRINGAES_H_
