#include "tester.h"
#include "external_sort.h"
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>

using namespace std;

SURVEY_TEST(test_external_sort)
{
  cout << "Testing external sort..." << std::endl;

  //Generate sample data
  std::default_random_engine generator;
  std::uniform_int_distribution<size_t> val_distr(10000ULL, 4294967295ULL);//4294967295ULL
  
  unsigned item_count = 1000;
  
  std::vector<unsigned> correct_vec(item_count);
  for(int j = 0; j < item_count; ++j)
      correct_vec[j] = val_distr(generator);

  {
	  std::ofstream dump_file("test_file.dat", std::ofstream::binary);
	  dump_file.write((const char*)correct_vec.data(), sizeof(unsigned) * correct_vec.size());
  }
  
  cout << "Sorting correct vector..." << std::endl;
  std::sort(correct_vec.begin(), correct_vec.end());

  cout << "Sorting with external sort...." << std::endl;

  external_sort<unsigned>("test_file.dat");

  cout << "Comparing results..." << std::endl;
  {
      std::ifstream in_file("test_file.dat", std::ifstream::binary);

      std::vector<unsigned> new_vec(item_count);
      in_file.read((char*)new_vec.data(), sizeof(unsigned) * new_vec.size());

      SURVEY_TEST_EQ(new_vec, correct_vec);
  }
}
