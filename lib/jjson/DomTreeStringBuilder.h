#pragma once

#include <lib/jjson/type.h>

#include <cstdio>
#include <string>

namespace jjson {

struct DomTreeStringBuilder {

	static void dump(FILE* out, const jjson::Node* root) noexcept {
		dump(out, root, 0);
	}

private:

	static void dump(FILE* out, const jjson::Node* root, unsigned level) noexcept {
		if(root) {
			fprintf(out, "%*s [%u] ", int(level * 4), "", level);
			fprintf(out, " %s ", to_string(root->type));

			switch(root->type) {
				case NodeType::Key :
				case NodeType::String :
				case NodeType::Number :
					fprintf(out, " '%.*s'", int(root->data.length()), root->data.begin());
				break;

				default:
					break;
			}

			fprintf(out, "\n");
			dump(out, root->value, level + 1u);
			dump(out, root->next, level);
		}
	}

	static const char* to_string(const jjson::NodeType& value) noexcept {
		switch(value) {
			case NodeType::Object :
				return "object";
			case NodeType::Array :
				return "array";
			case NodeType::String :
				return "string";
			case NodeType::Number :
				return "number";
			case NodeType::Bool :
				return "bool";
			case NodeType::Null :
				return "null";
			case NodeType::Key :
				return "key";
			case NodeType::Unknown :
				return "unknown";

			default :
				return "UNKNOWN";
		}
	}

};


}; // namespace jjson
