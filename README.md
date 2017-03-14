# CudaBenchmark
A benchmark to see if using the GPU is faster for [power conversions](https://en.wikipedia.org/wiki/DBm#Unit_conversions).

I attended a hackathon at work whose theme was GPU processing using nvidia Cuda (or OpenCL). I had never worked with GPUs before and created this little "hello world" type benchmark project before attending.

The program does a bunch of power conversions (from dBm to mW) on the host CPU and then on the GPU and outputs the execution time.

It turns out that using the GPU for these type of conversions is slower than the CPU.

### Environment
Install Microsoft Visual Studio. The hackathon used Visual Studio 2010, at home Visual Studio 2015 Community Edition is free and works fine. Install the latest version of the Cuda SDK from nvidia. Latest is 8.0, I believe hackathon used 6.0. There is a Cuda SDK version and also a "compute ability" number assigned to each GPU (model).

### Cuda
Useful for doing complex math operations on large sets of data. For example image processing such as edge detection algorithms. Not good for doing simple operations on sets of data, in such cases it is faster to just use the CPU.

Alternative is OpenCL. Open support for AMD, nvidia, or intel. Problem is that it offers less good tools (and probably performance) but is more open. That said, nvidia essentially dominates the GPU market currently and offers very good support to commercial outfits using their tech.

### Project Structure
* Main.cpp  
  * creates an instance of CudaTest  
  * runs the Cuda test  
* CudaTest.cpp  
* CudaTest.h  
  * Creates a Timer
  * __HOST TEST__
    * Sets up test
    * Starts Timer
    * Executes test on host (CPU)
    * Outputs result (stops timer)
  * __GPU TEST__
    * Sets up test on GPU
    * Starts Timer
    * Executes test on GPU by calling interface
    * Outputs result (stops timer)
* Timer.cpp
* Timer.h
  * Provides performance timer (usec resolution) using either Windows functions or POSIX
* cudaTestingKernel.cu
  * Function definitions for executing on the GPU  
  * `__device__` <-- device helper functions that can be called by GPU code  
  * `__global__` <-- functions that are executed as a "kernel" on the GPU  
