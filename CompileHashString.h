#pragma once

#ifndef _COMPILEHASHSTRING_H_
#define _COMPILEHASHSTRING_H_

// STL
#include <type_traits>
#include <array>
#include <string>

// ----------------------------------------------------------------
// CRC
// ----------------------------------------------------------------

namespace CRC {

	constexpr static const unsigned long long kCRC64Table[256] = {
		0x0000000000000000ull, 0x7AD870C830358979ull, 0xF5B0E190606B12F2ull, 0x8F689158505E9B8Bull,
		0xC038E5739841B68Full, 0xBAE095BBA8743FF6ull, 0x358804E3F82AA47Dull, 0x4F50742BC81F2D04ull,
		0xAB28ECB46814FE75ull, 0xD1F09C7C5821770Cull, 0x5E980D24087FEC87ull, 0x24407DEC384A65FEull,
		0x6B1009C7F05548FAull, 0x11C8790FC060C183ull, 0x9EA0E857903E5A08ull, 0xE478989FA00BD371ull,
		0x7D08FF3B88BE6F81ull, 0x07D08FF3B88BE6F8ull, 0x88B81EABE8D57D73ull, 0xF2606E63D8E0F40Aull,
		0xBD301A4810FFD90Eull, 0xC7E86A8020CA5077ull, 0x4880FBD87094CBFCull, 0x32588B1040A14285ull,
		0xD620138FE0AA91F4ull, 0xACF86347D09F188Dull, 0x2390F21F80C18306ull, 0x594882D7B0F40A7Full,
		0x1618F6FC78EB277Bull, 0x6CC0863448DEAE02ull, 0xE3A8176C18803589ull, 0x997067A428B5BCF0ull,
		0xFA11FE77117CDF02ull, 0x80C98EBF2149567Bull, 0x0FA11FE77117CDF0ull, 0x75796F2F41224489ull,
		0x3A291B04893D698Dull, 0x40F16BCCB908E0F4ull, 0xCF99FA94E9567B7Full, 0xB5418A5CD963F206ull,
		0x513912C379682177ull, 0x2BE1620B495DA80Eull, 0xA489F35319033385ull, 0xDE51839B2936BAFCull,
		0x9101F7B0E12997F8ull, 0xEBD98778D11C1E81ull, 0x64B116208142850Aull, 0x1E6966E8B1770C73ull,
		0x8719014C99C2B083ull, 0xFDC17184A9F739FAull, 0x72A9E0DCF9A9A271ull, 0x08719014C99C2B08ull,
		0x4721E43F0183060Cull, 0x3DF994F731B68F75ull, 0xB29105AF61E814FEull, 0xC849756751DD9D87ull,
		0x2C31EDF8F1D64EF6ull, 0x56E99D30C1E3C78Full, 0xD9810C6891BD5C04ull, 0xA3597CA0A188D57Dull,
		0xEC09088B6997F879ull, 0x96D1784359A27100ull, 0x19B9E91B09FCEA8Bull, 0x636199D339C963F2ull,
		0xDF7ADABD7A6E2D6Full, 0xA5A2AA754A5BA416ull, 0x2ACA3B2D1A053F9Dull, 0x50124BE52A30B6E4ull,
		0x1F423FCEE22F9BE0ull, 0x659A4F06D21A1299ull, 0xEAF2DE5E82448912ull, 0x902AAE96B271006Bull,
		0x74523609127AD31Aull, 0x0E8A46C1224F5A63ull, 0x81E2D7997211C1E8ull, 0xFB3AA75142244891ull,
		0xB46AD37A8A3B6595ull, 0xCEB2A3B2BA0EECECull, 0x41DA32EAEA507767ull, 0x3B024222DA65FE1Eull,
		0xA2722586F2D042EEull, 0xD8AA554EC2E5CB97ull, 0x57C2C41692BB501Cull, 0x2D1AB4DEA28ED965ull,
		0x624AC0F56A91F461ull, 0x1892B03D5AA47D18ull, 0x97FA21650AFAE693ull, 0xED2251AD3ACF6FEAull,
		0x095AC9329AC4BC9Bull, 0x7382B9FAAAF135E2ull, 0xFCEA28A2FAAFAE69ull, 0x8632586ACA9A2710ull,
		0xC9622C4102850A14ull, 0xB3BA5C8932B0836Dull, 0x3CD2CDD162EE18E6ull, 0x460ABD1952DB919Full,
		0x256B24CA6B12F26Dull, 0x5FB354025B277B14ull, 0xD0DBC55A0B79E09Full, 0xAA03B5923B4C69E6ull,
		0xE553C1B9F35344E2ull, 0x9F8BB171C366CD9Bull, 0x10E3202993385610ull, 0x6A3B50E1A30DDF69ull,
		0x8E43C87E03060C18ull, 0xF49BB8B633338561ull, 0x7BF329EE636D1EEAull, 0x012B592653589793ull,
		0x4E7B2D0D9B47BA97ull, 0x34A35DC5AB7233EEull, 0xBBCBCC9DFB2CA865ull, 0xC113BC55CB19211Cull,
		0x5863DBF1E3AC9DECull, 0x22BBAB39D3991495ull, 0xADD33A6183C78F1Eull, 0xD70B4AA9B3F20667ull,
		0x985B3E827BED2B63ull, 0xE2834E4A4BD8A21Aull, 0x6DEBDF121B863991ull, 0x1733AFDA2BB3B0E8ull,
		0xF34B37458BB86399ull, 0x8993478DBB8DEAE0ull, 0x06FBD6D5EBD3716Bull, 0x7C23A61DDBE6F812ull,
		0x3373D23613F9D516ull, 0x49ABA2FE23CC5C6Full, 0xC6C333A67392C7E4ull, 0xBC1B436E43A74E9Dull,
		0x95AC9329AC4BC9B5ull, 0xEF74E3E19C7E40CCull, 0x601C72B9CC20DB47ull, 0x1AC40271FC15523Eull,
		0x5594765A340A7F3Aull, 0x2F4C0692043FF643ull, 0xA02497CA54616DC8ull, 0xDAFCE7026454E4B1ull,
		0x3E847F9DC45F37C0ull, 0x445C0F55F46ABEB9ull, 0xCB349E0DA4342532ull, 0xB1ECEEC59401AC4Bull,
		0xFEBC9AEE5C1E814Full, 0x8464EA266C2B0836ull, 0x0B0C7B7E3C7593BDull, 0x71D40BB60C401AC4ull,
		0xE8A46C1224F5A634ull, 0x927C1CDA14C02F4Dull, 0x1D148D82449EB4C6ull, 0x67CCFD4A74AB3DBFull,
		0x289C8961BCB410BBull, 0x5244F9A98C8199C2ull, 0xDD2C68F1DCDF0249ull, 0xA7F41839ECEA8B30ull,
		0x438C80A64CE15841ull, 0x3954F06E7CD4D138ull, 0xB63C61362C8A4AB3ull, 0xCCE411FE1CBFC3CAull,
		0x83B465D5D4A0EECEull, 0xF96C151DE49567B7ull, 0x76048445B4CBFC3Cull, 0x0CDCF48D84FE7545ull,
		0x6FBD6D5EBD3716B7ull, 0x15651D968D029FCEull, 0x9A0D8CCEDD5C0445ull, 0xE0D5FC06ED698D3Cull,
		0xAF85882D2576A038ull, 0xD55DF8E515432941ull, 0x5A3569BD451DB2CAull, 0x20ED197575283BB3ull,
		0xC49581EAD523E8C2ull, 0xBE4DF122E51661BBull, 0x3125607AB548FA30ull, 0x4BFD10B2857D7349ull,
		0x04AD64994D625E4Dull, 0x7E7514517D57D734ull, 0xF11D85092D094CBFull, 0x8BC5F5C11D3CC5C6ull,
		0x12B5926535897936ull, 0x686DE2AD05BCF04Full, 0xE70573F555E26BC4ull, 0x9DDD033D65D7E2BDull,
		0xD28D7716ADC8CFB9ull, 0xA85507DE9DFD46C0ull, 0x273D9686CDA3DD4Bull, 0x5DE5E64EFD965432ull,
		0xB99D7ED15D9D8743ull, 0xC3450E196DA80E3Aull, 0x4C2D9F413DF695B1ull, 0x36F5EF890DC31CC8ull,
		0x79A59BA2C5DC31CCull, 0x037DEB6AF5E9B8B5ull, 0x8C157A32A5B7233Eull, 0xF6CD0AFA9582AA47ull,
		0x4AD64994D625E4DAull, 0x300E395CE6106DA3ull, 0xBF66A804B64EF628ull, 0xC5BED8CC867B7F51ull,
		0x8AEEACE74E645255ull, 0xF036DC2F7E51DB2Cull, 0x7F5E4D772E0F40A7ull, 0x05863DBF1E3AC9DEull,
		0xE1FEA520BE311AAFull, 0x9B26D5E88E0493D6ull, 0x144E44B0DE5A085Dull, 0x6E963478EE6F8124ull,
		0x21C640532670AC20ull, 0x5B1E309B16452559ull, 0xD476A1C3461BBED2ull, 0xAEAED10B762E37ABull,
		0x37DEB6AF5E9B8B5Bull, 0x4D06C6676EAE0222ull, 0xC26E573F3EF099A9ull, 0xB8B627F70EC510D0ull,
		0xF7E653DCC6DA3DD4ull, 0x8D3E2314F6EFB4ADull, 0x0256B24CA6B12F26ull, 0x788EC2849684A65Full,
		0x9CF65A1B368F752Eull, 0xE62E2AD306BAFC57ull, 0x6946BB8B56E467DCull, 0x139ECB4366D1EEA5ull,
		0x5CCEBF68AECEC3A1ull, 0x2616CFA09EFB4AD8ull, 0xA97E5EF8CEA5D153ull, 0xD3A62E30FE90582Aull,
		0xB0C7B7E3C7593BD8ull, 0xCA1FC72BF76CB2A1ull, 0x45775673A732292Aull, 0x3FAF26BB9707A053ull,
		0x70FF52905F188D57ull, 0x0A2722586F2D042Eull, 0x854FB3003F739FA5ull, 0xFF97C3C80F4616DCull,
		0x1BEF5B57AF4DC5ADull, 0x61372B9F9F784CD4ull, 0xEE5FBAC7CF26D75Full, 0x9487CA0FFF135E26ull,
		0xDBD7BE24370C7322ull, 0xA10FCEEC0739FA5Bull, 0x2E675FB4576761D0ull, 0x54BF2F7C6752E8A9ull,
		0xCDCF48D84FE75459ull, 0xB71738107FD2DD20ull, 0x387FA9482F8C46ABull, 0x42A7D9801FB9CFD2ull,
		0x0DF7ADABD7A6E2D6ull, 0x772FDD63E7936BAFull, 0xF8474C3BB7CDF024ull, 0x829F3CF387F8795Dull,
		0x66E7A46C27F3AA2Cull, 0x1C3FD4A417C62355ull, 0x935745FC4798B8DEull, 0xE98F353477AD31A7ull,
		0xA6DF411FBFB21CA3ull, 0xDC0731D78F8795DAull, 0x536FA08FDFD90E51ull, 0x29B7D047EFEC8728ull
	};

	constexpr static const unsigned int kCRC32Table[256] = {
			0x00000000ul, 0x77073096ul, 0xEE0E612Cul, 0x990951BAul,
		0x076DC419ul, 0x706AF48Ful, 0xE963A535ul, 0x9E6495A3ul,
		0x0EDB8832ul, 0x79DCB8A4ul, 0xE0D5E91Eul, 0x97D2D988ul,
		0x09B64C2Bul, 0x7EB17CBDul, 0xE7B82D07ul, 0x90BF1D91ul,
		0x1DB71064ul, 0x6AB020F2ul, 0xF3B97148ul, 0x84BE41DEul,
		0x1ADAD47Dul, 0x6DDDE4EBul, 0xF4D4B551ul, 0x83D385C7ul,
		0x136C9856ul, 0x646BA8C0ul, 0xFD62F97Aul, 0x8A65C9ECul,
		0x14015C4Ful, 0x63066CD9ul, 0xFA0F3D63ul, 0x8D080DF5ul,
		0x3B6E20C8ul, 0x4C69105Eul, 0xD56041E4ul, 0xA2677172ul,
		0x3C03E4D1ul, 0x4B04D447ul, 0xD20D85FDul, 0xA50AB56Bul,
		0x35B5A8FAul, 0x42B2986Cul, 0xDBBBC9D6ul, 0xACBCF940ul,
		0x32D86CE3ul, 0x45DF5C75ul, 0xDCD60DCFul, 0xABD13D59ul,
		0x26D930ACul, 0x51DE003Aul, 0xC8D75180ul, 0xBFD06116ul,
		0x21B4F4B5ul, 0x56B3C423ul, 0xCFBA9599ul, 0xB8BDA50Ful,
		0x2802B89Eul, 0x5F058808ul, 0xC60CD9B2ul, 0xB10BE924ul,
		0x2F6F7C87ul, 0x58684C11ul, 0xC1611DABul, 0xB6662D3Dul,
		0x76DC4190ul, 0x01DB7106ul, 0x98D220BCul, 0xEFD5102Aul,
		0x71B18589ul, 0x06B6B51Ful, 0x9FBFE4A5ul, 0xE8B8D433ul,
		0x7807C9A2ul, 0x0F00F934ul, 0x9609A88Eul, 0xE10E9818ul,
		0x7F6A0DBBul, 0x086D3D2Dul, 0x91646C97ul, 0xE6635C01ul,
		0x6B6B51F4ul, 0x1C6C6162ul, 0x856530D8ul, 0xF262004Eul,
		0x6C0695EDul, 0x1B01A57Bul, 0x8208F4C1ul, 0xF50FC457ul,
		0x65B0D9C6ul, 0x12B7E950ul, 0x8BBEB8EAul, 0xFCB9887Cul,
		0x62DD1DDFul, 0x15DA2D49ul, 0x8CD37CF3ul, 0xFBD44C65ul,
		0x4DB26158ul, 0x3AB551CEul, 0xA3BC0074ul, 0xD4BB30E2ul,
		0x4ADFA541ul, 0x3DD895D7ul, 0xA4D1C46Dul, 0xD3D6F4FBul,
		0x4369E96Aul, 0x346ED9FCul, 0xAD678846ul, 0xDA60B8D0ul,
		0x44042D73ul, 0x33031DE5ul, 0xAA0A4C5Ful, 0xDD0D7CC9ul,
		0x5005713Cul, 0x270241AAul, 0xBE0B1010ul, 0xC90C2086ul,
		0x5768B525ul, 0x206F85B3ul, 0xB966D409ul, 0xCE61E49Ful,
		0x5EDEF90Eul, 0x29D9C998ul, 0xB0D09822ul, 0xC7D7A8B4ul,
		0x59B33D17ul, 0x2EB40D81ul, 0xB7BD5C3Bul, 0xC0BA6CADul,
		0xEDB88320ul, 0x9ABFB3B6ul, 0x03B6E20Cul, 0x74B1D29Aul,
		0xEAD54739ul, 0x9DD277AFul, 0x04DB2615ul, 0x73DC1683ul,
		0xE3630B12ul, 0x94643B84ul, 0x0D6D6A3Eul, 0x7A6A5AA8ul,
		0xE40ECF0Bul, 0x9309FF9Dul, 0x0A00AE27ul, 0x7D079EB1ul,
		0xF00F9344ul, 0x8708A3D2ul, 0x1E01F268ul, 0x6906C2FEul,
		0xF762575Dul, 0x806567CBul, 0x196C3671ul, 0x6E6B06E7ul,
		0xFED41B76ul, 0x89D32BE0ul, 0x10DA7A5Aul, 0x67DD4ACCul,
		0xF9B9DF6Ful, 0x8EBEEFF9ul, 0x17B7BE43ul, 0x60B08ED5ul,
		0xD6D6A3E8ul, 0xA1D1937Eul, 0x38D8C2C4ul, 0x4FDFF252ul,
		0xD1BB67F1ul, 0xA6BC5767ul, 0x3FB506DDul, 0x48B2364Bul,
		0xD80D2BDAul, 0xAF0A1B4Cul, 0x36034AF6ul, 0x41047A60ul,
		0xDF60EFC3ul, 0xA867DF55ul, 0x316E8EEFul, 0x4669BE79ul,
		0xCB61B38Cul, 0xBC66831Aul, 0x256FD2A0ul, 0x5268E236ul,
		0xCC0C7795ul, 0xBB0B4703ul, 0x220216B9ul, 0x5505262Ful,
		0xC5BA3BBEul, 0xB2BD0B28ul, 0x2BB45A92ul, 0x5CB36A04ul,
		0xC2D7FFA7ul, 0xB5D0CF31ul, 0x2CD99E8Bul, 0x5BDEAE1Dul,
		0x9B64C2B0ul, 0xEC63F226ul, 0x756AA39Cul, 0x026D930Aul,
		0x9C0906A9ul, 0xEB0E363Ful, 0x72076785ul, 0x05005713ul,
		0x95BF4A82ul, 0xE2B87A14ul, 0x7BB12BAEul, 0x0CB61B38ul,
		0x92D28E9Bul, 0xE5D5BE0Dul, 0x7CDCEFB7ul, 0x0BDBDF21ul,
		0x86D3D2D4ul, 0xF1D4E242ul, 0x68DDB3F8ul, 0x1FDA836Eul,
		0x81BE16CDul, 0xF6B9265Bul, 0x6FB077E1ul, 0x18B74777ul,
		0x88085AE6ul, 0xFF0F6A70ul, 0x66063BCAul, 0x11010B5Cul,
		0x8F659EFFul, 0xF862AE69ul, 0x616BFFD3ul, 0x166CCF45ul,
		0xA00AE278ul, 0xD70DD2EEul, 0x4E048354ul, 0x3903B3C2ul,
		0xA7672661ul, 0xD06016F7ul, 0x4969474Dul, 0x3E6E77DBul,
		0xAED16A4Aul, 0xD9D65ADCul, 0x40DF0B66ul, 0x37D83BF0ul,
		0xA9BCAE53ul, 0xDEBB9EC5ul, 0x47B2CF7Ful, 0x30B5FFE9ul,
		0xBDBDF21Cul, 0xCABAC28Aul, 0x53B39330ul, 0x24B4A3A6ul,
		0xBAD03605ul, 0xCDD70693ul, 0x54DE5729ul, 0x23D967BFul,
		0xB3667A2Eul, 0xC4614AB8ul, 0x5D681B02ul, 0x2A6F2B94ul,
		0xB40BBE37ul, 0xC30C8EA1ul, 0x5A05DF1Bul, 0x2D02EF8Dul
	};

	template<typename T, std::size_t N>
	struct ByteIO;

	template<typename T>
	struct ByteIO<T, 1> {
		constexpr static std::array<unsigned char, 1> to(T Value) noexcept {
			std::array<unsigned char, 1> out{ { static_cast<unsigned char>(Value) } };
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

			std::array<unsigned char, 2> out{ {
				static_cast<unsigned char>(unX & 0xFF),
				static_cast<unsigned char>((unX >> 8) & 0xFF)
			} };

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

			std::array<unsigned char, 4> out{ {
				static_cast<unsigned char>(unX & 0xFF),
				static_cast<unsigned char>((unX >> 8) & 0xFF),
				static_cast<unsigned char>((unX >> 16) & 0xFF),
				static_cast<unsigned char>((unX >> 24) & 0xFF)
			} };

			return out;
		}

		constexpr static T from(const std::array<unsigned char, 4>& bytes) noexcept {
			const unsigned int unX = static_cast<unsigned int>(bytes[0]) |
				(static_cast<unsigned int>(bytes[1]) << 8) |
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

	template <typename CharT>
	constexpr unsigned long long CRC64(const CharT* s, std::size_t unSize) noexcept {
		static_assert(std::is_integral<CharT>::value, "CharT must be an integral character type");
		static_assert(sizeof(CharT) == 1 || sizeof(CharT) == 2 || sizeof(CharT) == 4, "Unsupported character size (expected 1, 2, or 4 bytes)");

		unsigned long long unH = 0xFFFFFFFFFFFFFFFFull;
		for (std::size_t i = 0; i < unSize; ++i) {
			const auto bytes = ToBytes<CharT>(s[i]);
			for (std::size_t k = 0; k < bytes.size(); ++k) {
				unH = kCRC64Table[(bytes[k] ^ static_cast<unsigned char>(unH & 0xFF))] ^ (unH >> 8);
			}
		}

		return unH ^ 0xFFFFFFFFFFFFFFFFull;
	}

	template <typename CharT>
	constexpr unsigned int CRC32(const CharT* s, std::size_t unSize) noexcept {
		static_assert(std::is_integral<CharT>::value, "CharT must be an integral character type");
		static_assert(sizeof(CharT) == 1 || sizeof(CharT) == 2 || sizeof(CharT) == 4, "Unsupported character size (expected 1, 2, or 4 bytes)");

		unsigned int unH = 0xFFFFFFFFul;
		for (std::size_t i = 0; i < unSize; ++i) {
			const auto bytes = ToBytes<CharT>(s[i]);
			for (std::size_t k = 0; k < bytes.size(); ++k) {
				unH = kCRC32Table[(bytes[k] ^ static_cast<unsigned char>(unH & 0xFF))] ^ (unH >> 8);
			}
		}

		return unH ^ 0xFFFFFFFFul;
	}

	template <typename CharT, std::size_t N>
	constexpr unsigned long long CRC64(const CharT(&s)[N]) noexcept {
		return CRC64(s, N ? (N - 1) : 0);
	}

	template <typename CharT, std::size_t N>
	constexpr unsigned int CRC32(const CharT(&s)[N]) noexcept {
		return CRC32(s, N ? (N - 1) : 0);
	}

	template <typename CharT, typename Traits, typename Alloc>
	inline unsigned long long CRC64(const std::basic_string<CharT, Traits, Alloc>& str) noexcept {
		return CRC64(str.data(), str.size());
	}

	template <typename CharT, typename Traits, typename Alloc>
	inline unsigned int CRC32(const std::basic_string<CharT, Traits, Alloc>& str) noexcept {
		return CRC32(str.data(), str.size());
	}
}

#define HASHSTRING64(STRING) \
	std::integral_constant<unsigned long long, CRC::CRC64(STRING)>::value

#define HASHSTRING32(STRING) \
	std::integral_constant<unsigned int, CRC::CRC32(STRING)>::value

#ifdef _M_X64
#define HASHSTRING(STRING) HASHSTRING64(STRING)
#elif defined(_M_IX86)
#define HASHSTRING(STRING) HASHSTRING32(STRING)
#endif

#endif // _COMPILEHASHSTRING_H_
