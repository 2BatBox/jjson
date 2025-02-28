#pragma once

#include <lib/jjson/type.h>
#include <lib/jjson/SaxParser.h>

#include <string>

namespace jjson {

class SaxStringBuilder {

	std::string _output;

public:

	const std::string& output() const noexcept {
		return _output;
	}

	void reset() noexcept {
		_output.resize(0);
	}

	void document_start() noexcept {
		reset();
	}

	bool document_stop() noexcept {
		return true;
	}

	void document_failure() noexcept {}

	void sax_event(const SaxParserEvent event, const std::string_view data) {

		switch (event) {
			case SaxParserEvent::ObjectStart:
			case SaxParserEvent::ObjectStop:
			case SaxParserEvent::ArrayStart:
			case SaxParserEvent::ArrayStop:
			case SaxParserEvent::ValueSeparator:
				_output.push_back(data[0]);
				break;

			case SaxParserEvent::String:
			case SaxParserEvent::Number:
			case SaxParserEvent::Null:
			case SaxParserEvent::Bool:
				_output.append(data);
				break;

			case SaxParserEvent::ObjectItemStart:
				_output.append(data);
				_output.push_back(':');
				break;

			default:
				break;
		}

	}

};

} // namespace jjson
