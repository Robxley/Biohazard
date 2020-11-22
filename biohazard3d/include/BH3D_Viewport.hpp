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
#ifndef _BH3D_VIEWPORT_H_
#define _BH3D_VIEWPORT_H_

#include <map>
#include <functional>
#include <algorithm>
#include <optional>

#include <glm/glm.hpp>

#include "BH3D_GL.hpp"

#include "BH3D_Common.hpp"

namespace bh3d
{	

	inline constexpr glm::vec4 DEFAULT_CLEAR_COLOR = { 48.0f / 255.0f, 51.0f / 255.0f, 46.0f / 255.0f,1.0f };

	/// <summary>
	/// Viewport class description.
	/// The viewport define the region of interest on the screen for drawing
	/// </summary>
	class Viewport
	{
	public:
		//Viewport size
		int m_position_x = 0;								//! Viewport screen position on X
		int m_position_y = 0;								//! Viewport screen position on Y
		int m_width = 0;									//! Viewport width
		int m_height = 0;									//! Viewport height
		glm::vec4 m_clearColor = DEFAULT_CLEAR_COLOR;		//! Screen clear color in the viewport

		/// <summary>
		/// Set viewport
		/// </summary>
		void SetViewport(int pos_x, int pos_y, int width, int height) 
		{
			m_position_x = pos_x;
			m_position_y = pos_y;
			m_width = width;
			m_height = height;
		}

		/// <summary>
		/// Set viewport
		/// </summary>
		template <typename T>
		void SetViewport(const glm::vec<4,T> & vec) {
			m_position_x	= (int)	vec[0];
			m_position_y	= (int)	vec[1];
			m_width			= (int)	vec[2];
			m_height		= (int)	vec[3];
		}

		/// <summary>
		/// Return true if the format is landscape (as width > height)
		/// </summary>
		/// <returns></returns>
		inline constexpr bool IsLandscape() const { return m_width > m_height; }

		/// <summary>
		/// Return true if the width and height are greater than 0
		/// </summary>
		/// <returns></returns>
		inline constexpr bool IsValid() const { return m_width > 0 && m_height > 0; }
		
		/// <summary>
		/// Bind the viewport and the clear color
		/// </summary>
		inline void Bind() const {
			GLViewport();
			GLClearColor();
		}

		/// <summary>
		/// Bind the viewport, the clear color and the Scissor with the viewport properties
		/// </summary>
		inline void BindScissor() const {
			GLScissor();			
			Bind();
		}

		/// <summary>
		/// Bind the viewport, the clear color and clear the buffer
		/// </summary>
		/// <param name="mask">buffer clear mod</param>
		inline void Bind(GLbitfield mask) const {
			GLViewport();
			ClearColor_Clear(mask);
		}

		/// <summary>
		/// Bind the viewport, the clear color and the Scissor with the viewport properties
		/// And clear the buffer
		/// </summary>
		/// <param name="mask">buffer clear mod</param>
		inline void BindScissor(GLbitfield mask) const {
			GLScissor();		
			Bind(mask);
		}

		/// <summary>
		/// Bind the wiewport
		/// </summary>
		inline void GLViewport() const {
			glViewport(m_position_x, m_position_y, m_width, m_height);
		}


		/// <summary>
		/// Set and Bind the Opengl clear color
		/// </summary>
		/// <param name="clearColor">Clear color value</param>
		inline void GLClearColor(const glm::vec4 & clearColor) {
			m_clearColor = clearColor;
			GLClearColor();
		}

		/// <summary>
		/// Bind the Opengl clear color
		/// </summary>
		inline void GLClearColor() const {
			glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
		}

		/// <summary>
		/// Bind the clear color, and clear the buffer
		/// </summary>
		/// <param name="mask"></param>
		inline void ClearColor_Clear(GLbitfield mask = GL_COLOR_BUFFER_BIT) const {
			GLClearColor();
			glClear(mask);
		}

		/// <summary>
		/// Call glClear function
		/// </summary>
		/// <param name="mask">Opengl Buffer bit</param>
		inline void GLClear(GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) const {
			glClear(mask);
		}

		/// <summary>
		/// Set the clear color
		/// </summary>
		/// <param name="clearColor">color value</param>
		inline void SetClearColor(const glm::vec4 & clearColor) {
			m_clearColor = clearColor;
		}

		/// <summary>
		/// Enable glScissor
		/// </summary>
		inline void GLScissor() const {
			static unique_call ucall = [] {
				if(!glIsEnabled(GL_SCISSOR_TEST))
					glEnable(GL_SCISSOR_TEST); 
			};
			ucall.do_nothing();
			assert(glIsEnabled(GL_SCISSOR_TEST));
			glScissor(m_position_x, m_position_y, m_width, m_height);
		}

		/// <summary>
		/// Check if a point is inside the Viewport
		/// </summary>
		/// <param name="point">Point to check</param>
		/// <returns>Checking result</returns>
		inline bool Intersect(const glm::ivec2 & point) const {
			auto outside = [](auto v, auto a, auto b) {return (v < a) || (v >= b); };
			return !(outside(point.x, m_position_x, m_position_x + m_width) || outside(point.y, m_position_y, m_position_y + m_height));
		}
		
	};

}
#endif //
