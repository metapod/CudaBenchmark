#include "Timer.h"
#include <windows.h>
#include <iostream>

// Constructor
Timer::Timer() :
m_counterStart(0),
m_frequency(0.0)
{

}

// Destructor
Timer::~Timer()
{

}

// Starts the timer.
// Returns -1 upon failure.
int Timer::start()
{
  // Large integer used to query performance methods.
  LARGE_INTEGER queryResult = { 0 };

  // Query the performance frequency.
  if (!QueryPerformanceFrequency(&queryResult))
  {
    std::cerr << "QueryPerformanceFrequency failed!\n.";
    return -1;
  }

  // Microseconds frequency
  m_frequency = static_cast<double>(queryResult.QuadPart) / 1000000.0;

  // "Start" the timer by getting the current tick count.
  if (!QueryPerformanceCounter(&queryResult))
  {
    std::cerr << "QueryPerformanceCounter failed!\n";
    return -1;
  }

  // Update the counter start time.
  m_counterStart = queryResult.QuadPart;
  return 0;
}

// Stops the timer and returns the result.
// Returns 0.0 on failure.
// Warning - does not check if timer was previously started!
double Timer::stop()
{
  // Large integer used to query performance methods.
  LARGE_INTEGER queryResult = { 0 };

  // Get the current counter ticks.
  if (!QueryPerformanceCounter(&queryResult))
  {
    std::cerr << "QueryPerformanceCounter failed!\n";
    return 0.0;
  }

  // Return (current ticks - start ticks) / frequency
  return static_cast<double>(queryResult.QuadPart - m_counterStart) / m_frequency;
}
