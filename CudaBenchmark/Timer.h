#pragma once

class Timer
{

public:
  
  // Constructor.
  Timer();

  // Destructor.
  ~Timer();

  // Starts the timer.
  // Returns -1 on failure.
  int start();

  // Stops the timer and returns the result (usec).
  // Returns 0.0 on failure.
  // Warning - does not check if timer was previously started!
  double stop();

private:

  // Clock frequency.
  double m_frequency;

  // Starting value for timer.
  __int64 m_counterStart;

};

