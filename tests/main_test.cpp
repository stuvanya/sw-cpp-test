#include "TestRunner.hpp"

// Pull in all test translation units via the linker.
// Each test file registers its TEST() cases at static-init time.

int main()
{
	return sw::test::runAll();
}
