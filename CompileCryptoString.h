#pragma once

#ifndef _COMPILECRYPTOSTRING_H_
#define _COMPILECRYPTOSTRING_H_

// STL
#include <type_traits>

// CompileTimeStamp
#define COMPILETIMESTAMP_USE_64BIT
#include "CompileTimeStamp.h"

// ----------------------------------------------------------------
// General definitions
// ----------------------------------------------------------------

#if defined(_MSC_VER)
#define _CRYPTOSTRING_NO_INLINE __declspec(noinline)
#define _CRYPTOSTRING_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define _CRYPTOSTRING_NO_INLINE __attribute__((noinline))
#define _CRYPTOSTRING_FORCE_INLINE __attribute__((always_inline))
#else
#define _CRYPTOSTRING_NO_INLINE
#define _CRYPTOSTRING_FORCE_INLINE inline
#endif

// ----------------------------------------------------------------
// CryptoString
// ----------------------------------------------------------------

namespace CryptoString {

	template<class T>
	using clean_type = std::remove_const_t<std::remove_reference_t<T>>;

	constexpr unsigned int rotl32(unsigned int unX, unsigned int unN) noexcept {
		return (unX << unN) | (unX >> (32 - unN));
	}

	constexpr unsigned long long rotr64(unsigned long long unX, unsigned int unN) noexcept {
		return (unX >> unN) | (unX << (64 - unN));
	}

	constexpr unsigned long long hash64(unsigned long long unX) noexcept {
		unX ^= unX >> 33;
		unX *= 0xFF51AFD7ED558CCDULL;
		unX ^= unX >> 33;
		unX *= 0xC4CEB9FE1A85EC53ULL;
		unX ^= unX >> 33;
		return unX;
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

	constexpr static unsigned char kBaseKey[32] = {
		static_cast<unsigned char>( s6.s0        & 0xFF),
		static_cast<unsigned char>((s6.s0 >>  8) & 0xFF),
		static_cast<unsigned char>((s6.s0 >> 16) & 0xFF),
		static_cast<unsigned char>((s6.s0 >> 24) & 0xFF),
		static_cast<unsigned char>( s6.s1        & 0xFF),
		static_cast<unsigned char>((s6.s1 >>  8) & 0xFF),
		static_cast<unsigned char>((s6.s1 >> 16) & 0xFF),
		static_cast<unsigned char>((s6.s1 >> 24) & 0xFF),
		static_cast<unsigned char>( s6.s2        & 0xFF),
		static_cast<unsigned char>((s6.s2 >>  8) & 0xFF),
		static_cast<unsigned char>((s6.s2 >> 16) & 0xFF),
		static_cast<unsigned char>((s6.s2 >> 24) & 0xFF),
		static_cast<unsigned char>( s6.s3        & 0xFF),
		static_cast<unsigned char>((s6.s3 >>  8) & 0xFF),
		static_cast<unsigned char>((s6.s3 >> 16) & 0xFF),
		static_cast<unsigned char>((s6.s3 >> 24) & 0xFF),
		static_cast<unsigned char>( s7.s0        & 0xFF),
		static_cast<unsigned char>((s7.s0 >>  8) & 0xFF),
		static_cast<unsigned char>((s7.s0 >> 16) & 0xFF),
		static_cast<unsigned char>((s7.s0 >> 24) & 0xFF),
		static_cast<unsigned char>( s7.s1        & 0xFF),
		static_cast<unsigned char>((s7.s1 >>  8) & 0xFF),
		static_cast<unsigned char>((s7.s1 >> 16) & 0xFF),
		static_cast<unsigned char>((s7.s1 >> 24) & 0xFF),
		static_cast<unsigned char>( s7.s2        & 0xFF),
		static_cast<unsigned char>((s7.s2 >>  8) & 0xFF),
		static_cast<unsigned char>((s7.s2 >> 16) & 0xFF),
		static_cast<unsigned char>((s7.s2 >> 24) & 0xFF),
		static_cast<unsigned char>( s7.s3        & 0xFF),
		static_cast<unsigned char>((s7.s3 >>  8) & 0xFF),
		static_cast<unsigned char>((s7.s3 >> 16) & 0xFF),
		static_cast<unsigned char>((s7.s3 >> 24) & 0xFF)
	};

	template<unsigned long long unLine, unsigned long long unCounter>
	class AdditionalKeyArray {
	public:
		constexpr AdditionalKeyArray() noexcept {
			constexpr unsigned long long unA = hash64(rotr64(unLine, 13));
			constexpr unsigned long long unB = hash64(rotr64(unA ^ unCounter, 31));
			constexpr unsigned long long unC = hash64(rotr64(unB + unLine, 17));
			constexpr unsigned long long unD = hash64(rotr64(unC ^ unCounter, 47));
			constexpr unsigned long long unE = hash64(rotr64(unD ^ unLine, 23));
			constexpr unsigned long long unF = hash64(rotr64(unE + unCounter, 37));

			constexpr unsigned long long kSelection[6] = { unA, unB, unC, unD, unE, unF };

			for (unsigned char i = 0; i < 8; ++i) {
				const unsigned long long unX = (i < 5) ? kSelection[i] : (unF * i);
				const unsigned char unBase = static_cast<unsigned char>(i << 3);

				m_Data[unBase]     = static_cast<unsigned char>( unX        & 0xFF);
				m_Data[unBase + 1] = static_cast<unsigned char>((unX >>  8) & 0xFF);
				m_Data[unBase + 2] = static_cast<unsigned char>((unX >> 16) & 0xFF);
				m_Data[unBase + 3] = static_cast<unsigned char>((unX >> 24) & 0xFF);
				m_Data[unBase + 4] = static_cast<unsigned char>((unX >> 32) & 0xFF);
				m_Data[unBase + 5] = static_cast<unsigned char>((unX >> 40) & 0xFF);
				m_Data[unBase + 6] = static_cast<unsigned char>((unX >> 48) & 0xFF);
				m_Data[unBase + 7] = static_cast<unsigned char>((unX >> 56) & 0xFF);
			}
		}

	public:
		unsigned char m_Data[64] {};
	};

	template<typename T, std::size_t N>
	struct ByteIO;

	template<typename T>
	struct ByteIO<T, 1> {
		static constexpr void to(T Value, unsigned char(&out)[1]) noexcept {
			out[0] = static_cast<unsigned char>(Value);
		}

		static constexpr T from(const unsigned char(&in)[1]) noexcept {
			return static_cast<T>(in[0]);
		}
	};

	template<typename T>
	struct ByteIO<T, 2> {
		static constexpr void to(T Value, unsigned char(&out)[2]) noexcept {
			const unsigned short unX = static_cast<unsigned short>(Value);
			out[0] = static_cast<unsigned char>( unX       & 0xFF);
			out[1] = static_cast<unsigned char>((unX >> 8) & 0xFF);
		}

		static constexpr T from(const unsigned char(&in)[2]) noexcept {
			const unsigned short unX = static_cast<unsigned short>(in[0]) |
									  (static_cast<unsigned short>(in[1]) << 8);
			return static_cast<T>(unX);
		}
	};

	template<typename T>
	struct ByteIO<T, 4> {
		static constexpr void to(T Value, unsigned char(&out)[4]) noexcept {
			const unsigned int unX = static_cast<unsigned int>(Value);
			out[0] = static_cast<unsigned char>( unX        & 0xFF);
			out[1] = static_cast<unsigned char>((unX >>  8) & 0xFF);
			out[2] = static_cast<unsigned char>((unX >> 16) & 0xFF);
			out[3] = static_cast<unsigned char>((unX >> 24) & 0xFF);
		}

		static constexpr T from(const unsigned char(&in)[4]) noexcept {
			const unsigned int unX = static_cast<unsigned int>(in[0])        |
									(static_cast<unsigned int>(in[1]) <<  8) |
									(static_cast<unsigned int>(in[2]) << 16) |
									(static_cast<unsigned int>(in[3]) << 24);
			return static_cast<T>(unX);
		}
	};

	template <unsigned long long unLength, typename T, unsigned long long unLine = 0, unsigned long long unCounter = 0>
	class CryptoString {
	private:
		static constexpr std::size_t kLength = static_cast<std::size_t>(unLength);
		static constexpr std::size_t kPlainBytes = kLength * sizeof(T);

	public:
		class DecryptedString {
		public:
			_CRYPTOSTRING_FORCE_INLINE explicit DecryptedString(const CryptoString& EncryptedString) noexcept {
				AdditionalKeyArray<unLine, unCounter> AdditionalKey {};

				for (std::size_t i = 0; i < kLength; ++i) {
					unsigned char tmp[sizeof(T)] {};

					for (std::size_t k = 0; k < sizeof(T); ++k) {
						const std::size_t j = i * sizeof(T) + k;
						tmp[k] = EncryptedString.m_Storage[j] ^ kBaseKey[j % sizeof(kBaseKey)] ^ AdditionalKey.m_Data[j % sizeof(AdditionalKey.m_Data)];
					}

					m_Buffer[i] = ByteIO<T, sizeof(T)>::from(tmp);
				}
			}

			_CRYPTOSTRING_FORCE_INLINE ~DecryptedString() noexcept {
				Clear();
			}

			DecryptedString(const DecryptedString&) = delete;
			DecryptedString& operator=(const DecryptedString&) = delete;

			_CRYPTOSTRING_FORCE_INLINE DecryptedString(DecryptedString&& other) noexcept {
				for (std::size_t i = 0; i < kLength; ++i) {
					m_Buffer[i] = other.m_Buffer[i];
				}

				other.Clear();
			}

			_CRYPTOSTRING_FORCE_INLINE DecryptedString& operator=(DecryptedString&& other) noexcept {
				if (this != &other) {
					for (std::size_t i = 0; i < kLength; ++i) {
						m_Buffer[i] = other.m_Buffer[i];
					}

					other.Clear();
				}

				return *this;
			}

			_CRYPTOSTRING_FORCE_INLINE T* get() noexcept { return m_Buffer; }
			_CRYPTOSTRING_FORCE_INLINE const T* c_str() const noexcept { return m_Buffer; }

			_CRYPTOSTRING_FORCE_INLINE operator T* () noexcept { return get(); }
			_CRYPTOSTRING_FORCE_INLINE operator const T* () const noexcept { return c_str(); }

		private:
			_CRYPTOSTRING_FORCE_INLINE void Clear() noexcept {
				volatile T* pData = m_Buffer;
				for (std::size_t i = 0; i < kLength; ++i) {
					pData[i] = T {};
				}
			}

			T m_Buffer[kLength] {};
		};

		_CRYPTOSTRING_FORCE_INLINE constexpr CryptoString(const T* pData) noexcept {
			using CT = clean_type<T>;
			static_assert(std::is_trivially_copyable_v<CT>, "T must be trivially copyable");
			static_assert(std::is_integral_v<CT> || std::is_enum_v<CT>, "T must be integral or enum");
			static_assert((sizeof(CT) == 1) || (sizeof(CT) == 2) || (sizeof(CT) == 4), "Supported sizes: 1/2/4 bytes");

			AdditionalKeyArray<unLine, unCounter> AdditionalKey {};

			for (std::size_t i = 0; i < kLength; ++i) {
				unsigned char bytes[sizeof(T)] {};
				ByteIO<T, sizeof(T)>::to(pData[i], bytes);

				for (std::size_t k = 0; k < sizeof(T); ++k) {
					const std::size_t j = i * sizeof(T) + k;
					m_Storage[j] = static_cast<unsigned char>(bytes[k] ^ kBaseKey[j % sizeof(kBaseKey)] ^ AdditionalKey.m_Data[j % sizeof(AdditionalKey.m_Data)]);
				}
			}
		}

		_CRYPTOSTRING_FORCE_INLINE DecryptedString Decrypt() const noexcept {
			return DecryptedString(*this);
		}

	private:
		unsigned char m_Storage[kPlainBytes] {};
	};
}

#define _CRYPTOSTRING(STRING)                                                                                                                              \
	([]() -> auto {                                                                                                                                        \
		constexpr size_t unLength = std::extent_v<std::remove_reference_t<decltype(STRING)>>;                                                              \
		constexpr auto Encrypted = CryptoString::CryptoString<unLength, CryptoString::clean_type<decltype((STRING)[0])>, __LINE__, __COUNTER__>((STRING)); \
		return Encrypted.Decrypt();                                                                                                                        \
	} ())

#define CRYPTOSTRING(STRING) _CRYPTOSTRING(STRING)

#undef _CRYPTOSTRING_FORCE_INLINE
#undef _CRYPTOSTRING_NO_INLINE

#endif // !_COMPILECRYPTOSTRING_H_
