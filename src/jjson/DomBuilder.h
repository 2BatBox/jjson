#pragma once

#include <jjson/type.h>
#include <jjson/SaxParser.h>

#include <cstdio>
#include <string>
#include <vector>
#include <memory>

namespace jjson {

template<typename A = std::allocator<Node> >
class DomBuilder {

	const size_t _capacity;
	A _allocator;
	Node* _value_pool;
	size_t _used_value;
	std::vector<Node*> _stack;
	Node* _root;
	bool _empty_pool;

public:

	DomBuilder(const DomBuilder&) = delete;
	DomBuilder& operator=(const DomBuilder&) = delete;

	DomBuilder(DomBuilder&& rv) = delete;
	DomBuilder& operator=(DomBuilder&&) = delete;

	DomBuilder(size_t value_pool_capacity) noexcept :
		_capacity(value_pool_capacity),
		_value_pool(_allocator.allocate(value_pool_capacity)),
		_used_value(0),
		_root(nullptr),
		_empty_pool(false) {}

	~DomBuilder() noexcept {
		_allocator.deallocate(_value_pool, _capacity);
		_value_pool = nullptr;
	}

	const Node* root() const noexcept {
		return _root;
	}

	void reset() noexcept {
		_used_value = 0;
		_stack.resize(0);
		_stack.push_back(nullptr);
		_root = nullptr;
		_empty_pool = false;
	}

	void document_start() noexcept {
		reset();
	}

	bool document_stop() noexcept {
		if(_used_value > 0) {
			_root = _value_pool;
		}
		return not _empty_pool;
	}

	void document_failure() noexcept {
		// printf("->document_failure()\n");
	}

	void sax_event(SaxParserEvent event, const std::string_view data) {
		switch(event) {
			case SaxParserEvent::ObjectStart :
				append_next_value(NodeType::Object, data);
				_stack.push_back(nullptr);
				break;

			case SaxParserEvent::ObjectStop :
				_stack.pop_back();
				break;

			case SaxParserEvent::ArrayStart :
				append_next_value(NodeType::Array, data);
				_stack.push_back(nullptr);
				break;

			case SaxParserEvent::ArrayStop :
				_stack.pop_back();
				break;

			case SaxParserEvent::String :
				append_next_value(NodeType::String, data.substr(1, data.size() - 2u));
				break;

			case SaxParserEvent::Number :
				append_next_value(NodeType::Number, data);
				break;

			case SaxParserEvent::Null :
				append_next_value(NodeType::Null, data);
				break;

			case SaxParserEvent::Bool :
				append_next_value((data[0] == 't' ? NodeType::True : NodeType::False), data);
				break;

			case SaxParserEvent::ObjectItemStart :
				append_next_value(NodeType::Key, data.substr(1, data.size() - 2u));
				_stack.push_back(nullptr);
				break;

			case SaxParserEvent::ObjectItemStop :
				_stack.pop_back();
				break;

			case SaxParserEvent::ValueSeparator :
				break;

		}
	}

private:


	void append_next_value(NodeType type, const std::string_view data) noexcept {
		if(_used_value < _capacity) {
			push_next(alloc_value(type, data));
		} else {
			_empty_pool = true;
		}
	}


	void push_next(Node* new_val) noexcept {
		if(_stack.back() == nullptr) {
			_stack.back() = new_val;

			// Link value
			if(_stack.size() > 1u) {
				auto parent = _stack[_stack.size() - 2u];
				parent->value = new_val;
			}
		} else {
			_stack.back()->next = new_val;
			_stack.back() = new_val;
		}
	}

	Node* alloc_value(const NodeType type, const std::string_view data) noexcept {
		Node* result = _value_pool + _used_value;
		_used_value++;
		result->next = nullptr;
		result->value = nullptr;
		result->data = data;
		result->type = type;
		return result;
	}

};

} // namespace jjson
