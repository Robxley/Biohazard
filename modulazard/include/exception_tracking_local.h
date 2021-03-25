#pragma once

#include "logger.h"

#include <exception>

#include <opencv2\opencv.hpp>

#define THIS_ERROR_MSG_LOCATION(msg)									::mzd::concat_to_string("-File: ", __FILE__," -Line: ", __LINE__, " -Func: ", __func__, " -Error: ", msg)

#define THIS_EXCEPTION_MSG_LOGGER(msg)								this->LogError(THIS_ERROR_MSG_LOCATION(msg))

#define THIS_EXCEPTION_MSG_LOGGER_WITH_THROW(msg)					THIS_EXCEPTION_MSG_LOGGER(msg); throw std::runtime_error(msg);


#define THIS_BEGIN_EXCEPTION_TRACKER									try{


#define THIS_OPENCV_EXCEPTION_CATCH_WITH_THROW()						\
	catch(const cv::Exception& e)								\
	{															\
		THIS_EXCEPTION_MSG_LOGGER_WITH_THROW(e.what());				\
	}															

#define THIS_STD_EXCEPTION_CATCH_WITH_THROW()						\
	catch(const std::exception& e)								\
	{															\
		THIS_EXCEPTION_MSG_LOGGER_WITH_THROW(e.what());				\
	}	

#define THIS_EXCEPTION_CATCH_WITH_THROW()							\
	catch(...) 													\
	{															\
		THIS_EXCEPTION_MSG_LOGGER_WITH_THROW("Unknown exception");	\
	}	



#define THIS_OPENCV_EXCEPTION_CATCH_WITH_CODE(code)					\
	catch(const cv::Exception& e)								\
	{															\
		{code}													\
		THIS_EXCEPTION_MSG_LOGGER_WITH_THROW(e.what());				\
	}	

#define THIS_STD_EXCEPTION_CATCH_WITH_CODE(code)						\
	catch(const std::exception& e)								\
	{															\
		{code}													\
		THIS_EXCEPTION_MSG_LOGGER_WITH_THROW(e.what());				\
	}	

#define THIS_EXCEPTION_CATCH_WITH_CODE(code)							\
	catch(...) 													\
	{															\
		{code}													\
		THIS_EXCEPTION_MSG_LOGGER_WITH_THROW("Unknown exception");	\
	}	


#define THIS_END_EXCEPTION_TRACKER_WITH_THROW()			}			\
	THIS_OPENCV_EXCEPTION_CATCH_WITH_THROW()							\
	THIS_STD_EXCEPTION_CATCH_WITH_THROW()							\
	THIS_EXCEPTION_CATCH_WITH_THROW()			

#define THIS_END_EXCEPTION_TRACKER_WITH_CODE(code)		}			\
	THIS_OPENCV_EXCEPTION_CATCH_WITH_CODE(code)						\
	THIS_STD_EXCEPTION_CATCH_WITH_CODE(code)							\
	THIS_EXCEPTION_CATCH_WITH_CODE(code)	

#define THIS_THROW_EXCEPTION_IF_FAIL(expr)	\
	if( !((expr)) )						\
	{									\
		THIS_EXCEPTION_MSG_LOGGER(expr);		\
		throw;							\
	}

#define THIS_LOG_ASSERTION_IF_FAIL(expr) if (expr){} else { THIS_EXCEPTION_MSG_LOGGER(#expr)); }

