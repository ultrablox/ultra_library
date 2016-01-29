
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

void load_string(const std::string & fname, std::string & str)
{
	std::ifstream t(fname);
	
	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	auto last_it = std::remove_if(str.begin(), str.end(), [](char c){
		return c == '\n';
	});

	str.erase(last_it, str.end());
}

SURVEY_TEST(lcs_complex)
{
	std::string str1, str2;

	load_string("dna_1.txt", str1);
	load_string("dna_2.txt", str2);

	std::vector<char> res;

	longest_common_subseq(str1.begin(), str1.end(), str2.begin(), str2.end(), res);
}