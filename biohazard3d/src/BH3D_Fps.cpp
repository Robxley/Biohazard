/*
 * Biohazard3D
 * The MIT License
 *
 * Copyright 2014 Robxley (Alexis Cailly).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <ctime>
#include <cmath>

#include "BH3D_Fps.hpp"

namespace bh3d
{

	namespace
	{
		constexpr Fps::chronotime one_second{ 1000000 };
	}



	void Fps::Compute()
	{
		auto t = std::chrono::high_resolution_clock::now();
		elapse_t = std::chrono::duration_cast<chronotime>(t - start_t);
		start_t = t;

		//fps moyen
		sum_elapse_t += elapse_t;
		
		if (sum_elapse_t >= one_second)
		{
			double ratio = (double)one_second.count() / (double)sum_elapse_t.count();
			fps = (unsigned int) std::round(count * (ratio));
			count = 0;
			sum_elapse_t = chronotime::zero();
		}
		else {
			count++;
		}
	}
}
