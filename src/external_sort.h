
#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include "thread_pool.h"
#include <queue>
#include <type_traits>


template<typename T>
struct cached_stream_t
{
	using value_t = T;

	void open(const std::string & file_name)
	{
		is.open(file_name.c_str(), std::ios_base::binary);
		assert(is);
	}

	void read_element()
	{
		is.read((char*)&val, sizeof(value_t));
		unsigned sk = is.tellg();
	}

	bool finished() const
	{
		return is.eof();
	}

	value_t val;
	std::ifstream is;
};

template<typename T, typename C>
void external_sort(const std::string & file_name, const std::string & res_fname, C cmp, size_t filesize_limit = 1024U)
{
	const size_t file_element_count = filesize_limit / sizeof(T);
	const size_t tmp_file_size = file_element_count * sizeof(T);

	thread_pool tp;

	std::ifstream src_file(file_name, std::ifstream::binary);
	std::ifstream * p_src_file = &src_file;
	src_file.seekg(0, std::ios::end);
	const size_t file_size = src_file.tellg();

	const size_t total_element_count = file_size / sizeof(T);

	unsigned file_count = file_size / tmp_file_size;
	if (file_size % tmp_file_size != 0)
		++file_count;

	std::mutex src_file_read;
	std::mutex * pmtx = &src_file_read;

	for (unsigned i = 0; i < file_count; ++i)
	{
		tp.submit([=](){
			size_t addr = i * tmp_file_size;
			size_t local_el_count = (i == (file_count - 1)) ? (total_element_count - (file_element_count * (file_count - 1))) : file_element_count;
			std::vector<T> tmp_vec(local_el_count);
			
			{
				std::lock_guard<std::mutex> grd(*pmtx);
				p_src_file->seekg(addr, std::ios::beg);
				p_src_file->read((char*)tmp_vec.data(), local_el_count * sizeof(T));

				assert(local_el_count * sizeof(T) == p_src_file->gcount());
			}

			std::sort(tmp_vec.begin(), tmp_vec.end(), cmp);

			std::ofstream out_file("file_" + std::to_string(i) + ".dat", std::ofstream::binary | std::ifstream::trunc);
			out_file.write((const char*)tmp_vec.data(), local_el_count * sizeof(T));
		});
	}

	tp.wait_finished();

	using tmp_stream_t = cached_stream_t < T > ;

	std::vector<tmp_stream_t> streams(file_count);
	auto streams_cmp = [&](unsigned lhs, unsigned rhs){
		return !cmp(streams[lhs].val, streams[rhs].val);
	};

	std::priority_queue<unsigned, std::vector<unsigned>, decltype(streams_cmp)> streams_heap(streams_cmp);

	for (unsigned i = 0; i < streams.size(); ++i)
	{
		streams[i].open("file_" + std::to_string(i) + ".dat");
		streams[i].read_element();
		streams_heap.push(i);
	}

	std::ofstream res_file(res_fname, std::ifstream::binary | std::ifstream::trunc);
	while (!streams_heap.empty())
	{
		unsigned top_idx = streams_heap.top();
		streams_heap.pop();

		tmp_stream_t & cur_stream = streams[top_idx];
		res_file.write((const char*)&cur_stream.val, sizeof(T));

		cur_stream.read_element();

		if (!cur_stream.finished())	
			streams_heap.push(top_idx);
	}
}

#endif
