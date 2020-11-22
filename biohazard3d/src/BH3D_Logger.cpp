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

#include <iostream>
#include <cassert>
#include <filesystem>

#include "BH3D_Logger.hpp"


namespace bh3d
{

	Logger::Logger(const std::filesystem::path & path_log, bool bind) : logFile(path_log)
	{
		// On verifie que le fichier est bien ouvert
		bool is_open = logFile.is_open();
		assert(is_open && ("Can't open the log file" + path_log.generic_string()).data());

		if(!is_open)
			return;

		// Inscription de l'en-tete du fichier
		logFile << "  ========================================" << std::endl;
		logFile << "   Biohazard3d - Logger File - OPEN " << std::endl;
		logFile << "  ========================================" << std::endl << std::endl;

		if (bind)
			Bind();
	}
	Logger::~Logger()
	{
		if (logFile.is_open())
		{
			logFile << std::endl;
			logFile << "  ========================================" << std::endl;
			logFile << "   Biohazard3d - Logger File - CLOSE " << std::endl;
			logFile << "  ========================================" << std::endl << std::endl;
		}
	}

	std::ofstream& Logger::getFileLogger()
	{ 
		assert(logFile.is_open());
		return logFile;
	}


}

