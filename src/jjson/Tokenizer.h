#pragma once

#include <jjson/type.h>
#include <endian.h>

namespace jjson {

/**
 * JSON string tokenizer.
 * See https://datatracker.ietf.org/doc/html/rfc8259 for more details.
 *
 * IMPORTANT:
 * - String escape codes are not supported.
 * - Integer format validation is not supported.
 * - Float format validation is not supported.
 */
class alignas(64u) Tokenizer {

	enum class CharClass : uint8_t {
		Unknown,
		Space,
		Structural,
		String,
		Integer,
		Null,
		True,
		False
	};

	const uint8_t* _str;
	const uint8_t* _str_end;
	size_t _str_len;
	size_t _chars_left;
	size_t _token_len;
	TokenType _token_type;

	CharClass _char_class_map[256u] = {
		CharClass::Unknown, // 000 00 NUL '\0' (null character)
		CharClass::Unknown, // 001 01 SOH (start of heading)
		CharClass::Unknown, // 002 02 STX (start of text)
		CharClass::Unknown, // 003 03 ETX (end of text)
		CharClass::Unknown, // 004 04 EOT (end of transmission)
		CharClass::Unknown, // 005 05 ENQ (enquiry)
		CharClass::Unknown, // 006 06 ACK (acknowledge)
		CharClass::Unknown, // 007 07 BEL '\a' (bell)
		CharClass::Unknown, // 008 08 BS  '\b' (backspace)
		CharClass::Space,   // 009 09 HT  '\t' (horizontal tab)
		CharClass::Space,   // 010 0A LF  '\n' (new line)
		CharClass::Unknown, // 011 0B VT  '\v' (vertical tab)
		CharClass::Unknown, // 012 0C FF  '\f' (form feed)
		CharClass::Space,   // 013 0D CR  '\r' (carriage ret)
		CharClass::Unknown, // 014 0E SO  (shift out)
		CharClass::Unknown, // 015 0F SI  (shift in)
		CharClass::Unknown, // 016 10 DLE (data link escape)
		CharClass::Unknown, // 017 11 DC1 (device control 1)
		CharClass::Unknown, // 018 12 DC2 (device control 2)
		CharClass::Unknown, // 019 13 DC3 (device control 3)
		CharClass::Unknown, // 020 14 DC4 (device control 4)
		CharClass::Unknown, // 021 15 NAK (negative ack.)
		CharClass::Unknown, // 022 16 SYN (synchronous idle)
		CharClass::Unknown, // 023 17 ETB (end of trans. blk)
		CharClass::Unknown, // 024 18 CAN (cancel)
		CharClass::Unknown, // 025 19 EM  (end of medium)
		CharClass::Unknown, // 026 1A SUB (substitute)
		CharClass::Unknown, // 027 1B ESC (escape)
		CharClass::Unknown, // 028 1C FS  (file separator)
		CharClass::Unknown, // 029 1D GS  (group separator)
		CharClass::Unknown, // 030 1E RS  (record separator)
		CharClass::Unknown, // 031 1F US  (unit separator)
		CharClass::Space,   // 032 20 SPACE
		CharClass::Unknown, // 033 21 '!'
		CharClass::String,  // 034 22 '"'
		CharClass::Unknown, // 035 23 '#'
		CharClass::Unknown, // 036 24 '$'
		CharClass::Unknown, // 037 25 '%'
		CharClass::Unknown, // 038 26 '&'
		CharClass::Unknown, // 039 27 '''
		CharClass::Unknown, // 040 28 '('
		CharClass::Unknown, // 041 29 ')'
		CharClass::Unknown, // 042 2A '*'
		CharClass::Unknown, // 043 2B '+'
		CharClass::Structural, // 044 2C ','
		CharClass::Integer, // 045 2D '-'
		CharClass::Unknown, // 046 2E '.'
		CharClass::Unknown, // 047 2F '/'
		CharClass::Integer, // 048 30 '0'
		CharClass::Integer, // 049 31 '1'
		CharClass::Integer, // 050 32 '2'
		CharClass::Integer, // 051 33 '3'
		CharClass::Integer, // 052 34 '4'
		CharClass::Integer, // 053 35 '5'
		CharClass::Integer, // 054 36 '6'
		CharClass::Integer, // 055 37 '7'
		CharClass::Integer, // 056 38 '8'
		CharClass::Integer, // 057 39 '9'
		CharClass::Structural, // 058 3A ':'
		CharClass::Unknown, // 059 3B ';'
		CharClass::Unknown, // 060 3C '<'
		CharClass::Unknown, // 061 3D '='
		CharClass::Unknown, // 062 3E '>'
		CharClass::Unknown, // 063 3F '?'
		CharClass::Unknown, // 064 40 '@'
		CharClass::Unknown, // 065 41 'A'
		CharClass::Unknown, // 066 42 'B'
		CharClass::Unknown, // 067 43 'C'
		CharClass::Unknown, // 068 44 'D'
		CharClass::Unknown, // 069 45 'E'
		CharClass::Unknown, // 070 46 'F'
		CharClass::Unknown, // 071 47 'G'
		CharClass::Unknown, // 072 48 'H'
		CharClass::Unknown, // 073 49 'I'
		CharClass::Unknown, // 074 4A 'J'
		CharClass::Unknown, // 075 4B 'K'
		CharClass::Unknown, // 076 4C 'L'
		CharClass::Unknown, // 077 4D 'M'
		CharClass::Unknown, // 078 4E 'N'
		CharClass::Unknown, // 079 4F 'O'
		CharClass::Unknown, // 080 50 'P'
		CharClass::Unknown, // 081 51 'Q'
		CharClass::Unknown, // 082 52 'R'
		CharClass::Unknown, // 083 53 'S'
		CharClass::Unknown, // 084 54 'T'
		CharClass::Unknown, // 085 55 'U'
		CharClass::Unknown, // 086 56 'V'
		CharClass::Unknown, // 087 57 'W'
		CharClass::Unknown, // 088 58 'X'
		CharClass::Unknown, // 089 59 'Y'
		CharClass::Unknown, // 090 5A 'Z'
		CharClass::Structural, // 091 5B '['
		CharClass::Unknown, // 092 5C '\'
		CharClass::Structural, // 093 5D ']'
		CharClass::Unknown, // 094 5E '^'
		CharClass::Unknown, // 095 5F '_'
		CharClass::Unknown, // 096 60 '`'
		CharClass::Unknown, // 097 61 'a'
		CharClass::Unknown, // 098 62 'b'
		CharClass::Unknown, // 099 63 'c'
		CharClass::Unknown, // 100 64 'd'
		CharClass::Unknown, // 101 65 'e'
		CharClass::False, // 102 66 'f'
		CharClass::Unknown, // 103 67 'g'
		CharClass::Unknown, // 104 68 'h'
		CharClass::Unknown, // 105 69 'i'
		CharClass::Unknown, // 106 6A 'j'
		CharClass::Unknown, // 107 6B 'k'
		CharClass::Unknown, // 108 6C 'l'
		CharClass::Unknown, // 109 6D 'm'
		CharClass::Null, // 110 6E 'n'
		CharClass::Unknown, // 111 6F 'o'
		CharClass::Unknown, // 112 70 'p'
		CharClass::Unknown, // 113 71 'q'
		CharClass::Unknown, // 114 72 'r'
		CharClass::Unknown, // 115 73 's'
		CharClass::True, // 116 74 't'
		CharClass::Unknown, // 117 75 'u'
		CharClass::Unknown, // 118 76 'v'
		CharClass::Unknown, // 119 77 'w'
		CharClass::Unknown, // 120 78 'x'
		CharClass::Unknown, // 121 79 'y'
		CharClass::Unknown, // 122 7A 'z'
		CharClass::Structural, // 123 7B '{'
		CharClass::Unknown, // 124 7C '|'
		CharClass::Structural, // 125 7D '}'
		CharClass::Unknown, // 126 7E '~'
		CharClass::Unknown, // 127 7F ''
		CharClass::Unknown, // 128 80
		CharClass::Unknown, // 129 81
		CharClass::Unknown, // 130 82
		CharClass::Unknown, // 131 83
		CharClass::Unknown, // 132 84
		CharClass::Unknown, // 133 85
		CharClass::Unknown, // 134 86
		CharClass::Unknown, // 135 87
		CharClass::Unknown, // 136 88
		CharClass::Unknown, // 137 89
		CharClass::Unknown, // 138 8A
		CharClass::Unknown, // 139 8B
		CharClass::Unknown, // 140 8C
		CharClass::Unknown, // 141 8D
		CharClass::Unknown, // 142 8E
		CharClass::Unknown, // 143 8F
		CharClass::Unknown, // 144 90
		CharClass::Unknown, // 145 91
		CharClass::Unknown, // 146 92
		CharClass::Unknown, // 147 93
		CharClass::Unknown, // 148 94
		CharClass::Unknown, // 149 95
		CharClass::Unknown, // 150 96
		CharClass::Unknown, // 151 97
		CharClass::Unknown, // 152 98
		CharClass::Unknown, // 153 99
		CharClass::Unknown, // 154 9A
		CharClass::Unknown, // 155 9B
		CharClass::Unknown, // 156 9C
		CharClass::Unknown, // 157 9D
		CharClass::Unknown, // 158 9E
		CharClass::Unknown, // 159 9F
		CharClass::Unknown, // 160 A0
		CharClass::Unknown, // 161 A1
		CharClass::Unknown, // 162 A2
		CharClass::Unknown, // 163 A3
		CharClass::Unknown, // 164 A4
		CharClass::Unknown, // 165 A5
		CharClass::Unknown, // 166 A6
		CharClass::Unknown, // 167 A7
		CharClass::Unknown, // 168 A8
		CharClass::Unknown, // 169 A9
		CharClass::Unknown, // 170 AA
		CharClass::Unknown, // 171 AB
		CharClass::Unknown, // 172 AC
		CharClass::Unknown, // 173 AD
		CharClass::Unknown, // 174 AE
		CharClass::Unknown, // 175 AF
		CharClass::Unknown, // 176 B0
		CharClass::Unknown, // 177 B1
		CharClass::Unknown, // 178 B2
		CharClass::Unknown, // 179 B3
		CharClass::Unknown, // 180 B4
		CharClass::Unknown, // 181 B5
		CharClass::Unknown, // 182 B6
		CharClass::Unknown, // 183 B7
		CharClass::Unknown, // 184 B8
		CharClass::Unknown, // 185 B9
		CharClass::Unknown, // 186 BA
		CharClass::Unknown, // 187 BB
		CharClass::Unknown, // 188 BC
		CharClass::Unknown, // 189 BD
		CharClass::Unknown, // 190 BE
		CharClass::Unknown, // 191 BF
		CharClass::Unknown, // 192 C0
		CharClass::Unknown, // 193 C1
		CharClass::Unknown, // 194 C2
		CharClass::Unknown, // 195 C3
		CharClass::Unknown, // 196 C4
		CharClass::Unknown, // 197 C5
		CharClass::Unknown, // 198 C6
		CharClass::Unknown, // 199 C7
		CharClass::Unknown, // 200 C8
		CharClass::Unknown, // 201 C9
		CharClass::Unknown, // 202 CA
		CharClass::Unknown, // 203 CB
		CharClass::Unknown, // 204 CC
		CharClass::Unknown, // 205 CD
		CharClass::Unknown, // 206 CE
		CharClass::Unknown, // 207 CF
		CharClass::Unknown, // 208 D0
		CharClass::Unknown, // 209 D1
		CharClass::Unknown, // 210 D2
		CharClass::Unknown, // 211 D3
		CharClass::Unknown, // 212 D4
		CharClass::Unknown, // 213 D5
		CharClass::Unknown, // 214 D6
		CharClass::Unknown, // 215 D7
		CharClass::Unknown, // 216 D8
		CharClass::Unknown, // 217 D9
		CharClass::Unknown, // 218 DA
		CharClass::Unknown, // 219 DB
		CharClass::Unknown, // 220 DC
		CharClass::Unknown, // 221 DD
		CharClass::Unknown, // 222 DE
		CharClass::Unknown, // 223 DF
		CharClass::Unknown, // 224 E0
		CharClass::Unknown, // 225 E1
		CharClass::Unknown, // 226 E2
		CharClass::Unknown, // 227 E3
		CharClass::Unknown, // 228 E4
		CharClass::Unknown, // 229 E5
		CharClass::Unknown, // 230 E6
		CharClass::Unknown, // 231 E7
		CharClass::Unknown, // 232 E8
		CharClass::Unknown, // 233 E9
		CharClass::Unknown, // 234 EA
		CharClass::Unknown, // 235 EB
		CharClass::Unknown, // 236 EC
		CharClass::Unknown, // 237 ED
		CharClass::Unknown, // 238 EE
		CharClass::Unknown, // 239 EF
		CharClass::Unknown, // 240 F0
		CharClass::Unknown, // 241 F1
		CharClass::Unknown, // 242 F2
		CharClass::Unknown, // 243 F3
		CharClass::Unknown, // 244 F4
		CharClass::Unknown, // 245 F5
		CharClass::Unknown, // 246 F6
		CharClass::Unknown, // 247 F7
		CharClass::Unknown, // 248 F8
		CharClass::Unknown, // 249 F9
		CharClass::Unknown, // 250 FA
		CharClass::Unknown, // 251 FB
		CharClass::Unknown, // 252 FC
		CharClass::Unknown, // 253 FD
		CharClass::Unknown, // 254 FE
		CharClass::Unknown, // 255 FF
	};

	static constexpr uint32_t build32u(const uint8_t str[4]) noexcept {
		uint32_t result = 0;
# if __BYTE_ORDER == __LITTLE_ENDIAN
			result = str[3];
			result <<= 8u;
			result |= str[2];
			result <<= 8u;
			result |= str[1];
			result <<= 8u;
			result |= str[0];
#elif
			result = str[0];
			result <<= 8u;
			result |= str[1];
			result <<= 8u;
			result |= str[2];
			result <<= 8u;
			result |= str[3];
#endif
		return result;
	}

public:

	Tokenizer() noexcept :
		_str(nullptr), _str_len(0),  _chars_left(0), _token_len(0) {}

	void reset(std::string_view strv) noexcept {
		reset(strv.data(), strv.size());
	}

	void reset(const char* str, const size_t str_len) noexcept {
		_str = reinterpret_cast<const uint8_t*>(str);
		_str_end = _str + str_len;
		_str_len = str_len;
		_chars_left = str_len;
	}

	~Tokenizer() noexcept = default;

	inline TokenType token_type() const noexcept {
		return _token_type;
	}

	inline const char* token_data() const noexcept {
		return reinterpret_cast<const char*>(_str);
	}

	inline std::string_view token_data_view() const noexcept {
		return {token_data(), _token_len};
	}

	inline size_t token_data_len() const noexcept {
		return _token_len;
	}

	/**
	* @return How many characters has been tokenized already.
	*/
	inline size_t chars_tokenized() const noexcept {
		return _str_len - _chars_left;
	}

	/**
	 * @return How many characters has not been tokenized yet.
	 */
	inline size_t chars_left() const noexcept {
		return _chars_left;
	}

	/**
	 * @return true - if any valid token has been read.
	 * If this method returns false and chars_left() returns zero it means
	 * whole the string has been tokenized.
	 * If this method returns false and chars_left() returns some positive value
	 * it means unknown token has been reached.
	 */
	bool token_read() {
		skip_token();
		skip_ws();
		if(_chars_left) {
			read_token();
		}
		return _token_len > 0;
	}

private:

	void read_token() noexcept {
		switch (_char_class_map[*_str]) {
			case CharClass::Structural:
				set_token(static_cast<TokenType>(*_str), 1u);
				break;

			case CharClass::String:
				set_token(TokenType::String, string_len());
				break;

			case CharClass::Integer:
				set_token(TokenType::Number, number_len());
				break;

			case CharClass::Null:
				set_token(TokenType::Null, null_len());
				break;

			case CharClass::True:
				set_token(TokenType::True, true_len());
				break;

			case CharClass::False:
				set_token(TokenType::False, false_len());
				break;

			default:
				_token_len = 0;
				break;
		}
	}

	inline size_t number_len() const noexcept {
		size_t result_offset = 1u;
		while(result_offset < _chars_left) {
			switch(_str[result_offset]) {
				case '.' :
				case '0'...'9' :
					result_offset++;
					break;

				default:
					return result_offset;
			}
		}
		return result_offset;
	}

	inline size_t string_len() const noexcept {
		const uint8_t* head = _str + 1u;
		size_t result = 0;
		head = static_cast<const uint8_t*>(memchr(head, '\"', _str_end - head));
		if(head) {
			head++;
			result = head - _str;
		}
		return result;
	}

	inline size_t null_len() const noexcept {
		static constexpr size_t TOKEN_LEN = 4u;
		static constexpr uint8_t TOKEN_CHARS [] = {'n', 'u', 'l', 'l'};
		static constexpr uint32_t TOKEN = build32u(TOKEN_CHARS);
		const auto uptr = reinterpret_cast<const uint32_t*>(_str);
		const bool result = available(TOKEN_LEN) && (*uptr == TOKEN);
		return result ? TOKEN_LEN : 0;
	}

	inline size_t true_len() const noexcept {
		static constexpr size_t TOKEN_LEN = 4u;
		static constexpr uint8_t TOKEN_CHARS [] = {'t', 'r', 'u', 'e'};
		static constexpr uint32_t TOKEN = build32u(TOKEN_CHARS);
		const auto uptr = reinterpret_cast<const uint32_t*>(_str);
		const bool result = available(TOKEN_LEN) && (*uptr == TOKEN);
		return result ? TOKEN_LEN : 0;
	}

	inline size_t false_len() const noexcept {
		static constexpr size_t TOKEN_LEN = 5u;
		static constexpr uint8_t TOKEN_CHARS [] = {'a', 'l', 's', 'e'};
		static constexpr uint32_t TOKEN = build32u(TOKEN_CHARS);
		const auto uptr = reinterpret_cast<const uint32_t*>(_str + 1u);
		const bool result = available(TOKEN_LEN) && (*uptr == TOKEN);
		return result ? TOKEN_LEN : 0;
	}

	inline bool available(size_t chars) const noexcept {
		return _chars_left >= chars;
	}

	inline void set_token(const TokenType type, const size_t str_len) noexcept {
		_token_type = type;
		_token_len = str_len;
	}

	inline void skip_token() noexcept {
		_str += _token_len;
		_chars_left -= _token_len;
		_token_len = 0;
	}

	inline void skip_ws() noexcept {
		while(_chars_left && _char_class_map[*_str] == CharClass::Space) {
			_str++;
			_chars_left--;
		}
	}

};

} // namespace jjson
