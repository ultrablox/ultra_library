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

template<typename T>
class mersenne_checker
{
    using uint_t = T;
public:
    bool check_lucas_lehmer(unsigned p)
    {
        if(p == 2)
            return true;
        
        uint_t s(4);
        
        uint_t M(1);
        M = (M << p) - 1;
        //= pow(2, p) - 1;
        
        for(unsigned i = 0; i < p - 2; ++i)
        {
            std::cout << "Progress " << 100.0f * float(i) / (p-2) << '\%' << std::endl;
            s = ((s * s) - 2) % M;
        }
        
        return (s == 0);
    }
};

void check_prime(const std::string & num)
{
    std::size_t power_pos = num.find('^');
    std::size_t minus_pos = num.find('-');
    
    std::string power_strval = num.substr(power_pos + 1, minus_pos - power_pos - 1);
    
    long long power_val = atoll(power_strval.c_str());
    
    //mersenne_checker<unsigned> checker;
    mersenne_checker<big_uint> checker;
    
    if(checker.check_lucas_lehmer(power_val))
        std::cout << num << " is prime" << std::endl;
    else
        std::cout << num <<" is not prime" << std::endl;
}

void run_test(const char * file_name, unsigned line, bool val)
{
    if(!val)
        std::cout << "Test failed " << file_name << ':' << line << std::endl;
}

template<typename T, typename U>
void test_eq(const char * file_name, unsigned line, const T & lhs, const U & rhs)
{
    run_test(file_name, line, lhs == rhs);
}

#define TEST_CHECK(val) run_test(__FILE__, __LINE__, val)
#define TEST_CHECK_EQ(val1, val2) test_eq(__FILE__, __LINE__, val1, val2)

void run_tests()
{
    
    {
        big_uint bi("4");
        TEST_CHECK(bi.data[0] == 4);
    }
    
    {
        big_uint bi("111111");
        //TEST_CHECK(bi.data[0] == 4);
        std::cout << bi;
    }
    
    {
        big_uint bi("4000000000000000000");
        TEST_CHECK(bi.data[0] == 0x00);// {0x37, 0x82, 0xda, 0xce, 0x9d, 0x90, 0x00, 0x00});
    }
    
    TEST_CHECK_EQ(big_uint("100") * 10U, big_uint("1000"));
    
    TEST_CHECK_EQ(big_uint("4") + big_uint("3"), big_uint("7"));
    TEST_CHECK_EQ(big_uint("123") + big_uint("456"), big_uint("579"));
    {
        big_uint bi1("11111111111111"), bi2("11111111111111"), bi3("22222222222222");
        big_uint bi4 = bi1 + bi2;
        TEST_CHECK_EQ(big_uint("11111111111111") + big_uint("11111111111111"), big_uint("22222222222222"));
    }
    TEST_CHECK_EQ(big_uint("1000000000") * 4, big_uint("4000000000"));
    TEST_CHECK_EQ(big_uint("12") * big_uint("10"), big_uint("120"));
    TEST_CHECK_EQ(big_uint("1000000000") * big_uint("4000000000"), big_uint("4000000000000000000"));
    
    TEST_CHECK_EQ(big_uint("100") - big_uint("12"), big_uint("88"));
    TEST_CHECK_EQ(big_uint("22222222222222") - big_uint("11111111111111"), big_uint("11111111111111"));
    
    TEST_CHECK_EQ(big_uint("100") % big_uint("9"), big_uint("1"));
    TEST_CHECK_EQ(big_uint("17") % big_uint("3"), big_uint("2"));
    
    TEST_CHECK_EQ(big_uint("100") % big_uint("9"), big_uint("1"));
    TEST_CHECK_EQ(big_uint("17") % big_uint("3"), big_uint("2"));
    
    TEST_CHECK(big_uint("123711") > big_uint("118243"));
    TEST_CHECK_EQ(big_uint("123711") / big_uint("2231"), big_uint("55"));
    TEST_CHECK_EQ(big_uint("123711") % big_uint("2231"), big_uint("1006"));
}

int main(int argc, const char * argv[])
{
    //run_tests();
    
    /*assert(argc >= 3);
    std::string numA(argv[1]), numB(argv[2]);
    if(big_uint(numA) % big_uint(numB) == big_uint(0))
        std::cout << "divisible" << std::endl;
    else
        std::cout << "indivisible" << std::endl;*/
    
    
    /*auto mn = big_uint::create_mersenne(34);
    std::cout << mn << std::endl;
    
    for(unsigned i = 0; i < 1000; ++i)
    {
        check_prime("2^" + std::to_string(i) + "-1");
    }*/
    
    /*check_prime("2^7-1");
    check_prime("2^11-1");
    
    check_prime("2^17-1");
    check_prime("2^19-1");
    check_prime("2^521-1");
    check_prime("2^6972593-1");
    check_prime("2^20996011-1");
    check_prime("2^43112609-1");
    check_prime("2^74207281-1");
    
    std::cout << "-----------" << std::endl;
    
    
    check_prime("2^4-1");
    check_prime("2^6-1");
    check_prime("2^12-1");
    check_prime("2^500-1");
    check_prime("2^74207280-1");
    
    
    check_prime("2^500-1");*/
    
    if(argc < 2)
        return 1;
    std::string mers_num(argv[1]);
    check_prime(mers_num);
    return 0;
}
