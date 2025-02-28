#pragma once

#include <lib/jjson/type.h>
#include <lib/jjson/Tokenizer.h>

#include <vector>
#include <string>

namespace jjson {

/**
 * JSON grammar.
 *
 * See https://datatracker.ietf.org/doc/html/rfc8259 for more details.
 *
 * Terminals: { } [ ] , : string number null true false eos
 * see jjson::Token
 *
 * Nonterminals : Document Array ArrayList Object ObjectList ObjectItem ObjectItemName ObjectItemValue Value
 * see jjson::SaxParser::State
 *
 * BNF
 * <Document> ::= <Value>
 * <Value> ::= <Object> | <Array> | string | number | null | true | false
 *
 * <Array> ::= [<ArrayList>]
 * <ArrayList> ::= e | <Value> | <Value>, <ArrayList>
 *
 * <Object> ::= {<ObjectList>}
 * <ObjectList> ::= e | <ObjectItem> | <ObjectItem>, <ObjectList>
 *
 * <ObjectItem> ::= <ObjectItemName>:<ObjectItemValue>
 * <ObjectItemName> ::= string
 * <ObjectItemValue> ::= <Value>
 *
 */

enum class SaxParserEvent : char {
	ObjectStart,
	ObjectStop,
	ObjectItemStart,
	ObjectItemStop,
	ValueSeparator,
	ArrayStart,
	ArrayStop,
	String,
	Number,
	Null,
	Bool
};

template <typename T>
class SaxParser {

	static constexpr int ERROR_TOKEN_LIMIT = 10;

	enum class State : char {
		Value,
		Array,
		Object,
		ArrayList,
		ObjectList,
		ObjectListItem,
		ObjectListItemValue,
		Failure
	};

	Tokenizer _tkz;
	std::vector<State> _stack;
	std::string _error;
	T& _receiver;

public:

	SaxParser(T& receiver) noexcept : _receiver(receiver) {}

	const T& receiver() const noexcept {
		return _receiver;
	}

	bool parse(std::string_view strv) noexcept {
		bool result = false;

		_tkz.reset(strv);
		_stack.resize(0);
		_stack.push_back(State::Value);
		_error.clear();

		if(_tkz.token_read()) {
			_receiver.document_start();

			bool has_token = true;
			while((not _stack.empty()) && _stack.back() != State::Failure && has_token) {
				// dump();
				if(step(_stack.back())) {
					has_token = _tkz.token_read();
				}
			}
			// dump();

			result = _stack.empty();
			if(result) {
				result = _receiver.document_stop();
			} else {
				// const size_t str_len = _tkz.chars_left() > ERROR_TOKEN_LIMIT ? ERROR_TOKEN_LIMIT : _tkz.chars_left();
				// std::string_view(_tkz.token_data(), str_len)
				_receiver.document_failure();
			}
		}

		return result;
	}

	[[nodiscard]] std::string error() const noexcept {
		return _error;
	}

	void dump(FILE* out) const {
		fprintf(out, "<SaxParser>\n");
		fprintf(out, "\t Token : %c '%.*s' %zu \n", char(_tkz.token_type()), int(_tkz.token_data_len()), _tkz.token_data(), _tkz.token_data_len());
		fprintf(out, "\t Chars : read=%zu left=%zu\n", _tkz.chars_tokenized(), _tkz.chars_left());
		fprintf(out, "\t Stack : ");
		for(const auto item : _stack) {
			fprintf(out, "%s, ", state_name(item));
		}
		fprintf(out, "\n");
	}

private:

	void set_error(const char* message) noexcept {
		_stack.push_back(State::Failure);

		_error.clear();

		if(message) {
			_error.append(message);
		}

		_error.append(" : +");
		_error.append(std::to_string(_tkz.chars_tokenized()));
		_error.append(" token-type='");
		_error.push_back(char(_tkz.token_type()));
		_error.append("'");
		_error.append(" token='");
		_error.append(_tkz.token_data_view());
		_error.append("'");

		_error.append(" >>> ");
		if(_tkz.chars_left() > ERROR_TOKEN_LIMIT) {
			_error.append(_tkz.token_data(), ERROR_TOKEN_LIMIT);
			_error.append("...");
		} else {
			_error.append(_tkz.token_data(), _tkz.chars_left());
		}

		_error.append("\nStack [");
		_error.append(std::to_string(_stack.size()));
		_error.append("] : ");
		for(size_t i = 0; i < _stack.size(); ++i) {
			_error.append(state_name(_stack[i]));
			_error.push_back(' ');
		}
	}

	bool step(const State state) noexcept {

		bool read_next_token;
		const auto token_type = _tkz.token_type();

		switch(state) {
			case State::Value :
				read_next_token = read_value(token_type);
				break;

			case State::Array :
				read_next_token = read_array(token_type);
				break;

			case State::ArrayList :
				read_next_token = read_array_list(token_type);
				break;

			case State::Object :
				read_next_token = read_object(token_type);
				break;

			case State::ObjectList :
				read_next_token = read_object_list(token_type);
				break;

			case State::ObjectListItem :
				read_next_token = read_object_list_item(token_type);
				break;

			case State::ObjectListItemValue :
				read_next_token = read_object_item_value(token_type);
				break;

			default:
				set_error("unknown state");
				read_next_token = false;
				break;

		}

		return read_next_token;
	}

	bool read_value(const TokenType tkn) noexcept {

		bool result = true;

		_stack.pop_back();

		switch(tkn) {
			case TokenType::ObjectBegin :
				_stack.push_back(State::Object);
				result = false;
				break;

			case TokenType::ArrayBegin :
				_stack.push_back(State::Array);
				result = false;
				break;

			case TokenType::Null :
				_receiver.sax_event(SaxParserEvent::Null, _tkz.token_data_view());
				break;

			case TokenType::True :
			case TokenType::False :
				_receiver.sax_event(SaxParserEvent::Bool, _tkz.token_data_view());
				break;

			case TokenType::String :
				_receiver.sax_event(SaxParserEvent::String, _tkz.token_data_view());
				break;

			case TokenType::Number :
				_receiver.sax_event(SaxParserEvent::Number, _tkz.token_data_view());
				break;

			default:
				set_error("value is expected");
				result = false;
				break;
		}

		return result;
	}

	bool read_array(const TokenType tkn) noexcept {
		bool result = true;
		switch(tkn) {
			case TokenType::ArrayBegin :
				_receiver.sax_event(SaxParserEvent::ArrayStart, _tkz.token_data_view());
				_stack.push_back(State::ArrayList);
				break;

			case TokenType::ArrayEnd:
				_stack.pop_back();
				_receiver.sax_event(SaxParserEvent::ArrayStop, _tkz.token_data_view());
				break;

			default:
				set_error("']' is expected");
				result = false;
				break;
		}
		return result;
	}

	bool read_array_list(const TokenType tkn) noexcept {
		bool result = false;
		switch(tkn) {
			case TokenType::ValueSeparator:
				_receiver.sax_event(SaxParserEvent::ValueSeparator, _tkz.token_data_view());
				_stack.push_back(State::Value);
				result = true;
				break;

			case TokenType::ArrayEnd:
				_stack.pop_back();
				break;

			default:
				_stack.push_back(State::Value);
				break;
		}
		return result;
	}

	bool read_object(const TokenType tkn) noexcept {
		bool result = true;
		switch(tkn) {
			case TokenType::ObjectBegin :
				_receiver.sax_event(SaxParserEvent::ObjectStart, _tkz.token_data_view());
				_stack.push_back(State::ObjectList);
				break;

			case TokenType::ObjectEnd:
				_stack.pop_back();
				_receiver.sax_event(SaxParserEvent::ObjectStop, _tkz.token_data_view());
				break;

			default:
				set_error("'}' is expected");
				result = false;
				break;
		}
		return result;
	}

	bool read_object_list(const TokenType tkn) noexcept {
		bool result = true;
		switch(tkn) {
			case TokenType::ValueSeparator:
				_receiver.sax_event(SaxParserEvent::ValueSeparator, _tkz.token_data_view());
				_stack.push_back(State::ObjectListItem);
				break;

			case TokenType::ObjectEnd:
				_stack.pop_back();
				result = false;
				break;

			default:
				result = false;
				_stack.push_back(State::ObjectListItem);
				break;
		}
		return result;
	}

	bool read_object_list_item(const TokenType tkn) noexcept {

		bool result = true;
		switch(tkn) {
			case TokenType::String:
				_receiver.sax_event(SaxParserEvent::ObjectItemStart, _tkz.token_data_view());
				_stack.back() = State::ObjectListItemValue;
				break;

			default:
				set_error("'string' is expected");
				result = false;
				break;
		}
		return result;
	}

	bool read_object_item_value(const TokenType tkn) noexcept {
		bool result = false;
		switch(tkn) {
			case TokenType::NameSeparator:
				_stack.push_back(State::Value);
				result = true;
				break;

			default:
				_receiver.sax_event(SaxParserEvent::ObjectItemStop, _tkz.token_data_view());
				result = false;
				_stack.pop_back();
				break;
		}
		return result;
	}

	static const char* state_name(State state) noexcept {
		switch(state) {
			case State::Value:
				return "Value";
			case State::Object:
				return "Object";
			case State::Array:
				return "Array";
			case State::ArrayList:
				return "ArrayList";
			case State::ObjectList:
				return "ObjectList";
			case State::ObjectListItem:
				return "ObjectListItem";
			case State::ObjectListItemValue:
				return "ObjectListItemValue";
			case State::Failure:
				return "Failure";
			default:
				return "Unknown";
		}
	}

};


} // namespace jjson
