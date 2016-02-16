
#include <external_sort.h>

int main(int argc, char** argv)
{
	assert(argc > 2);
	external_sort<unsigned>(std::string(argv[1]), std::string(argv[2]), std::less<unsigned>(), 262144, 200);
	return 0;
}