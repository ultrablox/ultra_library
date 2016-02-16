
#ifndef ULTRA_LIB_PRIMES_H
#define ULTRA_LIB_PRIMES_H

#include <string>
#include <stdio.h>
#include <map>
#include "big_int.h"
#include "progress_indicator.h"

template<typename T>
class mersenne_checker
{
	using uint_t = T;
public:
	bool check_lucas_lehmer(unsigned p)
	{
		if (p == 2)
			return true;

		uint_t s(4);

		uint_t M(1);
		M = (M << p) - 1;
		//= pow(2, p) - 1;

		progress_indicator pi(p - 2);
		for (unsigned i = 0; i < p - 2; ++i)
		{
			//std::cout << "Progress " << 100.0f * float(i) / (p - 2) << '\%' << std::endl;
			pi.submit_progress(i);
			s = ((s * s) - 2) % M;
		}

		return (s == 0);
	}
};

bool is_mersenne_prime(const std::string & num)
{
	std::size_t power_pos = num.find('^');
	std::size_t minus_pos = num.find('-');

	std::string power_strval = num.substr(power_pos + 1, minus_pos - power_pos - 1);

	long long power_val = atoll(power_strval.c_str());

	//mersenne_checker<unsigned> checker;
	mersenne_checker<big_uint> checker;
	return checker.check_lucas_lehmer(power_val);
}

void factorize(size_t num, std::map<size_t, unsigned> & decomposition)
{
	size_t div = 2;

	while(div <= num)
	{
		while((num % div) == 0)
		{
			num /= div;
			auto it = decomposition.find(div);
			if(it == decomposition.end())
				decomposition.insert(std::make_pair(div, 1U));
			else
				++(it->second);
		}
		++div;
	}

}

#endif
