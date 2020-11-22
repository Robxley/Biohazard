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




/*!
\file Common.hpp
\author Robxley (A.CAILLY)
\version 0.1
\date 09/12/2014
\~english
\brief Common define
\details  Common objects and macros definition
\~french
\brief Quelques definitions
\details  Décription des macros communes à l'ensemble du projet Biohazard3d
*/

#pragma once
#ifndef _BH3D_COMMON_H_
#define _BH3D_COMMON_H_

#include <variant>
#include <memory>
#include <cassert>
#include <iostream>
#include <functional>
#include <sstream>
#include <string>
#include <numeric>

#include <glm/glm.hpp>

namespace bh3d
{
	/*! Everything is fine ( return function ) */
	constexpr int BH3D_OK = 1;

	/*! Something bad has happened ( return function ) */
	constexpr int  BH3D_ERROR = 0;

	/*! Close the engine */
	constexpr int  BH3D_EXIT = 0;


	template<typename TObject>
	class vobject
	{
		using TObjectPtr = TObject * ;
		using TObjectSharedPtr = std::shared_ptr<TObject>;

		std::variant<TObject, TObjectPtr, TObjectSharedPtr> m_object = TObject{};
	public:
		vobject() : m_object(TObject{}) {}
		vobject(TObject && obj) : m_object(TObject(obj)) {}
		vobject(TObject & obj) : m_object(TObjectPtr(&obj)) {}
		vobject(TObject * obj) : m_object(TObjectPtr(obj)) {}
		vobject(TObjectSharedPtr obj) : m_object(TObjectSharedPtr(obj)) {}

		inline TObject& value() {
			struct Visitor {
				inline TObject& operator()(TObjectPtr ptr) { assert(ptr != nullptr); return *ptr; }
				inline TObject& operator()(TObjectSharedPtr ptr) { assert(ptr != nullptr); return *ptr; }
				inline TObject& operator()(TObject & obj) { return obj; }
			};
			return std::visit(Visitor{}, m_object);
		}
		inline operator TObject&() { return this->value(); }

	};

	/// <summary>
	/// Usefull class to perform a unique call of a function by using a static object
	/// </summary>
	class unique_call
	{ public:
		template<typename... Args>
		unique_call(Args... args) {
			std::invoke(std::forward<Args>(args)...);
		}

		inline void do_nothing(){}; //Remove compileur warning
	};

	template<typename TGLMVec = glm::vec4, typename TDot = char>
	std::string to_string(TGLMVec & vec, int precision = 1, TDot dot = ';')
	{
		std::ostringstream out;
		out.precision(1);
		out << std::fixed << vec[0];
		for (int i = 1; i < vec.length(); i++)
			out << dot << std::fixed << vec[i];
		return out.str();
	}


	template<typename T = int>

	/// <summary>
	/// keep a value in rang [min, max] fall
	/// </summary>
	class interval
	{
		T m_value = 0;
		T m_min = 0;
		T m_max = std::numeric_limits<T>::max();	// not include
		T m_range = 0;
		bool m_loop = false;							//! Perform a loop when the max/min id value is rushed, else the value is clamp

		inline int keep_in_range(T v)
		{
			return  m_loop
				? ((v >= m_min && v < m_max)
					? v
					: ((((v - m_min) % m_range) + m_range) % m_range + m_min)
					)
				: std::clamp(v, m_min, m_max-1);
		}

	public:

		interval(T value, T min = 0, T max = std::numeric_limits<T>::max(), bool loop = false) :
			m_value(value),
			m_min(min),
			m_max(max),
			m_loop(loop)
		{
			assert(m_min < m_max);
			m_range = m_max - m_min;
		};

		void range(T min, T max) {
			m_min = min;
			m_max = max;
			m_range = m_max - m_min;
		}

		void max(T max) {
			m_max = max;
			m_range = m_max - m_min;
		}

		void loop(bool l) { m_loop = l; }

		void min(T min) {
			m_min = min;
			m_range = m_max - m_min;
		}

		auto& operator=(const T & value)
		{
			m_value = value;
			return *this;
		}

		T value() { m_value = keep_in_range(m_value); return m_value; }

		operator T& () { m_value = keep_in_range(m_value);  return m_value; }
		operator const T& () const { m_value = keep_in_range(m_value); return m_value; }
	};

	using interval_int = interval<int>;

}

#endif