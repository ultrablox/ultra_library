//
//  main.cpp
//  prime_checker
//
//  Created by Yuri Blokhin on 25/01/16.
//  Copyright © 2016 Yuri Blokhin. All rights reserved.
//

#include <iostream>
#include <math.h>
#include "big_int.h"
#include "tester.h"
#include "primes.h"

SURVEY_TEST(big_int)
{
    
    {
        big_uint bi("4");
		SURVEY_TEST_EQ(bi.data[0] == 4, true);
    }
    
    {
        big_uint bi("111111");
        //TEST_CHECK(bi.data[0] == 4);
        std::cout << bi;
    }
    
    {
        big_uint bi("4000000000000000000");
		SURVEY_TEST_EQ(bi.data[0] == 0x00, true);// {0x37, 0x82, 0xda, 0xce, 0x9d, 0x90, 0x00, 0x00});
    }
    
	SURVEY_TEST_EQ(big_uint("100") * 10U, big_uint("1000"));
    
	SURVEY_TEST_EQ(big_uint("4") + big_uint("3"), big_uint("7"));
	SURVEY_TEST_EQ(big_uint("123") + big_uint("456"), big_uint("579"));
    {
        big_uint bi1("11111111111111"), bi2("11111111111111"), bi3("22222222222222");
        big_uint bi4 = bi1 + bi2;
		SURVEY_TEST_EQ(big_uint("11111111111111") + big_uint("11111111111111"), big_uint("22222222222222"));
    }
	SURVEY_TEST_EQ(big_uint("1000000000") * 4, big_uint("4000000000"));
	SURVEY_TEST_EQ(big_uint("12") * big_uint("10"), big_uint("120"));
	SURVEY_TEST_EQ(big_uint("1000000000") * big_uint("4000000000"), big_uint("4000000000000000000"));
    
	SURVEY_TEST_EQ(big_uint("100") - big_uint("12"), big_uint("88"));
	SURVEY_TEST_EQ(big_uint("22222222222222") - big_uint("11111111111111"), big_uint("11111111111111"));
    
	SURVEY_TEST_EQ(big_uint("100") % big_uint("9"), big_uint("1"));
	SURVEY_TEST_EQ(big_uint("17") % big_uint("3"), big_uint("2"));
    
	SURVEY_TEST_EQ(big_uint("100") % big_uint("9"), big_uint("1"));
	SURVEY_TEST_EQ(big_uint("17") % big_uint("3"), big_uint("2"));
    
	SURVEY_TEST_EQ(big_uint("123711") > big_uint("118243"), true);
	SURVEY_TEST_EQ(big_uint("123711") / big_uint("2231"), big_uint("55"));
	SURVEY_TEST_EQ(big_uint("123711") % big_uint("2231"), big_uint("1006"));
}

SURVEY_TEST(mersenne_primes)
{
	SURVEY_TEST_EQ(is_mersenne_prime("2^17-1"), true);
	//SURVEY_TEST_EQ(is_mersenne_prime("2^500-1"), false);

	auto bi = big_uint::as_mersenne("2^6972593-1");
	//std::cout << bi << std::endl;

//	SURVEY_TEST_EQ(is_mersenne_prime("2^6972593-1"), true);
}
