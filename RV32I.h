#pragma once

#ifndef _RV32I_H_
#define _RV32I_H_

// STL

#include <string_view>
#include <cstdint>
#include <iostream>
#include <array>

// ============================================================================
// rv32i
// ============================================================================

namespace rv32i {

	// =========================================================================
	// Constants / Limits
	// =========================================================================

	constexpr unsigned int DATA_BASE = 0x10000000;
	constexpr unsigned int STACK_BASE = 0x80000000;

	struct Limits {
		static constexpr std::size_t MAX_LINES = 2048;
		static constexpr std::size_t MAX_INSNS = 2048;
		static constexpr std::size_t MAX_LABELS = 512;
		static constexpr std::size_t MAX_TOKENS = 8;
		static constexpr std::size_t MAX_NAME = 64;
		static constexpr std::size_t STEP_BUDGET = 200000;
		static constexpr std::size_t MEM_BYTES_MAX = 1 << 20;
	};

	// =========================================================================
	// Enums / Structures
	// =========================================================================

	enum class OP : unsigned char {
		LUI = 0,
		AUIPC,
		JAL,
		JALR,
		BEQ,
		BNE,
		BLT,
		BGE,
		BLTU,
		BGEU,
		LB,
		LH,
		LW,
		LBU,
		LHU,
		SB,
		SH,
		SW,
		ADDI,
		SLTI,
		SLTIU,
		XORI,
		ORI,
		ANDI,
		SLLI,
		SRLI,
		SRAI,
		ADD,
		SUB,
		SLL,
		SLT,
		SLTU,
		XOR,
		SRL,
		SRA,
		OR,
		AND,
		FENCE,
		FENCE_I,
		ECALL,
		EBREAK,
		INVALID
	};

	enum class STAGE : unsigned char {
		OK = 0,
		FIRST_PASS,
		SECOND_PASS,
		RUNTIME
	};

	template <std::size_t N>
	struct FixedString {
		constexpr FixedString(const char(&szText)[N]) {
			for (std::size_t unIndex = 0; unIndex < N; ++unIndex) {
				m_Data[unIndex] = szText[unIndex];
			}
		}

		constexpr std::string_view ToStringView() const {
			return std::string_view(m_Data, N - 1);
		}

		char m_Data[N] {};
	};

	template <std::size_t N>
	FixedString(const char(&)[N]) -> FixedString<N>;

	struct Context {
		unsigned int m_Registers[32] {};
		int m_nPC = 0;
	};

	struct Error {
		const char* m_pMessage = nullptr;
		int m_nLine = -1;
	};

	struct Name {
		constexpr void Clear() {
			m_Chars[0] = 0;
			m_unLength = 0;
		}

		constexpr bool Push(char nChar) {
			if (m_unLength >= (Limits::MAX_NAME - 1)) {
				return false;
			}

			m_Chars[m_unLength] = nChar;
			++m_unLength;

			m_Chars[m_unLength] = 0;

			return true;
		}

		constexpr std::string_view ToStringView() const {
			return std::string_view(m_Chars, m_unLength);
		}

		char m_Chars[Limits::MAX_NAME] {};
		std::size_t m_unLength = 0;
	};

	struct Token {
		Name m_NameText;
	};

	struct Line {
		std::size_t m_unTokensCount = 0;
		Token m_Tokens[Limits::MAX_TOKENS] {};
		bool m_bHasLabel = false;
		Name m_Label {};
		bool m_bEmpty = true;
	};

	struct Label {
		Name m_Name {};
		int m_nPC = -1;
	};

	struct Source {
		std::string_view m_Source {};
		std::size_t m_LineStarts[Limits::MAX_LINES] {};
		std::size_t m_unLinesCount = 0;
	};

	struct MemoryReference {
		int m_nIMM = 0;
		int m_nRS1 = -1;
	};

	struct Instruction {
		OP m_Operand = OP::INVALID;
		int m_nRD = - 1;
		int m_nRS1 = -1;
		int m_nRS2 = -1;
		int m_nIMM = 0;
		int m_nTargetPC = -1;
		int m_nLine = -1;
	};

	struct Program {
		std::size_t m_unCount = 0;
		Instruction m_Instructions[Limits::MAX_INSNS] {};
	};

	template <std::size_t DATA_BYTES, std::size_t STACK_BYTES>
	struct ExecutionResult {
		unsigned int m_Registers[32] {};
		int m_nPC = -1;
		std::size_t m_unSteps = 0;
		std::array<unsigned char, DATA_BYTES> m_Data {};
		std::array<unsigned char, STACK_BYTES> m_Stack {};
		unsigned int m_unStackBase = 0;
	};

	template <std::size_t DATA_BYTES, std::size_t STACK_BYTES>
	struct Outcome {
		bool m_bOK = false;
		STAGE m_Stage = STAGE::OK;
		int m_nLine = -1;
		ExecutionResult<DATA_BYTES, STACK_BYTES> m_Execution {};
	};

	struct UnresolvedReference {
		int m_nInstructionIndex = -1;
		Name m_LabelName {};
	};

	struct LabelTable {
		constexpr int Find(std::string_view NameView) const {
			for (std::size_t i = 0; i < m_unCount; ++i) {
				if (m_Labels[i].m_Name.ToStringView() == NameView) {
					return static_cast<int>(i);
				}
			}

			return -1;
		}

		constexpr int Add(std::string_view NameView, int nPC, Error& error, int nLine) {
			int nID = Find(NameView);
			if (nID >= 0) {
				error = { "duplicate label", nLine };
				return nID;
			}

			if (m_unCount >= Limits::MAX_LABELS) {
				error = { "too many labels", nLine };
				return -1;
			}

			Label& label = m_Labels[m_unCount];
			++m_unCount;

			label.m_Name.Clear();
			for (char nChar : NameView) {
				label.m_Name.Push(nChar);
			}

			label.m_nPC = nPC;
			return static_cast<int>(m_unCount - 1);
		}

		std::size_t m_unCount = 0;
		Label m_Labels[Limits::MAX_LABELS] {};
	};

	// =========================================================================
	// Char/String helpers
	// =========================================================================

	constexpr bool IsSpace(char nChar) {
		return (nChar == ' ') || (nChar == '\t') || (nChar == '\r') || (nChar == '\n') || (nChar == '\v') || (nChar == '\f');
	}

	constexpr bool IsAlpha(char nChar) {
		return ((nChar >= 'a') && (nChar <= 'z')) || ((nChar >= 'A') && (nChar <= 'Z')) || (nChar == '_');
	}

	constexpr bool IsDigit(char nChar) {
		return (nChar >= '0') && (nChar <= '9');
	}

	constexpr bool IsHEXChar(char nChar) {
		return IsDigit(nChar) || ((nChar >= 'a') && (nChar <= 'f')) || ((nChar >= 'A') && (nChar <= 'F'));
	}

	constexpr int HEXValue(char nChar) {
		if (((nChar >= '0') && (nChar <= '9'))) {
			return nChar - '0';
		}

		if (((nChar >= 'a') && (nChar <= 'f'))) {
			return 10 + (nChar - 'a');
		}

		if (((nChar >= 'A') && (nChar <= 'F'))) {
			return 10 + (nChar - 'A');
		}

		return 0;
	}

	constexpr bool EqualIgnoreCase(std::string_view svA, std::string_view svB) {
		if (svA.size() != svB.size()) {
			return false;
		}

		for (std::size_t unIndex = 0; unIndex < svA.size(); ++unIndex) {
			char nA = svA[unIndex];
			char nB = svB[unIndex];

			if (((nA >= 'A') && (nA <= 'Z'))) {
				nA = static_cast<char>(nA - 'A' + 'a');
			}

			if (((nB >= 'A') && (nB <= 'Z'))) {
				nB = static_cast<char>(nB - 'A' + 'a');
			}

			if (nA != nB) {
				return false;
			}
		}

		return true;
	}

	// =========================================================================
	// Source -> Lines
	// =========================================================================

	constexpr Source SplitLines(std::string_view SourceView) {
		Source source {};
		source.m_Source = SourceView;
		source.m_unLinesCount = 0;
		source.m_LineStarts[source.m_unLinesCount] = 0;
		++source.m_unLinesCount;

		std::size_t unIndex = 0;
		while (unIndex < SourceView.size()) {
			if (SourceView[unIndex] == '\n') {
				if (source.m_unLinesCount < Limits::MAX_LINES) {
					source.m_LineStarts[source.m_unLinesCount] = unIndex + 1;
					++source.m_unLinesCount;
				}
			}

			++unIndex;
		}

		return source;
	}

	// =========================================================================
	// Lexer: one line
	// =========================================================================

	constexpr Line LexLine(std::string_view LineView) {
		Line line {};
		std::size_t unIndex = 0;

		for (std::size_t unCut = 0; unCut < LineView.size(); ++unCut) {
			if (LineView[unCut] == '#') {
				LineView = LineView.substr(0, unCut);
				break;
			}

			if ((LineView[unCut] == '/') && ((unCut + 1) < LineView.size()) && (LineView[unCut + 1] == '/')) {
				LineView = LineView.substr(0, unCut);
				break;
			}
		}

		auto SkipWhitespace = [&](std::size_t& unPos) {
			while ((unPos < LineView.size()) && IsSpace(LineView[unPos])) {
				++unPos;
			}
		};

		SkipWhitespace(unIndex);

		if (unIndex >= LineView.size()) {
			line.m_bEmpty = true;
			return line;
		}

		line.m_bEmpty = false;

		{
			auto IsLabelStart = [](char nChar) -> bool {
				return IsAlpha(nChar) || (nChar == '.');
			};

			auto IsLabelBody = [](char nChar) -> bool {
				return IsAlpha(nChar) || IsDigit(nChar) || (nChar == '.') || (nChar == '_');
			};

			std::size_t unPosition = unIndex;
			if ((unPosition < LineView.size()) && IsLabelStart(LineView[unPosition])) {
				Name LabelName {};

				while ((unPosition < LineView.size()) && IsLabelBody(LineView[unPosition])) {
					LabelName.Push(LineView[unPosition]);
					++unPosition;
				}

				if ((unPosition < LineView.size()) && (LineView[unPosition] == ':')) {
					line.m_bHasLabel = true;
					line.m_Label = LabelName;
					unIndex = (unPosition + 1);
					SkipWhitespace(unIndex);
				}
			}
		}

		while (unIndex < LineView.size()) {
			SkipWhitespace(unIndex);
			if (unIndex >= LineView.size()) {
				break;
			}

			if (LineView[unIndex] == ',') {
				++unIndex;
				continue;
			}

			Name TokenName {};
			while ((unIndex < LineView.size()) && !IsSpace(LineView[unIndex]) && (LineView[unIndex] != ',')) {
				TokenName.Push(LineView[unIndex]);
				++unIndex;
			}

			if ((TokenName.m_unLength > 0) && (line.m_unTokensCount < Limits::MAX_TOKENS)) {
				line.m_Tokens[line.m_unTokensCount].m_NameText = TokenName;
				++line.m_unTokensCount;
			}
		}

		return line;
	}

	// =========================================================================
	// Parsing tokens
	// =========================================================================

	constexpr int RegisterFrom(std::string_view TokenView) {
		if ((TokenView.size() >= 2) && ((TokenView[0] == 'x') || (TokenView[0] == 'X'))) {
			int nValue = 0;
			for (std::size_t unIndex = 1; unIndex < TokenView.size(); ++unIndex) {
				if (!IsDigit(TokenView[unIndex])) {
					return -1;
				}

				nValue = (nValue * 10) + (TokenView[unIndex] - '0');
			}

			return ((nValue >= 0) && (nValue < 32)) ? nValue : -1;
		}

		struct Map {
			const char* m_pName;
			int m_nRegister;
		};

		constexpr Map REGISTER_MAP[] = {
			{ "zero", 0 },
			{ "ra",   1 },
			{ "sp",   2 },
			{ "gp",   3 },
			{ "tp",   4 },
			{ "t0",   5 },
			{ "t1",   6 },
			{ "t2",   7 },
			{ "s0",   8 },
			{ "fp",   8 },
			{ "s1",   9 },
			{ "a0",  10 },
			{ "a1",  11 },
			{ "a2",  12 },
			{ "a3",  13 },
			{ "a4",  14 },
			{ "a5",  15 },
			{ "a6",  16 },
			{ "a7",  17 },
			{ "s2",  18 },
			{ "s3",  19 },
			{ "s4",  20 },
			{ "s5",  21 },
			{ "s6",  22 },
			{ "s7",  23 },
			{ "s8",  24 },
			{ "s9",  25 },
			{ "s10", 26 },
			{ "s11", 27 },
			{ "t3",  28 },
			{ "t4",  29 },
			{ "t5",  30 },
			{ "t6",  31 }
		};

		for (const auto& entry : REGISTER_MAP) {
			if (EqualIgnoreCase(TokenView, entry.m_pName)) {
				return entry.m_nRegister;
			}
		}

		return -1;
	}

	constexpr bool ParseInt(std::string_view TokenView, int& nOut) {
		if (TokenView.empty()) {
			return false;
		}

		int nSign = 1;
		std::size_t unPosition = 0;

		if (TokenView[0] == '+') {
			nSign = 1;
			unPosition = 1;
		} else if (TokenView[0] == '-') {
			nSign = -1;
			unPosition = 1;
		}

		if (((unPosition + 2) <= TokenView.size()) && (TokenView[unPosition] == '0') && ((TokenView[unPosition + 1] == 'x') || (TokenView[unPosition + 1] == 'X'))) {
			unPosition += 2;
			std::uint64_t unValue = 0;

			if (unPosition >= TokenView.size()) {
				return false;
			}

			for (; unPosition < TokenView.size(); ++unPosition) {
				if (!IsHEXChar(TokenView[unPosition])) {
					return false;
				}

				unValue = (unValue << 4) + static_cast<std::uint64_t>(HEXValue(TokenView[unPosition]));
				unValue &= 0xFFFFFFFFull;
			}

			nOut = static_cast<int>(static_cast<unsigned int>(unValue));

			if (nSign == -1) {
				nOut = static_cast<int>(static_cast<unsigned int>(-static_cast<int>(unValue)));
			}

			return true;
		} else {
			std::int64_t nValue = 0;

			if (unPosition >= TokenView.size()) {
				return false;
			}

			for (; unPosition < TokenView.size(); ++unPosition) {
				if (!IsDigit(TokenView[unPosition])) {
					return false;
				}

				nValue = (nValue * 10) + (TokenView[unPosition] - '0');
			}

			nOut = static_cast<int>(nValue);

			if (nSign == -1) {
				nOut = -nOut;
			}

			return true;
		}
	}

	constexpr bool ParseMemoryReference(std::string_view TokenView, MemoryReference& MemRef) {
		std::size_t unLeft = TokenView.find('(');

		if (unLeft == std::string_view::npos) {
			int nValue = 0;
			if (!ParseInt(TokenView, nValue)) {
				return false;
			}

			MemRef.m_nIMM = nValue;
			MemRef.m_nRS1 = -1;

			return true;
		}

		if (unLeft > 0) {
			std::string_view immView = TokenView.substr(0, unLeft);
			int nValue = 0;
			if (!ParseInt(immView, nValue)) {
				return false;
			}

			MemRef.m_nIMM = nValue;
		} else {
			MemRef.m_nIMM = 0;
		}

		std::size_t unRight = TokenView.find(')', unLeft + 1);
		if (unRight == std::string_view::npos) {
			return false;
		}

		std::string_view RegisterView = TokenView.substr(unLeft + 1, unRight - (unLeft + 1));
		int nRegister = RegisterFrom(RegisterView);
		if (nRegister < 0) {
			return false;
		}

		MemRef.m_nRS1 = nRegister;
		return true;
	}

	// =========================================================================
	// Pass 1 / Pass 2
	// =========================================================================

	constexpr Error FirstPass(const Source& source, LabelTable& labeltable, int& nInstructionCount) {
		Error error {};
		nInstructionCount = 0;

		for (std::size_t unLineIndex = 0; unLineIndex < source.m_unLinesCount; ++unLineIndex) {
			std::size_t unBegin = source.m_LineStarts[unLineIndex];
			std::size_t unEnd = (unLineIndex + 1 < source.m_unLinesCount) ? source.m_LineStarts[unLineIndex + 1] : source.m_Source.size();

			std::string_view LineView = source.m_Source.substr(unBegin, unEnd - unBegin);
			Line parsed = LexLine(LineView);
			if (parsed.m_bEmpty) {
				continue;
			}

			if (parsed.m_bHasLabel) {
				if (labeltable.Add(parsed.m_Label.ToStringView(), nInstructionCount, error, static_cast<int>(unLineIndex + 1)) < 0) {
					return error;
				}
			}

			if (parsed.m_unTokensCount == 0) {
				continue;
			}

			if (nInstructionCount >= static_cast<int>(Limits::MAX_INSNS)) {
				error = { "too many instructions", static_cast<int>(unLineIndex + 1) };
				return error;
			}

			++nInstructionCount;
		}

		return error;
	}

	constexpr OP OpFrom(std::string_view MnemonicView) {
		if (EqualIgnoreCase(MnemonicView, "lui")) {
			return OP::LUI;
		}

		if (EqualIgnoreCase(MnemonicView, "auipc")) {
			return OP::AUIPC;
		}

		if (EqualIgnoreCase(MnemonicView, "jal")) {
			return OP::JAL;
		}

		if (EqualIgnoreCase(MnemonicView, "jalr")) {
			return OP::JALR;
		}

		if (EqualIgnoreCase(MnemonicView, "beq")) {
			return OP::BEQ;
		}

		if (EqualIgnoreCase(MnemonicView, "bne")) {
			return OP::BNE;
		}

		if (EqualIgnoreCase(MnemonicView, "blt")) {
			return OP::BLT;
		}

		if (EqualIgnoreCase(MnemonicView, "bge")) {
			return OP::BGE;
		}

		if (EqualIgnoreCase(MnemonicView, "bltu")) {
			return OP::BLTU;
		}

		if (EqualIgnoreCase(MnemonicView, "bgeu")) {
			return OP::BGEU;
		}

		if (EqualIgnoreCase(MnemonicView, "lb")) {
			return OP::LB;
		}

		if (EqualIgnoreCase(MnemonicView, "lh")) {
			return OP::LH;
		}

		if (EqualIgnoreCase(MnemonicView, "lw")) {
			return OP::LW;
		}

		if (EqualIgnoreCase(MnemonicView, "lbu")) {
			return OP::LBU;
		}

		if (EqualIgnoreCase(MnemonicView, "lhu")) {
			return OP::LHU;
		}

		if (EqualIgnoreCase(MnemonicView, "sb")) {
			return OP::SB;
		}

		if (EqualIgnoreCase(MnemonicView, "sh")) {
			return OP::SH;
		}

		if (EqualIgnoreCase(MnemonicView, "sw")) {
			return OP::SW;
		}

		if (EqualIgnoreCase(MnemonicView, "addi")) {
			return OP::ADDI;
		}

		if (EqualIgnoreCase(MnemonicView, "slti")) {
			return OP::SLTI;
		}

		if (EqualIgnoreCase(MnemonicView, "sltiu")) {
			return OP::SLTIU;
		}

		if (EqualIgnoreCase(MnemonicView, "xori")) {
			return OP::XORI;
		}

		if (EqualIgnoreCase(MnemonicView, "ori")) {
			return OP::ORI;
		}

		if (EqualIgnoreCase(MnemonicView, "andi")) {
			return OP::ANDI;
		}

		if (EqualIgnoreCase(MnemonicView, "slli")) {
			return OP::SLLI;
		}

		if (EqualIgnoreCase(MnemonicView, "srli")) {
			return OP::SRLI;
		}

		if (EqualIgnoreCase(MnemonicView, "srai")) {
			return OP::SRAI;
		}

		if (EqualIgnoreCase(MnemonicView, "add")) {
			return OP::ADD;
		}

		if (EqualIgnoreCase(MnemonicView, "sub")) {
			return OP::SUB;
		}

		if (EqualIgnoreCase(MnemonicView, "sll")) {
			return OP::SLL;
		}

		if (EqualIgnoreCase(MnemonicView, "slt")) {
			return OP::SLT;
		}

		if (EqualIgnoreCase(MnemonicView, "sltu")) {
			return OP::SLTU;
		}

		if (EqualIgnoreCase(MnemonicView, "xor")) {
			return OP::XOR;
		}

		if (EqualIgnoreCase(MnemonicView, "srl")) {
			return OP::SRL;
		}

		if (EqualIgnoreCase(MnemonicView, "sra")) {
			return OP::SRA;
		}

		if (EqualIgnoreCase(MnemonicView, "or")) {
			return OP::OR;
		}

		if (EqualIgnoreCase(MnemonicView, "and")) {
			return OP::AND;
		}

		if (EqualIgnoreCase(MnemonicView, "fence")) {
			return OP::FENCE;
		}

		if (EqualIgnoreCase(MnemonicView, "fence.i")) {
			return OP::FENCE_I;
		}

		if (EqualIgnoreCase(MnemonicView, "ecall")) {
			return OP::ECALL;
		}

		if (EqualIgnoreCase(MnemonicView, "ebreak")) {
			return OP::EBREAK;
		}

		if (EqualIgnoreCase(MnemonicView, "nop")) {
			return OP::ADDI;
		}

		if (EqualIgnoreCase(MnemonicView, "ret")) {
			return OP::JALR;
		}

		return OP::INVALID;
	}

	constexpr Error SecondPass(const Source& source, const LabelTable& labeltable, Program& program) {
		Error error {};
		UnresolvedReference UnresolvedArray[Limits::MAX_INSNS] {};
		std::size_t unUnresolvedCount = 0;

		int nPC = 0;

		for (std::size_t unLineIndex = 0; unLineIndex < source.m_unLinesCount; ++unLineIndex) {
			std::size_t unBegin = source.m_LineStarts[unLineIndex];
			std::size_t unEnd = (unLineIndex + 1 < source.m_unLinesCount) ? source.m_LineStarts[unLineIndex + 1] : source.m_Source.size();

			std::string_view LineView = source.m_Source.substr(unBegin, unEnd - unBegin);
			Line parsed = LexLine(LineView);
			if (parsed.m_bEmpty || (parsed.m_unTokensCount == 0)) {
				continue;
			}

			int nLineNo = static_cast<int>(unLineIndex + 1);

			std::string_view MnemonicView = parsed.m_Tokens[0].m_NameText.ToStringView();
			OP OpCode = OpFrom(MnemonicView);
			if (OpCode == OP::INVALID) {
				error = { "unknown mnemonic", nLineNo };
				return error;
			}

			Instruction instruction {};
			instruction.m_Operand = OpCode;
			instruction.m_nLine = nLineNo;

			auto NeedRegister = [&](int& nRegisterOut, std::string_view TokenView) -> bool {
				int nRegister = RegisterFrom(TokenView);
				if (nRegister < 0) {
					error = { "bad register", nLineNo };
					return false;
				}

				nRegisterOut = nRegister;
				return true;
			};

			auto NeedImmediate = [&](int& nImmOut, std::string_view TokenView) -> bool {
				if (!ParseInt(TokenView, nImmOut)) {
					error = { "bad immediate", nLineNo };
					return false;
				}

				return true;
			};

			auto NeedMemory = [&](MemoryReference& MemRefOut, std::string_view TokenView) -> bool {
				if (!ParseMemoryReference(TokenView, MemRefOut)) {
					error = { "bad mem operand", nLineNo };
					return false;
				}

				return true;
			};

			auto TokenAt = [&](std::size_t nIndex) -> std::string_view {
				if (nIndex < parsed.m_unTokensCount) {
					return parsed.m_Tokens[nIndex].m_NameText.ToStringView();
				}

				return std::string_view {};
			};

			if (EqualIgnoreCase(MnemonicView, "ret")) {
				instruction.m_Operand = OP::JALR;
				instruction.m_nRD = 0;
				instruction.m_nRS1 = 1;
				instruction.m_nIMM = 0;

				program.m_Instructions[program.m_unCount] = instruction;
				++program.m_unCount;
				++nPC;

				continue;
			}

			if (EqualIgnoreCase(MnemonicView, "nop")) {
				instruction.m_Operand = OP::ADDI;
				instruction.m_nRD = 0;
				instruction.m_nRS1 = 0;
				instruction.m_nIMM = 0;

				program.m_Instructions[program.m_unCount] = instruction;
				++program.m_unCount;
				++nPC;

				continue;
			}

			switch (OpCode) {
				case OP::LUI: {
					if (parsed.m_unTokensCount != 3) {
						error = { "need rd, imm", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(2))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::AUIPC: {
					if (parsed.m_unTokensCount != 3) {
						error = { "need rd, imm", nLineNo };
						return error;
					}
					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(2))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::JAL: {
					if (parsed.m_unTokensCount == 2) {
						instruction.m_nRD = 1;
						int nLabelID = labeltable.Find(TokenAt(1));
						if (nLabelID < 0) {
							UnresolvedArray[unUnresolvedCount].m_nInstructionIndex = nPC;
							UnresolvedArray[unUnresolvedCount].m_LabelName = parsed.m_Tokens[1].m_NameText;
							++unUnresolvedCount;
						} else {
							instruction.m_nTargetPC = labeltable.m_Labels[nLabelID].m_nPC;
						}
					} else if (parsed.m_unTokensCount == 3) {
						if (!NeedRegister(instruction.m_nRD, TokenAt(1))) {
							return error;
						}

						int nLabelID = labeltable.Find(TokenAt(2));
						if (nLabelID < 0) {
							UnresolvedArray[unUnresolvedCount].m_nInstructionIndex = nPC;
							UnresolvedArray[unUnresolvedCount].m_LabelName = parsed.m_Tokens[2].m_NameText;
							++unUnresolvedCount;
						} else {
							instruction.m_nTargetPC = labeltable.m_Labels[nLabelID].m_nPC;
						}
					} else {
						error = { "need jal [rd,] label", nLineNo };
						return error;
					}

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::JALR: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need jalr rd, rs1, imm", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::BEQ: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need beq rs1, rs2, label", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRS1, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(2))) { return error; }

					int nLabelID = labeltable.Find(TokenAt(3));
					if (nLabelID < 0) {
						UnresolvedArray[unUnresolvedCount].m_nInstructionIndex = nPC;
						UnresolvedArray[unUnresolvedCount].m_LabelName = parsed.m_Tokens[3].m_NameText;
						++unUnresolvedCount;
					} else {
						instruction.m_nTargetPC = labeltable.m_Labels[nLabelID].m_nPC;
					}

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::BNE: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need bne rs1, rs2, label", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRS1, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(2))) { return error; }

					int nLabelID = labeltable.Find(TokenAt(3));
					if (nLabelID < 0) {
						UnresolvedArray[unUnresolvedCount].m_nInstructionIndex = nPC;
						UnresolvedArray[unUnresolvedCount].m_LabelName = parsed.m_Tokens[3].m_NameText;
						++unUnresolvedCount;
					} else {
						instruction.m_nTargetPC = labeltable.m_Labels[nLabelID].m_nPC;
					}

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::BLT: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need blt rs1, rs2, label", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRS1, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(2))) { return error; }

					int nLabelID = labeltable.Find(TokenAt(3));
					if (nLabelID < 0) {
						UnresolvedArray[unUnresolvedCount].m_nInstructionIndex = nPC;
						UnresolvedArray[unUnresolvedCount].m_LabelName = parsed.m_Tokens[3].m_NameText;
						++unUnresolvedCount;
					} else {
						instruction.m_nTargetPC = labeltable.m_Labels[nLabelID].m_nPC;
					}

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::BGE: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need bge rs1, rs2, label", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRS1, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(2))) { return error; }

					int nLabelID = labeltable.Find(TokenAt(3));
					if (nLabelID < 0) {
						UnresolvedArray[unUnresolvedCount].m_nInstructionIndex = nPC;
						UnresolvedArray[unUnresolvedCount].m_LabelName = parsed.m_Tokens[3].m_NameText;
						++unUnresolvedCount;
					} else {
						instruction.m_nTargetPC = labeltable.m_Labels[nLabelID].m_nPC;
					}

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::BLTU: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need bltu rs1, rs2, label", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRS1, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(2))) { return error; }

					int nLabelID = labeltable.Find(TokenAt(3));
					if (nLabelID < 0) {
						UnresolvedArray[unUnresolvedCount].m_nInstructionIndex = nPC;
						UnresolvedArray[unUnresolvedCount].m_LabelName = parsed.m_Tokens[3].m_NameText;
						++unUnresolvedCount;
					} else {
						instruction.m_nTargetPC = labeltable.m_Labels[nLabelID].m_nPC;
					}

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::BGEU: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need bgeu rs1, rs2, label", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRS1, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(2))) { return error; }

					int nLabelID = labeltable.Find(TokenAt(3));
					if (nLabelID < 0) {
						UnresolvedArray[unUnresolvedCount].m_nInstructionIndex = nPC;
						UnresolvedArray[unUnresolvedCount].m_LabelName = parsed.m_Tokens[3].m_NameText;
						++unUnresolvedCount;
					} else {
						instruction.m_nTargetPC = labeltable.m_Labels[nLabelID].m_nPC;
					}

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::LB: {
					if (parsed.m_unTokensCount != 3) {
						error = { "need lb rd, imm(rs1)", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }

					MemoryReference MemRef {};
					if (!NeedMemory(MemRef, TokenAt(2))) { return error; }

					instruction.m_nRS1 = ((MemRef.m_nRS1 < 0) ? 0 : MemRef.m_nRS1);
					instruction.m_nIMM = MemRef.m_nIMM;

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::LH: {
					if (parsed.m_unTokensCount != 3) {
						error = { "need lh rd, imm(rs1)", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }

					MemoryReference MemRef {};
					if (!NeedMemory(MemRef, TokenAt(2))) { return error; }

					instruction.m_nRS1 = ((MemRef.m_nRS1 < 0) ? 0 : MemRef.m_nRS1);
					instruction.m_nIMM = MemRef.m_nIMM;

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::LW: {
					if (parsed.m_unTokensCount != 3) {
						error = { "need lw rd, imm(rs1)", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }

					MemoryReference MemRef {};
					if (!NeedMemory(MemRef, TokenAt(2))) { return error; }

					instruction.m_nRS1 = ((MemRef.m_nRS1 < 0) ? 0 : MemRef.m_nRS1);
					instruction.m_nIMM = MemRef.m_nIMM;

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::LBU: {
					if (parsed.m_unTokensCount != 3) {
						error = { "need lbu rd, imm(rs1)", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }

					MemoryReference MemRef {};
					if (!NeedMemory(MemRef, TokenAt(2))) { return error; }

					instruction.m_nRS1 = ((MemRef.m_nRS1 < 0) ? 0 : MemRef.m_nRS1);
					instruction.m_nIMM = MemRef.m_nIMM;

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::LHU: {
					if (parsed.m_unTokensCount != 3) {
						error = { "need lhu rd, imm(rs1)", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }

					MemoryReference MemRef {};
					if (!NeedMemory(MemRef, TokenAt(2))) { return error; }

					instruction.m_nRS1 = ((MemRef.m_nRS1 < 0) ? 0 : MemRef.m_nRS1);
					instruction.m_nIMM = MemRef.m_nIMM;

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SB: {
					if (parsed.m_unTokensCount != 3) {
						error = { "need sb rs2, imm(rs1)", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRS2, TokenAt(1))) { return error; }

					MemoryReference MemRef {};
					if (!NeedMemory(MemRef, TokenAt(2))) { return error; }

					instruction.m_nRS1 = ((MemRef.m_nRS1 < 0) ? 0 : MemRef.m_nRS1);
					instruction.m_nIMM = MemRef.m_nIMM;

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SH: {
					if (parsed.m_unTokensCount != 3) {
						error = { "need sh rs2, imm(rs1)", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRS2, TokenAt(1))) { return error; }

					MemoryReference MemRef {};
					if (!NeedMemory(MemRef, TokenAt(2))) { return error; }

					instruction.m_nRS1 = ((MemRef.m_nRS1 < 0) ? 0 : MemRef.m_nRS1);
					instruction.m_nIMM = MemRef.m_nIMM;

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SW: {
					if (parsed.m_unTokensCount != 3) {
						error = { "need sw rs2, imm(rs1)", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRS2, TokenAt(1))) { return error; }

					MemoryReference MemRef {};
					if (!NeedMemory(MemRef, TokenAt(2))) { return error; }

					instruction.m_nRS1 = ((MemRef.m_nRS1 < 0) ? 0 : MemRef.m_nRS1);
					instruction.m_nIMM = MemRef.m_nIMM;

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::ADDI: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need addi rd, rs1, imm", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SLTI: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need slti rd, rs1, imm", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SLTIU: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need sltiu rd, rs1, imm", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::XORI: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need xori rd, rs1, imm", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::ORI: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need ori rd, rs1, imm", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::ANDI: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need andi rd, rs1, imm", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SLLI: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need slli rd, rs1, shamt", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(3))) { return error; }
					if ((instruction.m_nIMM < 0) || (instruction.m_nIMM > 31)) {
						error = { "bad shamt", nLineNo };
						return error;
					}

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SRLI: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need srli rd, rs1, shamt", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(3))) { return error; }
					if ((instruction.m_nIMM < 0) || (instruction.m_nIMM > 31)) {
						error = { "bad shamt", nLineNo };
						return error;
					}

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SRAI: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need srai rd, rs1, shamt", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedImmediate(instruction.m_nIMM, TokenAt(3))) { return error; }
					if ((instruction.m_nIMM < 0) || (instruction.m_nIMM > 31)) {
						error = { "bad shamt", nLineNo };
						return error;
					}

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::ADD: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need add rd, rs1, rs2", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SUB: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need sub rd, rs1, rs2", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SLL: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need sll rd, rs1, rs2", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SLT: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need slt rd, rs1, rs2", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SLTU: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need sltu rd, rs1, rs2", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::XOR: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need xor rd, rs1, rs2", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SRL: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need srl rd, rs1, rs2", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::SRA: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need sra rd, rs1, rs2", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::OR: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need or rd, rs1, rs2", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::AND: {
					if (parsed.m_unTokensCount != 4) {
						error = { "need and rd, rs1, rs2", nLineNo };
						return error;
					}

					if (!NeedRegister(instruction.m_nRD, TokenAt(1))) { return error; }
					if (!NeedRegister(instruction.m_nRS1, TokenAt(2))) { return error; }
					if (!NeedRegister(instruction.m_nRS2, TokenAt(3))) { return error; }

					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::FENCE: {
					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::FENCE_I: {
					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::ECALL: {
					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				case OP::EBREAK: {
					program.m_Instructions[program.m_unCount] = instruction;
					++program.m_unCount;
					++nPC;
					break;
				}

				default: {
					break;
				}
			}
		}

		for (std::size_t i = 0; i < unUnresolvedCount; ++i) {
			int nInsIndex = UnresolvedArray[i].m_nInstructionIndex;
			int nLabelID = labeltable.Find(UnresolvedArray[i].m_LabelName.ToStringView());

			if (nLabelID < 0) {
				error = { "unknown label", program.m_Instructions[nInsIndex].m_nLine };
				return error;
			}

			program.m_Instructions[nInsIndex].m_nTargetPC = labeltable.m_Labels[nLabelID].m_nPC;
		}

		return error;
	}

	// =========================================================================
	// Memory
	// =========================================================================

	template <std::size_t BYTES>
	struct Memory {
		static_assert(BYTES <= static_cast<std::size_t>(Limits::MEM_BYTES_MAX), "mem too big");

		constexpr unsigned char ReadByte(unsigned int unAddress) const {
			return (unAddress < BYTES) ? m_Data[unAddress] : 0;
		}

		constexpr void WriteByte(unsigned int unAddress, unsigned char unValue) {
			if (unAddress < BYTES) {
				m_Data[unAddress] = unValue;
			}
		}

		constexpr unsigned short ReadHalf(unsigned int unAddress) const {
			const unsigned short unValue = static_cast<unsigned short>(ReadByte(unAddress)) | static_cast<unsigned short>(static_cast<unsigned short>(ReadByte(unAddress + 1)) << 8);
			return unValue;
		}

		constexpr void WriteHalf(unsigned int unAddress, unsigned short unValue) {
			WriteByte(unAddress,     static_cast<unsigned char>(unValue & 0xFFu));
			WriteByte(unAddress + 1, static_cast<unsigned char>((unValue >> 8) & 0xFFu));
		}

		constexpr unsigned int ReadWord(unsigned int unAddress) const {
			return   static_cast<unsigned int>(ReadByte(unAddress)           ) |
					(static_cast<unsigned int>(ReadByte(unAddress + 1)) <<  8) |
					(static_cast<unsigned int>(ReadByte(unAddress + 2)) << 16) |
					(static_cast<unsigned int>(ReadByte(unAddress + 3)) << 24);
		}

		constexpr void WriteWord(unsigned int unAddress, unsigned int unValue) {
			WriteByte(unAddress,     static_cast<unsigned char>( unValue        & 0xFFu));
			WriteByte(unAddress + 1, static_cast<unsigned char>((unValue >> 8)  & 0xFFu));
			WriteByte(unAddress + 2, static_cast<unsigned char>((unValue >> 16) & 0xFFu));
			WriteByte(unAddress + 3, static_cast<unsigned char>((unValue >> 24) & 0xFFu));
		}

		unsigned char m_Data[BYTES] {};
	};

	// =========================================================================
	// ALU
	// =========================================================================

	constexpr unsigned int ADD32(unsigned int unA, unsigned int unB) {
		return (unA + unB);
	}

	constexpr unsigned int SUB32(unsigned int unA, unsigned int unB) {
		return (unA - unB);
	}

	constexpr unsigned int SLL32(unsigned int unA, int nSh) {
		return ((nSh != 0) ? (unA << (nSh & 31)) : unA);
	}

	constexpr unsigned int SRL32(unsigned int unA, int nSh) {
		return ((nSh != 0) ? (unA >> (nSh & 31)) : unA);
	}

	constexpr unsigned int SRA32(unsigned int unA, int nSh) {
		if (nSh == 0) {
			return unA;
		}

		const int nSigned = static_cast<int>(unA);
		return static_cast<unsigned int>(nSigned >> (nSh & 31));
	}

	constexpr unsigned int SLT32(unsigned int unA, unsigned int unB) {
		return ((static_cast<int>(unA) < static_cast<int>(unB)) ? 1 : 0);
	}

	constexpr unsigned int SLTU32(unsigned int unA, unsigned int unB) {
		return ((unA < unB) ? 1 : 0);
	}

	// =========================================================================
	// VM
	// =========================================================================

	template <std::size_t DATA_BYTES, std::size_t STACK_BYTES>
	struct VirtualMachine {
		static_assert(DATA_BYTES <= static_cast<std::size_t>(Limits::MEM_BYTES_MAX), "data mem too big");
		static_assert(STACK_BYTES <= static_cast<std::size_t>(Limits::MEM_BYTES_MAX), "stack mem too big");

		static constexpr unsigned int DATA_TOP = DATA_BASE + static_cast<unsigned int>(DATA_BYTES);
		static constexpr unsigned int STACK_TOP = STACK_BASE + static_cast<unsigned int>(STACK_BYTES);

		template <std::size_t N>
		constexpr void SetDataFromBytes(const unsigned char(&data)[N]) {
			static_assert(N <= DATA_BYTES, "initial data bigger than DATA_BYTES");

			for (std::size_t i = 0; i < N; ++i) {
				m_Data.WriteByte(static_cast<unsigned int>(i), data[i]);
			}
		}

		template <std::size_t N>
		constexpr void SetDataFromBytes(const std::array<unsigned char, N>& data) {
			static_assert(N <= DATA_BYTES, "initial data bigger than DATA_BYTES");

			for (std::size_t i = 0; i < N; ++i) {
				m_Data.WriteByte(static_cast<unsigned int>(i), data[i]);
			}
		}

		template <std::size_t N>
		constexpr void SetStackFromBytes(const unsigned char(&data)[N], unsigned int stack_offset = 0) {
			static_assert(N <= STACK_BYTES, "initial stack bigger than STACK_BYTES");

			for (std::size_t i = 0; i < N; ++i) {
				m_Stack.WriteByte(stack_offset + static_cast<unsigned int>(i), data[i]);
			}
		}

		constexpr unsigned char MRead8(unsigned int unAddress) const {
			if (unAddress >= STACK_BASE) {
				const unsigned int unOff = unAddress - STACK_BASE;
				return m_Stack.ReadByte(unOff);
			}

			if ((unAddress >= DATA_BASE) && (unAddress < DATA_TOP)) {
				const unsigned int unOff = unAddress - DATA_BASE;
				return m_Data.ReadByte(unOff);
			}

			return 0;
		}

		constexpr unsigned short MRead16(unsigned int unAddress) const {
			if (unAddress >= STACK_BASE) {
				const unsigned int unOff = unAddress - STACK_BASE;
				return m_Stack.ReadHalf(unOff);
			}

			if ((unAddress >= DATA_BASE) && (unAddress < DATA_TOP)) {
				const unsigned int unOff = unAddress - DATA_BASE;
				return m_Data.ReadHalf(unOff);
			}

			return 0;
		}

		constexpr unsigned int MRead32(unsigned int unAddress) const {
			if (unAddress >= STACK_BASE) {
				const unsigned int unOff = unAddress - STACK_BASE;
				return m_Stack.ReadWord(unOff);
			}

			if ((unAddress >= DATA_BASE) && (unAddress < DATA_TOP)) {
				const unsigned int unOff = unAddress - DATA_BASE;
				return m_Data.ReadWord(unOff);
			}

			return 0;
		}

		constexpr void MWrite8(unsigned int unAddress, unsigned char v) {
			if (unAddress >= STACK_BASE) {
				const unsigned int unOff = unAddress - STACK_BASE;
				m_Stack.WriteByte(unOff, v);
			} else if ((unAddress >= DATA_BASE) && (unAddress < DATA_TOP)) {
				const unsigned int unOff = unAddress - DATA_BASE;
				m_Data.WriteByte(unOff, v);
			}
		}

		constexpr void MWrite16(unsigned int unAddress, unsigned short v) {
			if (unAddress >= STACK_BASE) {
				const unsigned int unOff = unAddress - STACK_BASE;
				m_Stack.WriteHalf(unOff, v);
			} else if ((unAddress >= DATA_BASE) && (unAddress < DATA_TOP)) {
				const unsigned int unOff = unAddress - DATA_BASE;
				m_Data.WriteHalf(unOff, v);
			}
		}

		constexpr void MWrite32(unsigned int unAddress, unsigned int v) {
			if (unAddress >= STACK_BASE) {
				const unsigned int unOff = unAddress - STACK_BASE;
				m_Stack.WriteWord(unOff, v);
			} else if ((unAddress >= DATA_BASE) && (unAddress < DATA_TOP)) {
				const unsigned int unOff = unAddress - DATA_BASE;
				m_Data.WriteWord(unOff, v);
			}
		}

		constexpr bool Step() {
			if ((m_Result.m_nPC < 0) || (m_Result.m_nPC >= static_cast<int>(m_Program.m_unCount))) {
				m_Error = { "pc out of range", -1 };
				return false;
			}

			const Instruction& instruction = m_Program.m_Instructions[m_Result.m_nPC];
			auto& registers = m_Result.m_Registers;

			const unsigned int unPCBytes = static_cast<unsigned int>(m_Result.m_nPC * 4);
			int nNextPC = (m_Result.m_nPC + 1);

			auto WriteReg = [&](int nRegister, unsigned int unValue) {
				if (nRegister != 0) {
					registers[nRegister] = unValue;
				}
			};

			auto ReadReg = [&](int nRegister) -> unsigned int {
				return registers[nRegister];
			};

			switch (instruction.m_Operand) {
				case OP::LUI: {
					WriteReg(instruction.m_nRD, static_cast<unsigned int>(instruction.m_nIMM));
					break;
				}

				case OP::AUIPC: {
					WriteReg(instruction.m_nRD, ADD32(unPCBytes, static_cast<unsigned int>(instruction.m_nIMM)));
					break;
				}

				case OP::JAL: {
					WriteReg(instruction.m_nRD, ADD32(unPCBytes, 4));
					nNextPC = instruction.m_nTargetPC;
					break;
				}

				case OP::JALR: {
					const unsigned int unTarget = ADD32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM)) & ~1;
					WriteReg(instruction.m_nRD, ADD32(unPCBytes, 4));
					nNextPC = static_cast<int>(unTarget / 4);
					break;
				}

				case OP::BEQ: {
					if (ReadReg(instruction.m_nRS1) == ReadReg(instruction.m_nRS2)) {
						nNextPC = instruction.m_nTargetPC;
					}

					break;
				}

				case OP::BNE: {
					if (ReadReg(instruction.m_nRS1) != ReadReg(instruction.m_nRS2)) {
						nNextPC = instruction.m_nTargetPC;
					}

					break;
				}

				case OP::BLT: {
					if (static_cast<int>(ReadReg(instruction.m_nRS1)) < static_cast<int>(ReadReg(instruction.m_nRS2))) {
						nNextPC = instruction.m_nTargetPC;
					}

					break;
				}

				case OP::BGE: {
					if (static_cast<int>(ReadReg(instruction.m_nRS1)) >= static_cast<int>(ReadReg(instruction.m_nRS2))) {
						nNextPC = instruction.m_nTargetPC;
					}

					break;
				}

				case OP::BLTU: {
					if (ReadReg(instruction.m_nRS1) < ReadReg(instruction.m_nRS2)) {
						nNextPC = instruction.m_nTargetPC;
					}

					break;
				}

				case OP::BGEU: {
					if (ReadReg(instruction.m_nRS1) >= ReadReg(instruction.m_nRS2)) {
						nNextPC = instruction.m_nTargetPC;
					}
					
					break;
				}

				case OP::LB: {
					const unsigned int unAddress = ADD32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM));
					const char unValue = static_cast<char>(MRead8(unAddress));
					WriteReg(instruction.m_nRD, static_cast<unsigned int>(unValue));
					break;
				}

				case OP::LH: {
					const unsigned int unAddress = ADD32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM));
					if ((unAddress & 1) != 0) {
						m_Error = { "unaligned lh", instruction.m_nLine };
						return false;
					}

					const short unValue = static_cast<short>(MRead16(unAddress));
					WriteReg(instruction.m_nRD, static_cast<unsigned int>(unValue));
					break;
				}

				case OP::LW: {
					const unsigned int unAddress = ADD32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM));
					if ((unAddress & 3) != 0) {
						m_Error = { "unaligned lw", instruction.m_nLine };
						return false;
					}

					const unsigned int unValue = MRead32(unAddress);
					WriteReg(instruction.m_nRD, unValue);
					break;
				}

				case OP::LBU: {
					const unsigned int unAddress = ADD32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM));
					const unsigned char unValue = MRead8(unAddress);
					WriteReg(instruction.m_nRD, static_cast<unsigned int>(unValue));
					break;
				}

				case OP::LHU: {
					const unsigned int unAddress = ADD32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM));
					if ((unAddress & 1) != 0) {
						m_Error = { "unaligned lhu", instruction.m_nLine };
						return false;
					}

					const unsigned short unValue = MRead16(unAddress);
					WriteReg(instruction.m_nRD, static_cast<unsigned int>(unValue));
					break;
				}

				case OP::SB: {
					const unsigned int unAddress = ADD32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM));
					MWrite8(unAddress, static_cast<unsigned char>(ReadReg(instruction.m_nRS2) & 0xFFu));
					break;
				}

				case OP::SH: {
					const unsigned int unAddress = ADD32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM));
					if ((unAddress & 1) != 0) {
						m_Error = { "unaligned sh", instruction.m_nLine };
						return false;
					}

					MWrite16(unAddress, static_cast<unsigned short>(ReadReg(instruction.m_nRS2) & 0xFFFFu));
					break;
				}

				case OP::SW: {
					const unsigned int unAddress = ADD32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM));
					if ((unAddress & 3) != 0) {
						m_Error = { "unaligned sw", instruction.m_nLine };
						return false;
					}

					MWrite32(unAddress, ReadReg(instruction.m_nRS2));
					break;
				}

				case OP::ADDI: {
					WriteReg(instruction.m_nRD, ADD32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM)));
					break;
				}

				case OP::SLTI: {
					WriteReg(instruction.m_nRD, SLT32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM)));
					break;
				}

				case OP::SLTIU: {
					WriteReg(instruction.m_nRD, SLTU32(ReadReg(instruction.m_nRS1), static_cast<unsigned int>(instruction.m_nIMM)));
					break;
				}

				case OP::XORI: {
					WriteReg(instruction.m_nRD, (ReadReg(instruction.m_nRS1) ^ static_cast<unsigned int>(instruction.m_nIMM)));
					break;
				}

				case OP::ORI: {
					WriteReg(instruction.m_nRD, (ReadReg(instruction.m_nRS1) | static_cast<unsigned int>(instruction.m_nIMM)));
					break;
				}

				case OP::ANDI: {
					WriteReg(instruction.m_nRD, (ReadReg(instruction.m_nRS1) & static_cast<unsigned int>(instruction.m_nIMM)));
					break;
				}

				case OP::SLLI: {
					WriteReg(instruction.m_nRD, SLL32(ReadReg(instruction.m_nRS1), instruction.m_nIMM));
					break;
				}

				case OP::SRLI: {
					WriteReg(instruction.m_nRD, SRL32(ReadReg(instruction.m_nRS1), instruction.m_nIMM));
					break;
				}

				case OP::SRAI: {
					WriteReg(instruction.m_nRD, SRA32(ReadReg(instruction.m_nRS1), instruction.m_nIMM));
					break;
				}

				case OP::ADD: {
					WriteReg(instruction.m_nRD, ADD32(ReadReg(instruction.m_nRS1), ReadReg(instruction.m_nRS2)));
					break;
				}

				case OP::SUB: {
					WriteReg(instruction.m_nRD, SUB32(ReadReg(instruction.m_nRS1), ReadReg(instruction.m_nRS2)));
					break;
				}

				case OP::SLL: {
					WriteReg(instruction.m_nRD, SLL32(ReadReg(instruction.m_nRS1), static_cast<int>(ReadReg(instruction.m_nRS2) & 31)));
					break;
				}

				case OP::SLT: {
					WriteReg(instruction.m_nRD, SLT32(ReadReg(instruction.m_nRS1), ReadReg(instruction.m_nRS2)));
					break;
				}

				case OP::SLTU: {
					WriteReg(instruction.m_nRD, SLTU32(ReadReg(instruction.m_nRS1), ReadReg(instruction.m_nRS2)));
					break;
				}

				case OP::XOR: {
					WriteReg(instruction.m_nRD, (ReadReg(instruction.m_nRS1) ^ ReadReg(instruction.m_nRS2)));
					break;
				}

				case OP::SRL: {
					WriteReg(instruction.m_nRD, SRL32(ReadReg(instruction.m_nRS1), static_cast<int>(ReadReg(instruction.m_nRS2) & 31)));
					break;
				}

				case OP::SRA: {
					WriteReg(instruction.m_nRD, SRA32(ReadReg(instruction.m_nRS1), static_cast<int>(ReadReg(instruction.m_nRS2) & 31)));
					break;
				}

				case OP::OR: {
					WriteReg(instruction.m_nRD, (ReadReg(instruction.m_nRS1) | ReadReg(instruction.m_nRS2)));
					break;
				}

				case OP::AND: {
					WriteReg(instruction.m_nRD, (ReadReg(instruction.m_nRS1) & ReadReg(instruction.m_nRS2)));
					break;
				}

				case OP::FENCE: {
					break;
				}

				case OP::FENCE_I: {
					break;
				}

				case OP::ECALL: {
					m_Error = { "ecall trap",  instruction.m_nLine };
					return false;
				}

				case OP::EBREAK: {
					m_Error = { "ebreak trap", instruction.m_nLine };
					return false;
				}

				default: {
					m_Error = { "invalid op",  instruction.m_nLine };
					return false;
				}
			}

			registers[0] = 0;
			m_Result.m_nPC = nNextPC;
			return true;
		}

		constexpr ExecutionResult<DATA_BYTES, STACK_BYTES> Run(Context ctx) {
			for (unsigned char i = 0; i < 32; ++i) {
				m_Result.m_Registers[i] = ctx.m_Registers[i];
			}

			if (m_Result.m_Registers[2] == 0) {
				m_Result.m_Registers[2] = STACK_TOP;
			}

			if (m_Result.m_Registers[10] == 0) {
				m_Result.m_Registers[10] = DATA_BASE;
			}

			m_Result.m_nPC = ctx.m_nPC;
			m_Result.m_unSteps = 0;

			while ((m_Result.m_nPC >= 0) && (m_Result.m_nPC < static_cast<int>(m_Program.m_unCount))) {
				++m_Result.m_unSteps;

				if (m_Result.m_unSteps > Limits::STEP_BUDGET) {
					m_Error = { "step budget exceeded", -1 };
					break;
				}

				if (!Step()) {
					break;
				}
			}

			for (std::size_t i = 0; i < DATA_BYTES; ++i) {
				m_Result.m_Data[i] = m_Data.m_Data[i];
			}

			for (std::size_t i = 0; i < STACK_BYTES; ++i) {
				m_Result.m_Stack[i] = m_Stack.m_Data[i];
			}

			m_Result.m_unStackBase = STACK_BASE;

			return m_Result;
		}

		Program m_Program {};
		Memory<DATA_BYTES> m_Data {};
		Memory<STACK_BYTES> m_Stack {};
		ExecutionResult<DATA_BYTES, STACK_BYTES> m_Result {};
		Error m_Error {};
	};

	// =========================================================================
	// Frontend: consteval Run
	// =========================================================================

	template <FixedString SOURCE_STRING, std::size_t DATA_N, std::size_t STACK_BYTES = 4096>
	consteval Outcome<DATA_N, STACK_BYTES> Run(const unsigned char(&DataBytes)[DATA_N], const Context& initialctx) {
		Outcome<DATA_N, STACK_BYTES> outcome {};

		auto source = SplitLines(SOURCE_STRING.ToStringView());
		LabelTable labeltable {};
		int nInstructionCount = 0;

		if (Error error = FirstPass(source, labeltable, nInstructionCount); error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::FIRST_PASS;
			outcome.m_nLine = error.m_nLine;
			return outcome;
		}

		Program program {};
		if (Error error = SecondPass(source, labeltable, program); error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::SECOND_PASS;
			outcome.m_nLine = error.m_nLine;
			return outcome;
		}

		VirtualMachine<DATA_N, STACK_BYTES> vm {};
		vm.m_Program = program;
		vm.SetDataFromBytes(DataBytes);

		auto result = vm.Run(initialctx);

		if (vm.m_Error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::RUNTIME;
			outcome.m_nLine = vm.m_Error.m_nLine;
			outcome.m_Execution = result;
			return outcome;
		}

		outcome.m_bOK = true;
		outcome.m_Stage = STAGE::OK;
		outcome.m_Execution = result;
		return outcome;
	}

	template <FixedString SOURCE_STRING, std::size_t DATA_N, std::size_t STACK_BYTES = 4096>
	consteval Outcome<DATA_N, STACK_BYTES> Run(const std::array<unsigned char, DATA_N>& DataBytes, const Context& initialctx) {
		Outcome<DATA_N, STACK_BYTES> outcome {};

		auto source = SplitLines(SOURCE_STRING.ToStringView());
		LabelTable labeltable {};
		int nInstructionCount = 0;

		if (Error error = FirstPass(source, labeltable, nInstructionCount); error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::FIRST_PASS;
			outcome.m_nLine = error.m_nLine;
			return outcome;
		}

		Program program {};
		if (Error error = SecondPass(source, labeltable, program); error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::SECOND_PASS;
			outcome.m_nLine = error.m_nLine;
			return outcome;
		}

		VirtualMachine<DATA_N, STACK_BYTES> vm {};
		vm.m_Program = program;
		vm.SetDataFromBytes(DataBytes);

		auto result = vm.Run(initialctx);

		if (vm.m_Error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::RUNTIME;
			outcome.m_nLine = vm.m_Error.m_nLine;
			outcome.m_Execution = result;
			return outcome;
		}

		outcome.m_bOK = true;
		outcome.m_Stage = STAGE::OK;
		outcome.m_Execution = result;
		return outcome;
	}

	template <std::size_t DATA_N, std::size_t STACK_BYTES = 4096, std::size_t SRC_N>
	consteval Outcome<DATA_N, STACK_BYTES> Run(const char(&SRC)[SRC_N], const unsigned char(&DataBytes)[DATA_N], const Context& initialctx) {
		Outcome<DATA_N, STACK_BYTES> outcome {};

		auto source = SplitLines(std::string_view { SRC, SRC_N - 1 });
		LabelTable labeltable {};
		int nInstructionCount = 0;

		if (Error error = FirstPass(source, labeltable, nInstructionCount); error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::FIRST_PASS;
			outcome.m_nLine = error.m_nLine;
			return outcome;
		}

		Program program {};
		if (Error error = SecondPass(source, labeltable, program); error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::SECOND_PASS;
			outcome.m_nLine = error.m_nLine;
			return outcome;
		}

		VirtualMachine<DATA_N, STACK_BYTES> vm {};
		vm.m_Program = program;
		vm.SetDataFromBytes(DataBytes);

		auto result = vm.Run(initialctx);

		if (vm.m_Error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::RUNTIME;
			outcome.m_nLine = vm.m_Error.m_nLine;
			outcome.m_Execution = result;
			return outcome;
		}

		outcome.m_bOK = true;
		outcome.m_Stage = STAGE::OK;
		outcome.m_Execution = result;
		return outcome;
	}

	template <std::size_t DATA_N, std::size_t STACK_BYTES = 4096, std::size_t SRC_N>
	consteval Outcome<DATA_N, STACK_BYTES> Run(const char(&SRC)[SRC_N], std::array<unsigned char, DATA_N> DataBytes, const Context& initialctx) {
		Outcome<DATA_N, STACK_BYTES> outcome {};

		auto source = SplitLines(std::string_view { SRC, SRC_N - 1 });
		LabelTable labeltable {};
		int nInstructionCount = 0;

		if (Error error = FirstPass(source, labeltable, nInstructionCount); error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::FIRST_PASS;
			outcome.m_nLine = error.m_nLine;
			return outcome;
		}

		Program program {};
		if (Error error = SecondPass(source, labeltable, program); error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::SECOND_PASS;
			outcome.m_nLine = error.m_nLine;
			return outcome;
		}

		VirtualMachine<DATA_N, STACK_BYTES> vm {};
		vm.m_Program = program;
		vm.SetDataFromBytes(DataBytes);

		auto result = vm.Run(initialctx);

		if (vm.m_Error.m_pMessage) {
			outcome.m_bOK = false;
			outcome.m_Stage = STAGE::RUNTIME;
			outcome.m_nLine = vm.m_Error.m_nLine;
			outcome.m_Execution = result;
			return outcome;
		}

		outcome.m_bOK = true;
		outcome.m_Stage = STAGE::OK;
		outcome.m_Execution = result;
		return outcome;
	}
}

#endif // !_RV32I_H_
