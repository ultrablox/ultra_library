
#include "tester.h"
#include "lcs.h"
#include <fstream>
#include <algorithm>

SURVEY_TEST(lcs_simple)
{
	std::string X = "ABCBDAB", Y = "BDCABA", res;
	longest_common_subseq(X.begin(), X.end(), Y.begin(), Y.end(), res);
	SURVEY_TEST_EQ(res, "BCBA");
}

template<typename C>
void load_string(const std::string & fname, C & str)
{
	std::ifstream t(fname);
	
	t.seekg(0, std::ios::end);
	unsigned file_size = t.tellg();
	std::cout << "Loading string with size=" << file_size << std::endl;
	str.reserve(file_size);
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	auto last_it = std::remove_if(str.begin(), str.end(), [](char c){
		return (c == '\n') || (c == '\r');
	});

	std::cout << "Removed " << std::distance(last_it, str.end()) << " returns" << std::endl;

	str.erase(last_it, str.end());
}

SURVEY_TEST(lcs_middle)
{
	return;
	std::vector<char> str1, str2;

	load_string("dna_1_small.txt", str1);
	load_string("dna_2_small.txt", str2);

	std::vector<char> res;

	longest_common_subseq(str1.begin(), str1.end(), str2.begin(), str2.end(), res);

	std::cout << "Found CLS of length=" << res.size() << std::endl;
	std::ofstream rfile("dna_cls_small.txt");
	for(char c : res)
		rfile << c;
}

SURVEY_TEST(lcs_complex)
{
	std::string str1, str2;

	load_string("dna_1.txt", str1);
	load_string("dna_2.txt", str2);

	std::vector<char> res;

	longest_common_subseq(str1.begin(), str1.end(), str2.begin(), str2.end(), res);

	std::cout << "Found CLS of length=" << res.size() << std::endl;
	std::ofstream rfile("dna_cls.txt");
	for(char c : res)
		rfile << c;
}