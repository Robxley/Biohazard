
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
#ifndef _BH3D_GL_CHECK_ERROR_H_
#define _BH3D_GL_CHECK_ERROR_H_

#ifdef OPENGL_DEBUG

//Only use this define to check opengl error
#define BH3D_GL_CHECK_ERROR 			assert(glGenBuffers && "Opengl context have to be created before calling Opengl function"); bh3d::GLCheckError::LogMsgFileFuncLine(__FILE__,__func__,__LINE__)

#include <string>

namespace bh3d
{
	class GLCheckError
	{
	public:
		/// <summary>
		/// Get the opengl error. Return a empty string whitout error
		/// </summary>
		/// <returns>Return the result of glGetError</returns>
		static std::string GetError();

		/// <summary>
		/// Opengl is OK ?
		/// </summary>
		/// <returns>Check if a opengl error has occured</returns>
		static  bool OpenglOK() {
			return GetError().empty();
		}

		static void LogMsgFileFuncLine(const char * file, const char *func, int line);
		static std::fstream & GetLogDebugGL();
		static void InitGLDebugMsg();
	};
}
#else
#define BH3D_GL_CHECK_ERROR
#endif

#endif //_BH3D_GL_CHECK_ERROR_H_