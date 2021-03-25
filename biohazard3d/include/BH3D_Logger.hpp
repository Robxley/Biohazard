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

#define BH3D_VERBOSE

#ifndef BH3D_VERBOSE

#define BH3D_LOGGER(msg)		
#define BH3D_LOGGER_ERROR(msg)
#define BH3D_LOGGER_WARNING(msg)

#else

#if defined(__ANDROID__) || defined(__IPHONEOS__)
#define BH3D_USE_SDL_LOGGER
#elif defined(WIN32)
#define BH3D_USE_COUNT_AND_FILE_LOGGER
#else
#define BH3D_USE_FILE_LOGGER
#endif

#include <iomanip>

 //Don't use it directly perfer used BH3D_LOGGER, BH3D_LOGGER_ERROR, BH3D_LOGGER_WARNING
#define BH3D_LOG_FILE_LINE_FUNC			" -- FILE: <"<<__FILE__<<"> -- LINE: <"<<__LINE__<<">"<<" -- FUNC: <"<<__func__<<">"
#define BH3D_LOG_WIDTH					std::setw(15)
#define BH3D_LOG_MSG(msg)				BH3D_LOG_WIDTH<<"<LOG>: "<<msg<<std::endl
#define BH3D_LOG_WARNING(msg)			BH3D_LOG_WIDTH<<"<WARNING>: "<<msg<<BH3D_LOG_FILE_LINE_FUNC<<std::endl
#define BH3D_LOG_ERROR(msg)				BH3D_LOG_WIDTH<<"<ERROR>: "<<msg<<BH3D_LOG_FILE_LINE_FUNC<<std::endl



#ifdef BH3D_USE_SDL_LOGGER

#define BH3D_LOGGER(msg)			{std::ostringstream a; a <<BH3D_LOG_MSG(msg)	; SDL_Log(a.str().c_str());}
#define BH3D_LOGGER_ERROR(msg)		{std::ostringstream a; a <<BH3D_LOG_ERROR(msg)	; SDL_Log(a.str().c_str());}
#define BH3D_LOGGER_WARNING(msg)	{std::ostringstream a; a <<BH3D_LOG_WARNING(msg); SDL_Log(a.str().c_str());}

#elif defined(BH3D_USE_COUT_LOGGER)

#include<iostream>
#define BH3D_LOGGER(msg)			{std::cout<<BH3D_LOG_MSG(msg)		; }
#define BH3D_LOGGER_ERROR(msg)		{std::cout<<BH3D_LOG_ERROR(msg)		; }
#define BH3D_LOGGER_WARNING(msg)	{std::cout<<BH3D_LOG_WARNING(msg)	; }

#elif defined(BH3D_USE_FILE_LOGGER)

#define BH3D_LOGGER_INSTANCE				::bh3d::Logger::Instance().getFileLogger()
#define BH3D_LOGGER(msg)					{BH3D_LOGGER_INSTANCE<<BH3D_LOG_MSG(msg);}
#define BH3D_LOGGER_ERROR(msg)				{BH3D_LOGGER_INSTANCE<<BH3D_LOG_ERROR(msg);}
#define BH3D_LOGGER_WARNING(msg)			{BH3D_LOGGER_INSTANCE<<BH3D_LOG_WARNING(msg);}

#elif defined (BH3D_USE_COUNT_AND_FILE_LOGGER)

#include<iostream>

#define BH3D_LOGGER_INSTANCE				::bh3d::Logger::Instance().getFileLogger()
#define BH3D_LOGGER(msg)					{	BH3D_LOGGER_INSTANCE<<BH3D_LOG_MSG(msg);		std::cout<<BH3D_LOG_MSG(msg);	}
#define BH3D_LOGGER_ERROR(msg)				{	BH3D_LOGGER_INSTANCE<<BH3D_LOG_ERROR(msg);		std::cout<<BH3D_LOG_MSG(msg);	}
#define BH3D_LOGGER_WARNING(msg)			{	BH3D_LOGGER_INSTANCE<<BH3D_LOG_WARNING(msg);	std::cout<<BH3D_LOG_MSG(msg);	}

#endif



#include <filesystem>
#include <iomanip>
#include <fstream>
#include <sstream>

#include <BH3D_Bindgleton.hpp>

namespace bh3d
{
	/*!
	\author 	Robxley (A.CAILLY)
	\version	0.1
	\date		08/12/2014
	\~english
	\brief		Logger Class to log what you want in a speficique file. (See macro BH3D_LOGGER_FILE "BH3Dlogger.log").
	\remark		It's a singleton. See macros BH3D_LOGGER(msg), BH3D_LOGGERERROR(msg) and BH3D_LOGGERWARNING(msg).
	\~french
	\brief		Classer logger. Pour écrire ce que tu veux dans un fichier spécifique (voir macro BH3D_LOGGER_FILE "BH3Dlogger.log")
	\remark		C'est un singleton. Voir les macros BH3D_LOGGER(msg), BH3D_LOGGERERROR(msg) and BH3D_LOGGERWARNING(msg).
	*/
	class Logger : public Bindgleton<Logger>
	{
	public:
			std::ofstream      logFile;
			/**
			*\~english
			*\brief		Private Constructor - Creates the file define by the macro BH3D_LOGGER_FILE.
			*\~french
			*\brief		Private Constructor - Créée le fichier défini par la macro BH3D_LOGGER_FILE.
			*/
			Logger(const std::filesystem::path & path_log = "bh3d_logger.txt", bool bind = true);

			/**
			*\~english
			*\brief		Private Constructor - Closes the file.
			*\~french
			*\brief		Private Constructor - Ferme le fichier.
			*/
			~Logger();

			/**
			*\~english
			*\brief		Function to access to the logger.
			*\return	return a reference on logger.
			*\~french
			*\brief		Utiliser cette fonction pour avoir acc�s au logger.
			*\return	retourne une reference sur le logger.
			*/
			//static Logger &Bind();

			/**
			*\~english
			*\brief		gets the logger ofstream.
			*\return	return a reference on the used ofstream.
			*\~french
			*\brief		Pour obtenir le "ofstream" du logger.
			*\return	retourne une reference sur le ofstream utilisé par le logger.
			*/
			std::ofstream& getFileLogger();


	};

}

#endif // BH3D_VERBOSE
