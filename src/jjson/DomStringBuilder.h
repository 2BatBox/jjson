#pragma once

#include <jjson/type.h>

#include <cstdio>
#include <string>

namespace jjson {

struct DomStringBuilder {

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
			case NodeType::True :
				return "true";
			case NodeType::False :
				return "false";
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

	// static void dump(FILE* out, const jjson::Document& root) noexcept {
	// 	fprintf(out, "ROOT: value=%p\n", root.value);
	// 	dump(out, root.value, 0);
	// }

	static void dump(FILE* out, const jjson::Node* root, unsigned level, bool rec = true) noexcept {
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
			if(rec) {
				dump(out, root->value, level + 1u);
				dump(out, root->next, level);
			}
		}
	}

	static std::string to_json_string(const jjson::Node* root) noexcept {
		std::string result;
		while (root) {
			switch (root->type) {
				case NodeType::Object:
					result.push_back('{');
					result += to_json_string(root->value);
					result.push_back('}');
					break;

				case NodeType::Array:
					result.push_back('[');
					result += to_json_string(root->value);
					result.push_back(']');
					break;

				case NodeType::String:
					result.push_back('"');
					result.append(root->data);
					result.push_back('"');
					break;

				case NodeType::Number:
				case NodeType::True:
				case NodeType::False:
				case NodeType::Null:
					result.append(root->data);
					break;

				case NodeType::Key:
					result.push_back('"');
					result.append(root->data);
					result.push_back('"');
					result.push_back(':');
					result += to_json_string(root->value);
					break;

				case NodeType::Unknown:

				default:
					break;
			}

			if (root->next) {
				result.push_back(',');
			}

			root = root->next;
		}
		return result;
	}

};


}; // namespace jjson
