#pragma once
#ifndef __EXCEPTION_TRACKER_H__

#include "logger.h"

#include <exception>

#include <opencv2\opencv.hpp>


#define ERROR_MSG_LOCATION(msg)									concat_to_string("-File: ",__FILE__," -Line: ", __LINE__, " -Func: ", __func__, " -Error: ", msg)

#define EXCEPTION_MSG_LOGGER(msg)								logging::Error(ERROR_MSG_LOCATION(msg))

#define EXCEPTION_MSG_LOGGER_WITH_THROW(msg)					EXCEPTION_MSG_LOGGER(msg); throw std::runtime_error(msg);


#define BEGIN_EXCEPTION_TRACKER									try{


#define OPENCV_EXCEPTION_CATCH_WITH_THROW()						\
	catch(const cv::Exception& e)								\
	{															\
		EXCEPTION_MSG_LOGGER_WITH_THROW(e.what());				\
	}															

#define STD_EXCEPTION_CATCH_WITH_THROW()						\
	catch(const std::exception& e)								\
	{															\
		EXCEPTION_MSG_LOGGER_WITH_THROW(e.what());				\
	}	

#define EXCEPTION_CATCH_WITH_THROW()							\
	catch(...) 													\
	{															\
		EXCEPTION_MSG_LOGGER_WITH_THROW("Unknown exception");	\
	}	



#define OPENCV_EXCEPTION_CATCH_WITH_CODE(code)					\
	catch(const cv::Exception& e)								\
	{															\
		{code}													\
		EXCEPTION_MSG_LOGGER_WITH_THROW(e.what());				\
	}	

#define STD_EXCEPTION_CATCH_WITH_CODE(code)						\
	catch(const std::exception& e)								\
	{															\
		{code}													\
		EXCEPTION_MSG_LOGGER_WITH_THROW(e.what());				\
	}	

#define EXCEPTION_CATCH_WITH_CODE(code)							\
	catch(...) 													\
	{															\
		{code}													\
		EXCEPTION_MSG_LOGGER_WITH_THROW("Unknown exception");	\
	}	


#define END_EXCEPTION_TRACKER_WITH_THROW()			}			\
	OPENCV_EXCEPTION_CATCH_WITH_THROW()							\
	STD_EXCEPTION_CATCH_WITH_THROW()							\
	EXCEPTION_CATCH_WITH_THROW()			

#define END_EXCEPTION_TRACKER_WITH_CODE(code)		}			\
	OPENCV_EXCEPTION_CATCH_WITH_CODE(code)						\
	STD_EXCEPTION_CATCH_WITH_CODE(code)							\
	EXCEPTION_CATCH_WITH_CODE(code)	

#define THROW_EXCEPTION_IF_FAIL(expr)	\
	if( !((expr)) )						\
	{									\
		EXCEPTION_MSG_LOGGER(expr);		\
		throw;							\
	}

#define LOG_ASSERTION_IF_FAIL(expr) if (expr){} else { EXCEPTION_MSG_LOGGER(#expr)); }

#endif