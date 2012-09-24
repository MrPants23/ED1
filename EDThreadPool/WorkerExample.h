#ifndef _WORKER_EXAMPLE_H_
#define _WORKER_EXAMPLE_H_

#include <queue>

#include "WorkerThread.h"

/* an example class of how to subclass a WorkerThread and supply work to the thread pool
*/

namespace EDThreadPool
{
	class WorkerExample : public WorkerThread
	{
	public:
		int radix;
		int primesFound;

	public:
		WorkerExample(int _radix)
		{
			radix = _radix;
			primesFound = 0;
		}

		void DoWork()
		{
			for (int i = 2; i < radix; i++)
			{
				bool bIsPrime = true;
				for (int j = 2; j < (int)(radix / 2); j++)
				{
					// if the divisor has grown to size of int we are checking
					if (j >= i)
						break;

					// if it is divisible must not be prime
					if (i % j == 0)
					{
						bIsPrime = false;
						break;
					}
				}

				if (bIsPrime == true)
				{
					primesFound++;
				}
			}
		}
	};
}

#endif