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

#include <filesystem>
#include <sstream>
#include <fstream>
#include <cassert>


#include "BH3D_GL.hpp"

#include "BH3D_GLCheckError.hpp"
#include "BH3D_Logger.hpp"

#ifdef OPENGL_DEBUG
namespace
{
	const std::filesystem::path DEBUG_FILE_PATH = "./Opengl_CheckError.log";

	std::string DEBUG_OPENGL_FUNC;
	std::string DEBUG_OPENGL_FILE;
	int DEBUG_OPENGL_LINE = 0;

	std::fstream& DebugLogStreamGL(const std::filesystem::path & debug_file) 
	{
		//Create a log file for opengl
		static std::fstream file;
		if(!file.is_open())
		{
			std::filesystem::create_directories(debug_file.parent_path());
			file.open(debug_file, std::fstream::out | std::fstream::trunc);
			assert(file.is_open());
		}
		return file;
	};


#ifdef glDebugMessageCallback

	void OpenglCallBackFunction(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei /*length*/,
		const GLchar *message,
		const void */*userParam*/)
	{

		// unsignifant code (like buffer NVIDIA buffer creation)
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
			return;


		//Get the opengl log file reference
		auto & ref_log = DebugLogStreamGL(DEBUG_FILE_PATH);
		assert(ref_log.is_open());

		//Some text alignement
		using SSETW = decltype (ref_log << std::setw(0));
		auto LOG_WIDTH = [&] (int n = 15) -> SSETW {
			return (ref_log << std::setw(n));
		};

		//fill the log file
		LOG_WIDTH(0) << "----- opengl-callback:" << std::endl;
		LOG_WIDTH() << "File: " << DEBUG_OPENGL_FILE << std::endl;
		LOG_WIDTH() << "Func: " << DEBUG_OPENGL_FUNC << std::endl;
		LOG_WIDTH() << "Line: " << DEBUG_OPENGL_LINE << std::endl;
		LOG_WIDTH() << "Debug message (" << id << "): " << message << std::endl;

		LOG_WIDTH() << "Source: ";
		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
			ref_log << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			ref_log << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			ref_log << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			ref_log << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:
			ref_log << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:
			ref_log << "Source: Other"; break;
		}
		ref_log << std::endl;

		LOG_WIDTH() << "Type: ";
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			ref_log << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			ref_log << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			ref_log << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:
			ref_log << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			ref_log << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:
			ref_log << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			ref_log << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:
			ref_log << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:
			ref_log << "Type: Other"; break;
		}
		ref_log << std::endl;

		LOG_WIDTH() << "Severity: ";
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			ref_log << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			ref_log << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:
			ref_log << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			ref_log << "Severity: notification"; break;
		}
		ref_log << std::endl<<std::endl;
	
	}

#endif

}

namespace bh3d
{

		std::string GLCheckError::GetError()
		{
			std::string strErrorGL;
			assert(glGetError != nullptr);
			GLenum error = glGetError();
			switch (error)
			{

				case GL_NO_ERROR: break;

#define CASE(name) case name: strErrorGL = #name; break

				CASE(GL_INVALID_ENUM);
				CASE(GL_INVALID_VALUE);
				CASE(GL_INVALID_OPERATION);
				CASE(GL_INVALID_FRAMEBUFFER_OPERATION);
				CASE(GL_OUT_OF_MEMORY);
				CASE(GL_STACK_UNDERFLOW);
				CASE(GL_STACK_OVERFLOW);

			default:
				break;
			}
			return strErrorGL;
		}

		void GLCheckError::LogMsgFileFuncLine(const char * file, const char *func, int line)
		{
			DEBUG_OPENGL_FUNC = func;
			DEBUG_OPENGL_FILE = file;
			DEBUG_OPENGL_LINE = line;
		}

		std::fstream & GLCheckError::GetLogDebugGL()
		{
			auto & ref = DebugLogStreamGL(DEBUG_FILE_PATH);
			assert(ref.is_open());
			return ref;
		}

		void GLCheckError::InitGLDebugMsg()
		{
#ifdef glDebugMessageCallback
			//Set the opengl callback function 
			assert(glDebugMessageCallback);		//Check if the opengl callback extension is supported (opengl 4.3+)
		
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenglCallBackFunction, nullptr);

			glDebugMessageControl(GL_DONT_CARE,
				GL_DONT_CARE,
				GL_DONT_CARE,
				0, nullptr, GL_TRUE);
#endif
		}
	}


#endif