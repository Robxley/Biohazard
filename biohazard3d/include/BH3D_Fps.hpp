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

#pragma once
#ifndef _BH3D_FPS_H_
#define _BH3D_FPS_H_

#include <chrono>

namespace bh3d
{
	class Fps
	{
	public:

		using chronotime = std::chrono::microseconds;

		using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

		using millisecond =
			std::chrono::duration<float, std::chrono::milliseconds::period>;

		using second =
			std::chrono::duration<double, std::chrono::seconds::period>;

		Fps() = default;

		/// <summary>
		/// Main function to call in the main loop.
		/// </summary>
		void Compute();

		
		/// <summary>
		/// Frame per second
		/// </summary>
		/// <returns>fps value</returns>
		inline unsigned int GetFps() const {
			return fps;
		}
	
		/// <summary>
		/// Get the duration time between two calls of compute function
		/// </summary>
		/// <returns>Elapse Time in microsecond</returns>
		inline const auto & GetElapseTime() const {
			return elapse_t;
		}

		/// <summary>
		/// Get the duration time between two calls of compute function
		/// </summary>
		/// <returns>Elapse Time in millisecond</returns>
		inline auto GetElapseTimeMillisecond() const {
			return millisecond(elapse_t).count();
		}

		/// <summary>
		/// Get the duration time between two calls of compute function
		/// </summary>
		/// <returns>Elapse Time in second</returns>
		inline auto GetElapseTimeSecond() const {
			return second(elapse_t).count();
		}

	private:

		chronotime elapse_t = chronotime::zero();		//time between two images
		chronotime sum_elapse_t = chronotime::zero();	//time accumulation waiting one second

		unsigned int fps = 60;								//image number for one second
		unsigned int count = 0;
		time_point start_t = std::chrono::high_resolution_clock::now(); 
		time_point end_t;
		
	};


}
#endif //
