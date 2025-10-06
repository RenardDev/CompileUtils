#pragma once

#ifndef _COMPILECRYPTOARRAY_H_
#define _COMPILECRYPTOARRAY_H_

// STL
#include <type_traits>
#include <array>

// CompileTimeStamp
#define COMPILETIMESTAMP_USE_64BIT
#include "CompileTimeStamp.h"

// ----------------------------------------------------------------
// General definitions
// ----------------------------------------------------------------

#if defined(_MSC_VER)
#define _CRYPOARRAY_NO_INLINE __declspec(noinline)
#define _CRYPOARRAY_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define _CRYPOARRAY_NO_INLINE __attribute__((noinline))
#define _CRYPOARRAY_FORCE_INLINE __attribute__((always_inline))
#else
#define _CRYPOARRAY_NO_INLINE
#define _CRYPOARRAY_FORCE_INLINE inline
#endif

// ----------------------------------------------------------------
// CryptoArray
// ----------------------------------------------------------------

namespace CryptoArray {

	template<class _Ty>
	using clean_type = typename std::remove_const_t<std::remove_reference_t<_Ty>>;

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
	class AdditionalKeyArray {
	public:
		constexpr AdditionalKeyArray() noexcept {
			constexpr unsigned long long unA = hash64(rotr64(unLine, 13));
			constexpr unsigned long long unB = hash64(rotr64(unA ^ unCounter, 31));
			constexpr unsigned long long unC = hash64(rotr64(unB + unLine, 17));
			constexpr unsigned long long unD = hash64(rotr64(unC ^ unCounter, 47));
			constexpr unsigned long long unE = hash64(rotr64(unD ^ unLine, 23));
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
	class CryptoArray {
	private:
		static constexpr std::size_t kLength = static_cast<std::size_t>(unLength);
		static constexpr std::size_t kPlainBytes = kLength * sizeof(T);

	public:
		class DecryptedArray {
		public:
			_CRYPOARRAY_FORCE_INLINE explicit DecryptedArray(const CryptoArray& Encrypted) noexcept {
				AdditionalKeyArray<unLine, unCounter> AdditionalKey{};
				for (std::size_t i = 0; i < kLength; ++i) {
					std::array<unsigned char, sizeof(T)> tmp {};
					for (std::size_t k = 0; k < sizeof(T); ++k) {
						const std::size_t j = i * sizeof(T) + k;
						tmp[k] = Encrypted.m_pStorage[j] ^ kBaseKey[j % sizeof(kBaseKey)] ^ AdditionalKey.m_Data[j % sizeof(AdditionalKey.m_Data)];
					}

					m_pBuffer[i] = FromBytes<T>(tmp);
				}
			}

			_CRYPOARRAY_FORCE_INLINE ~DecryptedArray() noexcept {
				Clear();
			}

			DecryptedArray(const DecryptedArray&) = delete;
			DecryptedArray& operator=(const DecryptedArray&) = delete;

			_CRYPOARRAY_FORCE_INLINE DecryptedArray(DecryptedArray&& other) noexcept {
				for (std::size_t i = 0; i < kLength; ++i) {
					m_pBuffer[i] = other.m_pBuffer[i];
				}

				other.Clear();
			}

			_CRYPOARRAY_FORCE_INLINE DecryptedArray& operator=(DecryptedArray&& other) noexcept {
				if (this != &other) {
					for (std::size_t i = 0; i < kLength; ++i) {
						m_pBuffer[i] = other.m_pBuffer[i];
					}

					other.Clear();
				}

				return *this;
			}

			_CRYPOARRAY_FORCE_INLINE T* data() noexcept { return m_pBuffer; }
			_CRYPOARRAY_FORCE_INLINE const T* data() const noexcept { return m_pBuffer; }
			_CRYPOARRAY_FORCE_INLINE std::size_t size() const noexcept { return kLength; }
			_CRYPOARRAY_FORCE_INLINE T& operator[](std::size_t idx) noexcept { return m_pBuffer[idx]; }
			_CRYPOARRAY_FORCE_INLINE const T& operator[](std::size_t idx) const noexcept { return m_pBuffer[idx]; }
			_CRYPOARRAY_FORCE_INLINE T* begin() noexcept { return m_pBuffer; }
			_CRYPOARRAY_FORCE_INLINE T* end() noexcept { return m_pBuffer + kLength; }
			_CRYPOARRAY_FORCE_INLINE const T* begin() const noexcept { return m_pBuffer; }
			_CRYPOARRAY_FORCE_INLINE const T* end() const noexcept { return m_pBuffer + kLength; }

			_CRYPOARRAY_FORCE_INLINE operator T* () noexcept { return data(); }
			_CRYPOARRAY_FORCE_INLINE operator const T* () const noexcept { return data(); }

		private:
			_CRYPOARRAY_FORCE_INLINE void Clear() noexcept {
				volatile T* pData = m_pBuffer;
				for (std::size_t i = 0; i < kLength; ++i) {
					pData[i] = T {};
				}
			}

			T m_pBuffer[kLength] {};
		};

		_CRYPOARRAY_FORCE_INLINE constexpr CryptoArray(const T* pData) noexcept {
			static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable for bytestream obfuscation");
			AdditionalKeyArray<unLine, unCounter> AdditionalKey{};
			for (std::size_t i = 0; i < kLength; ++i) {
				const auto bytes = ToBytes<T>(pData[i]);
				for (std::size_t k = 0; k < sizeof(T); ++k) {
					const std::size_t j = i * sizeof(T) + k;
					m_pStorage[j] = static_cast<unsigned char>(bytes[k] ^ kBaseKey[j % sizeof(kBaseKey)] ^ AdditionalKey.m_Data[j % sizeof(AdditionalKey.m_Data)]);
				}
			}
		}

		_CRYPOARRAY_FORCE_INLINE DecryptedArray Decrypt() const noexcept {
			return DecryptedArray(*this);
		}

	private:
		unsigned char m_pStorage[kPlainBytes] {};
	};

	template<unsigned long long unLine, unsigned long long unCounter, typename T, std::size_t N>
	_CRYPOARRAY_FORCE_INLINE constexpr auto MakeCryptoArray(const T(&arr)[N]) noexcept {
		return CryptoArray<N, T, unLine, unCounter>(arr);
	}

	template<unsigned long long unLine, unsigned long long unCounter, typename T, std::size_t N>
	_CRYPOARRAY_FORCE_INLINE constexpr auto MakeCryptoArray(const std::array<T, N>& arr) noexcept {
		return CryptoArray<N, T, unLine, unCounter>(arr.data());
	}
}

#define _CRYPTOARRAY(A)                                                                    \
	([&]() -> auto {                                                                       \
		constexpr auto Encrypted = CryptoArray::MakeCryptoArray<__LINE__, __COUNTER__>(A); \
		return Encrypted.Decrypt();                                                        \
	} ())

#define CRYPTOARRAY(A) _CRYPTOARRAY(A)

#undef _CRYPOARRAY_FORCE_INLINE
#undef _CRYPOARRAY_NO_INLINE

#endif // !_COMPILECRYPTOARRAY_H_
