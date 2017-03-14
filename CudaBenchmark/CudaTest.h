#pragma once

#include "Timer.h"

class CudaTest
{
public:
	
	// Constructor.
	CudaTest();
	
	// Destructor.
	~CudaTest();

	// Runs the main test.
	void run();

private:

  // Timer used to time the testing.
  Timer m_timer;
};

