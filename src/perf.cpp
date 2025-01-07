// #include <cstdio>
//
// #include <jjson/jjson.h>
// #include <Tsc.h>
// #include <cassert>
// #include <chrono>
//
// #include <x86intrin.h>
//
// using namespace jjson;
//
// static inline long now_millisec() noexcept {
// 	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
// }
//
// static std::string bandwidth(const size_t bytes, const double sec) noexcept {
// 	double bw = bytes / sec;
// 	std::string postfix = "B/S";
// 	if(bw > 1024) {
// 		bw /= 1024.0;
// 		postfix = "KB/S";
// 	}
// 	if(bw > 1024) {
// 		bw /= 1024.0;
// 		postfix = "MB/S";
// 	}
// 	if(bw > 1024) {
// 		bw /= 1024.0;
// 		postfix = "GB/S";
// 	}
// 	return std::to_string(bw) + " " + postfix;
// }
//
// struct Dummy {
// 	size_t cnt_tokens = 0;
// 	size_t cnt_len = 0;
// 	void sax_event(SaxParserEvent event, const char* str, size_t str_len) {
// 		cnt_tokens++;
// 		cnt_len += str_len;
// 	}
// };
//
// int process_document(Document& doc) noexcept {
// 	Printer::dump(stdout, doc);
//
// 	return EXIT_SUCCESS;
// }
//
// template <typename T>
// int run_sax_parser(T& parser, const std::string& input, const bool is_dry_run) noexcept {
// 	int result = EXIT_FAILURE;
//
// 	const unsigned rounds = 1000u;
// 	unsigned cnt = rounds;
// 	bool res = true;
//
// 	const auto ms_before = now_millisec();
// 	const auto tsc_before = utils::Tsc::read();
// 	while(cnt) {
// 		parser.reset(input.c_str(), input.length());
// 		res = res && parser.parse();
// 		cnt--;
// 	}
//
// 	if(not is_dry_run) {
// 		const auto diff = utils::Tsc::read() - tsc_before;
// 		const auto ms_diff = now_millisec() - ms_before;
// 		const auto bytes_total = input.length() * rounds;
// 		const auto clk_per_run = diff / double(rounds);
// 		const auto clk_per_symbol = clk_per_run / double(input.length());
//
// 		if(res) {
// 			printf("<SaxParser>\n");
// 			printf("\tcnt_tokens       : %zu\n", parser.receiver().cnt_tokens);
// 			printf("\tcnt_len          : %zu\n", parser.receiver().cnt_len);
// 			printf("\tclock-per-symbol : %.2f\n", clk_per_symbol);
// 			printf("\tbandwidth        : %s\n", bandwidth(bytes_total, ms_diff / 1000.0).c_str());
// 		} else {
// 			fprintf(stderr, "%s\n", parser.error().c_str());
// 		}
// 	}
//
// 	return result;
// }
//
// void run_tokenizer(Tokenizer& tkn, const std::string& input, const bool is_dry_run) noexcept {
// 	size_t tkn_total = 0;
// 	size_t tkn_len_total = 0;
//
// 	const unsigned rounds = 1000u;
// 	unsigned cnt = rounds;
//
// 	const auto ms_before = now_millisec();
// 	const auto tsc_before = __rdtsc();
// 	while(cnt--) {
// 		tkn.reset(input);
// 		while(tkn.next()) {
// 			tkn_total++;
// 			tkn_len_total += tkn.token_len();
// 		}
// 	}
//
// 	if(not is_dry_run) {
//
// 		const auto tsc_diff = __rdtsc() - tsc_before;
// 		const auto ms_diff = now_millisec() - ms_before;
// 		const auto bytes_total = input.length() * rounds;
// 		const auto clk_per_run = tsc_diff / double(rounds);
// 		const auto clk_per_symbol = clk_per_run / double(input.length());
//
// 		printf("<Tokenizer>\n");
// 		printf("\ttokens-total     : %zu\n", tkn_total);
// 		printf("\ttokens-len-total : %zu\n", tkn_len_total);
// 		printf("\tclock-per-symbol : %.2f\n", clk_per_symbol);
// 		printf("\tms-total         : %ld\n", ms_diff);
// 		printf("\tbandwidth        : %s\n", bandwidth(bytes_total, ms_diff / 1000.0).c_str());
// 	}
// }
//
//
// int process_file(FILE* file) noexcept {
// 	fseek(file, 0, SEEK_END);
// 	const auto file_size = size_t(ftell(file));
// 	fseek(file, 0, SEEK_SET);
//
// 	std::string input;
// 	input.resize(file_size + 1u);
// 	input[file_size] = 0;
//
// 	if(fread(&(input.front()), file_size, 1, file) != 1) {
// 		fprintf(stderr, "File size mismatch.\n");
// 		return EXIT_FAILURE;
// 	}
//
// 	// Tokenizer tkn;
// 	// run_tokenizer(tkn, input, true);
// 	// run_tokenizer(tkn, input, false);
//
// 	Dummy dummy;
//
// 	SaxParser<Dummy> parser(dummy);
//
// 	run_sax_parser(parser, input, true);
// 	run_sax_parser(parser, input, false);
//
// 	return EXIT_SUCCESS;
// }
//
// int process_file_name(const char* file_name) noexcept {
// 	int result = EXIT_FAILURE;
// 	auto file = fopen(file_name, "r");
// 	if(file) {
// 		result = process_file(file);
// 		fclose(file);
// 	} else {
// 		fprintf(stderr, "File '%s' is not available for reading.\n", file_name);
// 	}
// 	return result;
// }
//
int main(int argc, char** argv) {

	// if(argc < 2) {
	// 	fprintf(stderr, "usage : json-file-name(s)\n");
	// 	return EXIT_FAILURE;
	// }
	//
	// int err;
	// for(int arg = 1; arg < argc; ++arg) {
	// 	const auto file_name = argv[arg];
	// 	err = process_file_name(file_name);
	// }

	// return err;
	return 0;
}

