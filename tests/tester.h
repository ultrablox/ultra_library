
#ifndef SURVEY_TESTER_H
#define SURVEY_TESTER_H

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include "USSingleton.h"

#define REAL_EPS 0.001

template<typename T>
void test_check_eq(const std::string & msg, const T & checked_val, const T & correct_val)
{
	if (checked_val != correct_val)
		std::cout << "Test failed: " << msg << std::endl;
}

struct test_invoker_base
{
	virtual void run() = 0;
};

class uav_tester
{
public:
	using test_func_t = void(*)(void);

	uav_tester()
		:errorCount(0)
	{}
	
	int exec()
	{
		std::cout << "Running " << invokers.size() << " test cases" << std::endl;

		for (auto inv : invokers)
		{
			try
			{
				inv->run();
			}
			catch (std::exception & ex)
			{
				++errorCount;
				std::cout << "Test terminated with exception: " << ex.what() << std::endl;
				//throw;
			}
			catch (...)
			{
				++errorCount;
				std::cout << "Test terminated with unknown exception" << std::endl;
				throw;
			}
		}
			

		if (errorCount)
		{
			std::cout << "Test failed: total " << errorCount << " errors" << std::endl;
		}
		else
		{
			std::cout << "All tests passed" << std::endl;
		}
		return 0;
	}

	template<typename T, typename U>
	void check_eq(const char * file_name, unsigned line_number, const T & checked_val, const U & correct_val)
	{
		if (checked_val != correct_val)
		{
			/*std::stringstream ss;
			ss << "not equal:" << checked_val << " != " << correct_val;
			submit_error(file_name, line_number, ss.str());*/
			submit_error(file_name, line_number, "not equal");
		}
	}

	template<typename T>
	void check_eq_eps(const char * file_name, unsigned line_number, const T & checked_val, const T & correct_val)
	{
		if (abs(checked_val - correct_val) > REAL_EPS)
		{
			submit_error(file_name, line_number, "not equal");
		}
	}
	

	template<typename T>
	void check_le(const char * file_name, unsigned line_number, const T & checked_val, const T & correct_val)
	{
		if (checked_val > correct_val)
		{
			/*std::stringstream ss;
			ss << checked_val << " != " << correct_val;
			submit_error(file_name, line_number, ss.str());*/
			submit_error(file_name, line_number, "not less or equal");
		}
	}

	template<typename T, typename U>
	void check_le_eps(const char * file_name, unsigned line_number, const T & v1, const U & v2)
	{
		if (v1 > v2)
		{
			/*std::stringstream ss;
			ss << checked_val << " != " << correct_val;
			submit_error(file_name, line_number, ss.str());*/
			submit_error(file_name, line_number, "not less or equal");
		}
	}
	
	template<typename T, typename U>
	void check_neq(const char * file_name, unsigned line_number, const T & checked_val, const U & correct_val)
	{
		if (checked_val == correct_val)
		{
			/*std::stringstream ss;
			ss << checked_val << " == " << correct_val;
			submit_error(file_name, line_number, ss.str());*/
			submit_error(file_name, line_number, "equal");
		}
	}

private:
	void submit_error(const char * file_name, unsigned line_number, const char * error_text)
	{
		++errorCount;
		std::cout << "Error (" << file_name << ':' << line_number << "): " << error_text << std::endl;
	}
public:
	std::vector<test_invoker_base*> invokers;
private:
	unsigned errorCount;
};

typedef USAutocreateSingleton<uav_tester> tester_singleton;

#define SURVEY_TEST_NEQ(exp1, exp2) CREATE_UAV_TEST_NEQ(__FILE__, __LINE__, exp1, exp2)
#define CREATE_UAV_TEST_NEQ(filename, linenum, exp1, exp2) tester_singleton::singleton()->check_neq(filename, linenum, exp1, exp2)

#define SURVEY_TEST_EQ(exp1, exp2) CREATE_SURVEY_TEST_EQ(__FILE__, __LINE__, exp1, exp2)
#define CREATE_SURVEY_TEST_EQ(filename, linenum, exp1, exp2) tester_singleton::singleton()->check_eq(filename, linenum, exp1, exp2)

#define SURVEY_TEST_EQ_EPS(exp1, exp2) CREATE_SURVEY_TEST_EQ_EPS(__FILE__, __LINE__, exp1, exp2)
#define CREATE_SURVEY_TEST_EQ_EPS(filename, linenum, exp1, exp2) tester_singleton::singleton()->check_eq_eps(filename, linenum, exp1, exp2)

#define SURVEY_TEST_LE(exp1, exp2) CREATE_SURVEY_TEST_LE(__FILE__, __LINE__, exp1, exp2)
#define CREATE_SURVEY_TEST_LE(filename, linenum, exp1, exp2) tester_singleton::singleton()->check_le(filename, linenum, exp1, exp2)

#define SURVEY_TEST_LE_EPS(exp1, exp2) CREATE_SURVEY_TEST_LE_EPS(__FILE__, __LINE__, exp1, exp2)
#define CREATE_SURVEY_TEST_LE_EPS(filename, linenum, exp1, exp2) tester_singleton::singleton()->check_le_eps(filename, linenum, exp1, exp2)

#define SURVEY_TEST(test_name) CREATE_UAV_TEST(uav_test_##test_name)
#define CREATE_UAV_TEST(struct_name) struct struct_name : public test_invoker_base{\
						struct_name(){\
							tester_singleton::singleton()->invokers.push_back(this);\
						}\
						virtual void run() override;\
						static struct_name invoker_instance;\
                   };\
				   struct_name struct_name::invoker_instance;\
				void struct_name::run()


#endif
