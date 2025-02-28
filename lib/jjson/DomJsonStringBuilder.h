#pragma once

#include <lib/jjson/type.h>

#include <cstdio>
#include <string>

namespace jjson {

struct DomJsonStringBuilder {

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
				case NodeType::Bool:
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
