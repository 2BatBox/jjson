#pragma once

#include <jjson/type.h>
#include <jjson/SaxParser.h>

#include <cstdio>
#include <string>
#include <cassert>

namespace jjson {

class SaxEventPrinter {

public:

	void sax_event(SaxParserEvent event, std::string_view data) {
		printf("sax_event('%s' : '%.*s', %zu)\n", event_name(event), int(data.size()), data.data(), data.size());
		// assert(data.size() > 0);
	}

	static const char* event_name(SaxParserEvent event) noexcept {
		switch(event) {
			case SaxParserEvent::ObjectStart : return "ObjectStart";
			case SaxParserEvent::ObjectStop : return "ObjectStop";
			case SaxParserEvent::ArrayStart : return "ArrayStart";
			case SaxParserEvent::ArrayStop : return "ArrayStop";
			case SaxParserEvent::String : return "String";
			case SaxParserEvent::Number : return "Number";
			case SaxParserEvent::Null : return "Null";
			case SaxParserEvent::Bool : return "Bool";
			case SaxParserEvent::ObjectItemStart : return "ObjectItemStart";
			case SaxParserEvent::ObjectItemStop : return "ObjectItemStop";
			case SaxParserEvent::ValueSeparator : return "ValueSeparator";
			default : return "UNKNOWN";
		}
	}

};

} // namespace jjson
