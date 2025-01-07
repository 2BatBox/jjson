#pragma once

#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <string_view>

namespace jjson {

enum class NodeType : char {
	Object,
	Array,
	String,
	Number,
	True,
	False,
	Null,
	Key,
	Unknown
};

struct Node {
	Node* next;
	Node* value;
	std::string_view data;
	NodeType type;
};

enum class TokenType : uint8_t {
	// Single char tokens.
	ObjectBegin = '{',
	ObjectEnd = '}',
	ArrayBegin = '[',
	ArrayEnd = ']',
	NameSeparator = ':',
	ValueSeparator = ',',

	// Multi char tokens.
	Null = 'N',
	True = 'T',
	False = 'F',
	String = 'S',
	Number = 'I',

};

} // namespace jjson
