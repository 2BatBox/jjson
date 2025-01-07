#pragma once

#include <jjson/type.h>

namespace jjson {

struct Document {

	Node* value;

	inline bool enter() noexcept {
		if(value) {
			value = value->value;
		}
		return value != nullptr;
	}

	bool find_key(const char* name) noexcept {
		while(find_type(NodeType::Key)) {
			if(string_cmp(name)) {
				break;
			} else {
				value = value->next;
			}
		}
		return value != nullptr;
	}

	bool find_key_value(const char* name) noexcept {
		find_key(name);
		return next();
	}

	inline bool next() noexcept {
		if(value) {
			value = value->next;
		}
		return value != nullptr;
	}

private:

	inline bool find_type(const NodeType type) noexcept {
		while(value) {
			if(value->type == type) {
				break;
			}
			value = value->next;
		}
		return value != nullptr;
	}

	inline bool string_cmp(const char* name) const noexcept {
		const char* head = value->data.begin();
		while(*name && *head) {
			if(*name != *head) {
				return false;
			}
			name++;
			head++;
		}
		return *name == 0 && *head == '"';
	}

};

}; // namespace jjson
