
#ifndef ULTRA_LIB_LCS_H
#define ULTRA_LIB_LCS_H

#include <iostream>
#include "paged_vector.h"

struct lcs_table
{
	using cell_t = unsigned short;
	enum class step_t {left, up, left_up};

	//using container_t = std::vector < cell_t > ;
	using container_t = paged_vector < cell_t >;
	
	lcs_table(unsigned _width, unsigned _height)
		:width(_width), height(_height), pData(width * height, 0)
	{
		//memset(pData, 0, width * height * sizeof(cell_t));
	}

	~lcs_table()
	{
	}

	cell_t & at(unsigned x, unsigned y)
	{
		return pData[y * width + x];
	}

	template<typename It>
	step_t step_at(unsigned i, unsigned j, It x_first, It y_first)
	{
		if (*(x_first + i - 1) == *(y_first + j - 1))
			return step_t::left_up;
		else if (at(i - 1, j) >= at(i, j - 1))
			return step_t::up;
		else
			return step_t::left;
	}

	friend std::ostream & operator<<(std::ostream & os, lcs_table & table)
	{
		for (unsigned x = 0; x < table.width; ++x)
		{
			for (unsigned y = 0; y < table.height; ++y)
			{
				os << table.at(x, y) << ',';
			}

			os << std::endl;
		}
		return os;
	}

	unsigned width, height;
	container_t pData;
};

template<typename It, typename V>
void longest_common_subseq(It x_first, It x_last, It y_first, It y_last, V & dest)
{
	unsigned m = std::distance(x_first, x_last),
		n = std::distance(y_first, y_last);

	lcs_table ltable(m + 1, n + 1);

	for (unsigned i = 1; i <= m; ++i)
		ltable.at(i, 0) = 0;

	for (unsigned j = 0; j <= n; ++j)
		ltable.at(0, j) = 0;

	for (unsigned i = 1; i <= m; ++i)
	{
		for (unsigned j = 1; j <= n; ++j)
		{
			if (*(x_first + i - 1) == *(y_first + j - 1))
				ltable.at(i, j) = ltable.at(i - 1, j - 1) + 1;
			else if (ltable.at(i - 1, j) >= ltable.at(i, j - 1))
				ltable.at(i, j) = ltable.at(i - 1, j);
			else
				ltable.at(i, j) = ltable.at(i, j-1);
		}

		std::cout << "Building table... " << 100.0f * i / m << "%" << std::endl;
	}

	//std::cout << ltable << std::endl;
	
	unsigned subseq_size = ltable.at(m, n);
	dest.resize(subseq_size);

	auto dest_it = dest.rbegin();
	unsigned i = m, j = n;

	while ((i != 0) && (j != 0))
	{
		auto cur_step = ltable.step_at(i, j, x_first, y_first);
		if (cur_step == lcs_table::step_t::left_up)
		{
			*dest_it++ = *(x_first + i - 1);
			--i;
			--j;
		}
		else if (cur_step == lcs_table::step_t::up)
			--i;
		else
			--j;
	}
}

#endif
