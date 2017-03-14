
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <iostream>

// Kind of lame, but just put static file-level variables here for now.
// Pointer to device results array.
float * dev_result = 0;

// Pointer to device data array.
float * dev_data = 0;

// Size of data/result sets (i.e. number of entries in array).
unsigned int testArraySize = 0;

// GPU function to converts the provided dBm value to mW.
// The power in milliwatts (P(mW)) is equal to 1mW times 10 raised by the 
// power in decibel-milliwatts (P(dBm)) divided by 10:
// P(mW) = 1mW * 10 ^ (P(dBm) / 10)
__device__ float convertDbmToMw(const float dBm)
{
  return powf(10.0f, dBm / 10.0f);
}

// Convert dBm to mW kernel to run on the GPU.
__global__ void convertDbmToMwKernal(float * result, const float * data)
{
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  result[i] = convertDbmToMw(data[i]);
}

// Performs cleanup of allocated memory on GPU.
int cleanup()
{
  int status = 0;

  // Free any device allocated memory.
  if (cudaFree(dev_result) != cudaSuccess)
  {
    status = -1;
    std::cerr << "Failed to free dev_result!\n";
  }

  if (cudaFree(dev_data) != cudaSuccess)
  {
    status = -1;
    std::cerr << "Failed to free dev_data!\n";
  }

  // Reset the device.
  if (cudaDeviceReset() != cudaSuccess)
  {
    status = -1;
    std::cerr << "cudaDeviceReset failed!\n";
  }

  return status;
}

int setupCudaTestDbmToMw(float * result, float * data, unsigned int size)
{
  int status = 0;

  // Store off test set size.
  testArraySize = size;

  // Choose which GPU to run on, change this on a multi-GPU system.
  cudaError_t cudaStatus = cudaSetDevice(0);
  if (cudaStatus != cudaSuccess)
  {
    std::cerr << "cudaSetDevice failed!\n";
  }

  // Allocate GPU buffer for output (results).
  cudaStatus = cudaMalloc((void**)&dev_result, size * sizeof(int));
  if (cudaStatus != cudaSuccess)
  {
    std::cerr << "cudaMalloc failed!\n";
  }

  // Allocate GPU buffer for input (data).
  cudaStatus = cudaMalloc((void**)&dev_data, size * sizeof(int));
  if (cudaStatus != cudaSuccess)
  {
    std::cerr << "cudaMalloc failed!\n";
  }

  // Copy input vector from host memory to GPU buffer.
  cudaStatus = cudaMemcpy(dev_data, data, size * sizeof(float), cudaMemcpyHostToDevice);
  if (cudaStatus != cudaSuccess)
  {
    std::cerr << "cudaMemcpy failed!\n";
  }

  // If a failure occurred cleanup.
  if (cudaStatus != cudaSuccess)
  {
    // Indicate failure occurred.
    status = -1;

    // Attempt to perform cleanup.
    if (cleanup() == -1)
    {
      std::cerr << "cleanup failed!\n";
    }
  }

  return status;
}

// Runs the CUDA test.
int runCudaTest(float * results)
{
  int status = 0;

  // Launch a kernel on the GPU with one thread for each element.

  static const int blockSize = 512;

  int gridSize = (testArraySize + blockSize - 1) / blockSize;

  convertDbmToMwKernal<<<gridSize, blockSize>>>(dev_result, dev_data);

  // Check for any errors launching the kernel
  cudaError_t cudaStatus = cudaGetLastError();
  if (cudaStatus != cudaSuccess)
  {
    status = -1;
    std::cerr << "convertDbmToMwKernal launch failed " << cudaGetErrorString(cudaStatus) << std::endl;
  }

  // cudaDeviceSynchronize waits for the kernel to finish, and returns
  // any errors encountered during the launch.
  cudaStatus = cudaDeviceSynchronize();
  if (cudaStatus != cudaSuccess)
  {
    status = -1;
    std::cerr << "cudaDeviceSynchronize returned error code " << cudaStatus
      << " after launching addKernel!\n";
  }

  // Copy output vector from GPU buffer to host memory.
  cudaStatus = cudaMemcpy(results, dev_result, testArraySize * sizeof(float), cudaMemcpyDeviceToHost);
  if (cudaStatus != cudaSuccess)
  {
    status = -1;
    std::cerr << "cudaMemcpy failed!\n";
  }

  // Perform cleanup.
  if (cleanup() == -1)
  {
    status = -1;
    std::cerr << "cleanup failed!\n";
  }

  return status;
}

