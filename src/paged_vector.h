#ifndef PAGED_VECTOR_H_
#define PAGED_VECTOR_H_

#include <vector>
#include <cstdlib>
#include <stdexcept>

template<typename T, unsigned PageSize = 1048576U>
class paged_vector
{
public:
	static const unsigned PAGE_SIZE = PageSize;
	using value_t = T;

	struct page
	{
		page(value_t empty_val = value_t())
			:data(PAGE_SIZE, empty_val)
		{}

		std::vector<value_t> data;
	};

	paged_vector(size_t element_count = 0, value_t default_val = value_t())
		:m_pages(1024, nullptr)
	{
		resize(element_count, default_val);
	}

	~paged_vector()
	{
		for(auto p : m_pages)
			delete p;
	}

	value_t & operator[](unsigned idx)
	{
		unsigned page_id = idx / PAGE_SIZE;

		if(!m_pages[page_id])
		{
			m_pages[page_id] = new page;
		}

		return m_pages[page_id]->data[idx % PAGE_SIZE];
	}

	const value_t & operator[](unsigned idx) const
	{
		unsigned page_id = idx / PAGE_SIZE;

		if(!m_pages[page_id])
			throw std::runtime_error("Out of bounds");

		return m_pages[page_id]->data[idx % PAGE_SIZE];
	}

	size_t size() const
	{
		unsigned i = 0;
		for(; (m_pages[i] != nullptr) && (i < m_pages.size()); ++i);
		return i * PAGE_SIZE;
	}

	void resize(size_t new_size, value_t empty_val = value_t())
	{
		unsigned page_id = new_size / PAGE_SIZE;

		for(unsigned i = 0; i <= page_id; ++i)
		{
			if(!m_pages[i])
			{
				m_pages[i] = new page(empty_val);
			}
		}

	}

	void clear()
	{
		for(auto & page : m_pages)
		{
			delete page;
			page = nullptr;
		}
	}
private:
	std::vector<page*> m_pages;
};

#endif
