#include <iostream>
#include "CudaTest.h"

// The main function of this program.
int main()
{
	std::cout << "Starting CUDA benchmark...\n";

  // Attempt to allocate and run the test.
  try
  {
    CudaTest * testProgram = new CudaTest();
    testProgram->run();
    delete testProgram;
  }
  catch (...)
  {
    std::cout << "Error! Exception occurred during test!\n";
  }

  // Inform user the test is complete.
	std::cout << "Benchmark complete.\n"
		<< "Press enter to continue...\n";
	std::cin.get();
	return 0;
}