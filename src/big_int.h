
#ifndef BIG_INT_H
#define BIG_INT_H

#include <vector>
#include <ostream>
#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <string>

//template<typename T = unsigned char>
class big_uint
{
public:
    using byte_t = unsigned char;
    
    big_uint()
    {}
    
    big_uint(const std::string & str_num)
    {
        big_uint tmp(0);
        
        big_uint mult(1);
        
        const unsigned s_count = str_num.size();
        for(unsigned i = 0; i < s_count; ++i)
        {
            char cur_dec_digit[2] = {str_num[str_num.size() - 1 - i], 0};
            byte_t digit = (byte_t)atol(cur_dec_digit);
            //big_uint added =mult * digit;
            tmp = tmp + mult * digit;
            //tmp = tmp + added;
            mult = mult * (byte_t)10;
        }
        
        this->data = tmp.data;
    }
    
    big_uint(unsigned small_val)
    {
        data.push_back(small_val);
    }
    
    static big_uint create_mersenne(unsigned p)
    {
        big_uint res(1);
        res = (res << p) - 1;
        return std::move(res);
    }
    
    bool max_bit() const
    {
        byte_t max_el = *data.rbegin();
        byte_t mask = byte_t(1) << ((byte_t)8 * sizeof(byte_t) - 1);
        return max_el & mask;
    }
    
    void shrink_head()
    {
        while((data.size() > 1) && (*data.rbegin() == 0))
            data.pop_back();
    }
    
    void divide(const big_uint & divisor, big_uint & res, big_uint & mod) const
    {
//        std::cout << "divide " << *this << '/' << divisor << std::endl;
        
		res.data.clear();

        mod = *this;
        unsigned divisor_digit_count = divisor.data.size();
        big_uint mult;
        unsigned digits_offset = 0;
        while(mod >= divisor)
        {
            //Check if we need additional digit offset
            big_uint tmp;
            tmp.data.insert(tmp.data.end(), mod.data.begin() + mod.data.size() - divisor_digit_count, mod.data.end());
            
            if(tmp < divisor)
            {
                tmp.data.clear();
                tmp.data.insert(tmp.data.end(), mod.data.begin() + mod.data.size() - 1 - divisor_digit_count, mod.data.end());
            }
            
            digits_offset = mod.data.size() - tmp.data.size();
            
            unsigned d = std::numeric_limits<byte_t>::max();
            for(; d > 0; --d)
            {
                mult = divisor * d;

                if(tmp >= mult)
                    break;
                
            }
            assert(d > 0);
            assert(mod >= mult);
            res.data.insert(res.data.begin(), d);
            mod = mod - (mult << (digits_offset * sizeof(byte_t) * 8));
        }
    }
    
    friend big_uint operator-(const big_uint & lhs, const big_uint & rhs)
    {
        big_uint res(lhs), extened_rhs(rhs);
        extened_rhs.data.resize(res.data.size(), (byte_t)0);
        
        unsigned carry = 0;
        for(unsigned i = 0; i < res.data.size(); ++i)
        {
            unsigned l_op = res.data[i], r_op = extened_rhs.data[i];
            unsigned diff = 0;
            if(r_op + carry > l_op)
            {
                diff = (unsigned)std::numeric_limits<byte_t>::max() + 1U + l_op - r_op - carry;
                carry = 1;
            }
            else
            {
                diff = l_op - r_op - carry;
                carry = 0;
            }
            
            assert(diff <= std::numeric_limits<byte_t>::max());
            
            res.data[i] = diff;
        }
        
        res.shrink_head();
        
        return std::move(res);
    }
    
    friend big_uint operator+(const big_uint & lhs, const big_uint & rhs)
    {
        big_uint res;
        res.data.resize(std::max(lhs.data.size(), rhs.data.size()));
        
        unsigned overflow = 0;
        for(unsigned i = 0; i < res.data.size(); ++i)
        {
            unsigned l_op = (lhs.data.size() <= i) ? 0 : lhs.data[i],
                    r_op = (rhs.data.size() <= i) ? 0 : rhs.data[i];
            
            unsigned sum = l_op + r_op + overflow;
            if(sum > (unsigned)std::numeric_limits<byte_t>::max())
                overflow = sum >> (sizeof(byte_t) * 8);
            else
                overflow = 0;

            res.data[i] = sum & std::numeric_limits<byte_t>::max();
        }
        
        if(overflow)
            res.data.push_back((byte_t)overflow);
            
        res.shrink_head();
        
        return std::move(res);
    }
    
    friend big_uint operator*(const big_uint & lhs, byte_t digit)
    {
        big_uint res;
        
        unsigned element_bit_count = sizeof(byte_t) * 8;
        unsigned least_mask = std::numeric_limits<byte_t>::max();
        
        unsigned overflow = 0;
        for(unsigned i = 0; i < lhs.data.size(); ++i)
        {
            unsigned val = lhs.data[i] * digit + overflow;
            if(val & (~least_mask))
                overflow = val >> element_bit_count;
            else
                overflow = 0;
            
            res.data.push_back((byte_t)(val & least_mask));
        }
        
        if(overflow > 0)
            res.data.push_back((byte_t)(overflow & least_mask));
            
        return std::move(res);
    }
    
    friend big_uint operator*(const big_uint & lhs, const big_uint & rhs)
    {
        big_uint res(0);
        
        for(unsigned i = 0; i < rhs.data.size(); ++i)
        {
            big_uint tmp = lhs * rhs.data[i] << (i * 8 * sizeof(byte_t));
            res = res + tmp;
        }
        
        return std::move(res);
    }
    
    friend big_uint operator<<(const big_uint & val, unsigned offset)
    {
        big_uint res(val);

		unsigned zero_digit_count = offset / (sizeof(byte_t) * 8);
		{
			std::vector<byte_t> tmp(zero_digit_count, 0);
			res.data.insert(res.data.begin(), tmp.begin(), tmp.end());
		}

		offset = offset % (sizeof(byte_t) * 8);
        
        while(offset--)
        {
            if(res.max_bit())
                res.data.push_back(0);
            
            bool overflow = false;
            for(auto & el : res.data)
            {
                bool new_overflow = (byte_t(1) << ((byte_t)8 * sizeof(byte_t) - 1)) & el;
                el = el << 1;
                if(overflow)
                    el = el | byte_t(1);
                overflow = new_overflow;
            }
        }
        
        return res;
    }
    
    friend big_uint operator%(const big_uint & lhs, const big_uint & rhs)
    {
        big_uint mod, div;
        lhs.divide(rhs, div, mod);
        return std::move(mod);
    }
    
    friend big_uint operator/(const big_uint & lhs, const big_uint & rhs)
    {
        big_uint mod, div;
        lhs.divide(rhs, div, mod);
        return std::move(div);
    }
    
    friend bool operator==(const big_uint & lhs, const big_uint & rhs)
    {
        return lhs.data == rhs.data;
    }

	friend bool operator!=(const big_uint & lhs, const big_uint & rhs)
	{
		return !(lhs == rhs);
	}
    
    friend bool operator>=(const big_uint & lhs, const big_uint & rhs)
    {
        return (!(lhs < rhs)) || (lhs == rhs);
    }
    
    friend bool operator>(const big_uint & lhs, const big_uint & rhs)
    {
        return !(lhs < rhs);
    }
    
    friend bool operator<(const big_uint & lhs, const big_uint & rhs)
    {
        if(lhs.data.size() == rhs.data.size())
        {
            for(unsigned i = 0; i < lhs.data.size(); ++i)
            {
                unsigned idx = lhs.data.size() - 1 - i;
                if(lhs.data[idx] != rhs.data[idx])
                {
                    return lhs.data[idx] < rhs.data[idx];
                }
                
            }

            return false;
        }
        else
            return lhs.data.size() < rhs.data.size();
    }
    
    friend std::ostream & operator<<(std::ostream & os, const big_uint & val)
    {
        /*for(unsigned i = 0; i < val.data.size(); ++i)
			os << std::hex << (unsigned)val.data[val.data.size() - 1 - i] << std::dec;*/

		std::string dec_num;

		big_uint ten(10);
		big_uint res(val), cur_res, cur_mod;

		while (res > 0)
		{
			res.divide(ten, cur_res, cur_mod);
			std::string dec_digit = std::to_string(cur_mod.data[0]);
			dec_num.push_back(dec_digit[0]);
			res = cur_res;
		}

		std::reverse(dec_num.begin(), dec_num.end());
		os << dec_num;
        return os;
    }

	static big_uint as_mersenne(const std::string & num)
	{
		std::size_t power_pos = num.find('^');
		std::size_t minus_pos = num.find('-');

		std::string power_strval = num.substr(power_pos + 1, minus_pos - power_pos - 1);

		long long power_val = atoll(power_strval.c_str());

		big_uint res(1);
		res = (res << power_val) - 1;
		return res;
	}
public:
    std::vector<byte_t> data;
    
};

#endif
