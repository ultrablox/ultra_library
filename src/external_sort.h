
#ifndef EXTERNAL_SORT_H
#define EXTERNAL_SORT_H

#include "thread_pool.h"
#include <queue>
#include <type_traits>
#include <fstream>
#include <string>

template<typename T>
struct cached_stream_t
{
	using value_t = T;

	~cached_stream_t()
	{
		is.close();
		remove(fname.c_str());
	}

	void open(const std::string & file_name)
	{
		fname = file_name;
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
	std::string fname;
};

template<typename T, typename It, typename C>
void merge_files(It first, It last, C cmp, const std::string & out_file_name)
{
	using tmp_stream_t = cached_stream_t < T >;

	assert(std::distance(first, last) > 1);

	std::vector<tmp_stream_t> streams(std::distance(first, last));
	auto streams_cmp = [&](unsigned lhs, unsigned rhs){
		return !cmp(streams[lhs].val, streams[rhs].val);
	};

	std::priority_queue<unsigned, std::vector<unsigned>, decltype(streams_cmp)> streams_heap(streams_cmp);

	for (unsigned i = 0; i < streams.size(); ++i)
	{
		streams[i].open(*first++);
		streams[i].read_element();
		streams_heap.push(i);
	}

	std::ofstream res_file(out_file_name, std::ifstream::binary | std::ifstream::trunc);
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

template<typename T, typename C>
void external_sort(const std::string & file_name, const std::string & res_fname, C cmp, size_t filesize_limit = 1024U, unsigned max_streams_per_thread = 4)
{
	size_t file_element_count = filesize_limit / sizeof(T);
	const size_t tmp_file_size = file_element_count * sizeof(T);

	std::ifstream src_file(file_name, std::ifstream::binary);
	assert(src_file);

	std::ifstream * p_src_file = &src_file;
	src_file.seekg(0, std::ios::end);
	const size_t file_size = src_file.tellg();

	const size_t total_element_count = file_size / sizeof(T);
	assert(total_element_count * sizeof(T) == file_size);

	file_element_count = std::min<size_t>(file_element_count, total_element_count);

	unsigned file_count = file_size / tmp_file_size;
	if (file_size % tmp_file_size != 0)
		++file_count;

	std::mutex mtx;
	std::mutex * pmtx = &mtx;

	std::vector<std::string> file_names;
	file_names.reserve(file_count);
	auto * pnames = &file_names;

	thread_pool tp;
	for (unsigned i = 0; i < file_count; ++i)
	{
		tp.submit([=](){
			size_t addr = i * tmp_file_size;
			size_t local_el_count = (i == (file_count - 1)) ? (total_element_count - (file_element_count * (file_count - 1))) : file_element_count;
			std::vector<T> tmp_vec(local_el_count);
			std::string fname = "file_" + std::to_string(i) + ".dat";

			{
				std::lock_guard<std::mutex> grd(*pmtx);
				p_src_file->seekg(addr, std::ios::beg);
				p_src_file->read((char*)tmp_vec.data(), local_el_count * sizeof(T));

				assert(local_el_count * sizeof(T) == p_src_file->gcount());
				pnames->push_back(fname);
			}

			std::sort(tmp_vec.begin(), tmp_vec.end(), cmp);

			std::ofstream out_file(fname, std::ofstream::binary | std::ifstream::trunc);
			out_file.write((const char*)tmp_vec.data(), local_el_count * sizeof(T));
		});
	}

	tp.wait_busy();

	//Merge iterations
	unsigned merge_iteration = 0;
	while (file_names.size() > 1)
	{
		unsigned streams_per_thread = std::min<unsigned>(file_names.size() / tp.thread_count(), max_streams_per_thread);
		unsigned merge_count = streams_per_thread > 1 ? file_names.size() / streams_per_thread : 1;

		std::vector<std::string> new_fnames(merge_count);
		for (unsigned i = 0; i < merge_count; ++i)
		{
			std::string merged_fname = "file_" + std::to_string(merge_iteration) + "_" + std::to_string(i) + ".dat";
			tp.submit([=](){
				merge_files<T>(file_names.begin() + i*streams_per_thread, (i == merge_count - 1) ? file_names.end() : file_names.begin() + (i + 1)*streams_per_thread, cmp, merged_fname);
			});
			new_fnames[i] = merged_fname;
		}
		tp.wait_busy();

		std::swap(file_names, new_fnames);
		++merge_iteration;
	}

	tp.wait_finished();

	rename(file_names[0].c_str(), res_fname.c_str());
}

#endif
