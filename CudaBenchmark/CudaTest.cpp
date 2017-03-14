#include "CudaTest.h"
#include <Windows.h>
#include <math.h>
#include <iostream>
#include <ppl.h>
#include <vector>

// Sets up the CUDA test.
int setupCudaTestDbmToMw(float * result, float * data, unsigned int size);
// Runs the CUDA test.
int runCudaTest(float * results);

// Constructor
CudaTest::CudaTest()
{

}

// Destructor
CudaTest::~CudaTest()
{

}

// Converts the provided dBm value to mW.
// The power in milliwatts (P(mW)) is equal to 1mW times 10 raised by the 
// power in decibel-milliwatts (P(dBm)) divided by 10:
// P(mW) = 1mW * 10 ^ (P(dBm) / 10)
float convertDbmToMwHost(const float dBm)
{
  return powf(10.0f, dBm / 10.0f);
}

// Runs the main test.
void CudaTest::run()
{
  // Data set size (list size for testing)
  static const size_t dataSetSize = 1024 * 50;
  
  // Data set for testing.
  float * dataList_dBm = new float[dataSetSize];

  // Result of conversion operations.
  float * result_mW = new float[dataSetSize];
  
  // Initalize data set to arbitary values.
  static const float startingValue_dBm = -70.0f;
  static const float stepFactor_dBm = 2.0f;
  for (int i = 0; i < sizeof(dataList_dBm) / sizeof(dataList_dBm[0]); ++i)
  {
    dataList_dBm[i] = startingValue_dBm + (i * stepFactor_dBm);
  }

  std::cout << "Performing host operation...\n";

  m_timer.start();

  typedef std::vector<float> container;
  typedef container::iterator iter;

  container v(dataList_dBm, dataList_dBm + sizeof(dataList_dBm) / sizeof(dataList_dBm[0]));

  auto worker = [](iter begin, iter end) {
    for (auto it = begin; it != end; ++it) {
      *it = convertDbmToMwHost(*it);
    }
  };

  std::vector<std::thread> threads(8);
  const int grainsize = v.size() / 8;

  auto work_iter = std::begin(v);
  for (auto it = std::begin(threads); it != std::end(threads) - 1; ++it) {
    *it = std::thread(worker, work_iter, work_iter + grainsize);
    work_iter += grainsize;
  }
  threads.back() = std::thread(worker, work_iter, std::end(v));

  for (auto&& i : threads) {
    i.join();
  }
  //Concurrency::parallel_for(std::begin(v), std::end(v), worker);

  double elapsedVector_usec = m_timer.stop();
  std::cout << "Vector took " << elapsedVector_usec << " usec\n";

  // Start timing the code.
  if (m_timer.start() != -1)
  {
    // Run the timed host test.
    // Iterate over list of dBm values.
    for (int i = 0; i < sizeof(dataList_dBm) / sizeof(dataList_dBm[0]); ++i)
    {
      // Convert to mW and store result in output array.
      result_mW[i] = convertDbmToMwHost(dataList_dBm[i]);
    }

    // Stop timing the code.
    double elapsed_usec = m_timer.stop();

    // Output the result.
    std::cout << "Host operation complete!\n"
      << elapsed_usec << " usec elapsed.\n";
  }

  float * hostResults = new float[dataSetSize];
  memcpy(hostResults, result_mW, sizeof(hostResults));

  int cudaStatus = setupCudaTestDbmToMw(
    result_mW,
    dataList_dBm,
    sizeof(dataList_dBm) / sizeof(dataList_dBm[0]));

  if (cudaStatus != -1)
  {
    Timer cudaTimer;

    if (m_timer.start() != -1)
    {
      
      cudaTimer.start();

      // Runs the CUDA test.
      cudaStatus = runCudaTest(result_mW);
    }

    double elapsed_usec = cudaTimer.stop();

    // Stop timing the code.
    double elapsed_usec2 = m_timer.stop();

    // Output the result.
    std::cout << "GPU operation complete!\n"
      << elapsed_usec << " usec elapsed.\n";
  }

  delete[] result_mW;
  delete[] hostResults;
  delete[] dataList_dBm;
}