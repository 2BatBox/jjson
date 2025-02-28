#include <cstdio>

#include <lib/jjson/jjson.h>
#include <cassert>

using namespace jjson;

bool test_sax_string_builder(const std::string& input) noexcept {
	bool result = false;
	SaxStringBuilder builder;
	SaxParser parser(builder);
	result = parser.parse(input);
	if (result) {
		const std::string& output = builder.output();
		result = (strcmp(input.data(), output.data()) == 0);
		if (not result) {
			fprintf(stderr, "SaxStringBuilder test has failed : the input and output strings are not the same!\n");
			fprintf(stderr, "input  : '%s'\n", input.c_str());
			fprintf(stderr, "output : '%s'\n", output.c_str());
		}
	}
	else {
		const std::string& output = parser.error();
		fprintf(stderr, "SaxStringBuilder test has failed during the parsing!\n");
		fprintf(stderr, "input  : '%s'\n", input.c_str());
		fprintf(stderr, "error  : '%s'\n", parser.error().c_str());
	}
	return result;
}

bool test_dom_string_builder(const std::string& input) noexcept {
	bool result = false;
	DomBuilder dom(1024 * 1024);
	SaxParser parser(dom);
	result = parser.parse(input);
	if (result) {
		const auto root = dom.root();
		const std::string& output = DomJsonStringBuilder::to_json_string(root);
		result = (strcmp(input.data(), output.data()) == 0);
		if (not result) {
			fprintf(stderr, "DomStringBuilder test has failed : the input and output strings are not the same!\n");
			fprintf(stderr, "input  : '%s'\n", input.c_str());
			fprintf(stderr, "output : '%s'\n", output.c_str());
		}
	} else {
		if(dom.is_allocation_reject()) {
			fprintf(stderr, "DomStringBuilder test has failed, the node pool is empty!\n");
		} else {
			const std::string& output = parser.error();
			fprintf(stderr, "DomStringBuilder test has failed during the parsing!\n");
			fprintf(stderr, "input  : '%s'\n", input.c_str());
			fprintf(stderr, "error  : '%s'\n", output.c_str());
		}

	}
	return result;
}


void remove_junk(std::string& input) {
	while (input.size() > 0) {
		switch (input.back()) {
			case 0:
			case ' ':
			case '\r':
			case '\n':
				input.pop_back();
			continue;

			default:
				break;
		}
		break;
	}
}

bool process_file(FILE* file) noexcept {
	fseek(file, 0, SEEK_END);
	const auto file_size = size_t(ftell(file));
	fseek(file, 0, SEEK_SET);

	std::string input;
	input.resize(file_size + 1u);
	input[file_size] = 0;

	bool result = (fread(&(input.front()), file_size, 1, file) == 1);
	if(result) {
		// Remove all the junk which "SMART EDITORS" put at the end of the file.
		remove_junk(input);
		result = test_sax_string_builder(input) && test_dom_string_builder(input);
	} else {
		fprintf(stderr, "File size mismatch.\n");
	}

	return result;

}

int process_file_name(const char* file_name) noexcept {
	int result = EXIT_FAILURE;
	auto file = fopen(file_name, "r");
	if (file) {
		result = process_file(file);
		fclose(file);
	}
	else {
		fprintf(stderr, "File '%s' is not available for reading.\n", file_name);
	}
	return result;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		fprintf(stderr, "usage : json-file-name(s)\n");
		return EXIT_FAILURE;
	}

	int err = EXIT_SUCCESS;
	for (int arg = 1; arg < argc; ++arg) {
		const auto file_name = argv[arg];
		if (not process_file_name(file_name)) {
			fprintf(stderr, "The test has filed at file '%s'\n", file_name);
			err = EXIT_FAILURE;
			break;
		}
	}

	return err;
}
