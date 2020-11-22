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

#ifndef _BH3D_TINY_ENGINE_H_
#define _BH3D_TINY_ENGINE_H_

/// <summary>
/// Basic engine based on biohasard 3D
/// </summary>


#include <glm/glm.hpp>

#include "BH3D_Logger.hpp"
#include "BH3D_Camera.hpp"

namespace bh3d
{
	class TinyEngine
	{
	
	public:

		//BH3D objects
#ifdef BH3D_VERBOSE
		bh3d::Logger m_logger = { "./Biohazard3D.log" };			//Create a logger file
#endif
		CameraEngine m_camera;										//Default camera used to draw the Opengl scene in the screen

	public:
	
		// Main functions
		//-------------------------------------------------------------
		virtual ~TinyEngine() { 
			Clear(); 
		};

		/// <summary>
		/// Resize function to call when the window (or drawing buffer) size change to ajust some variable 
		/// (Update the LookAt matrixe, Projection matrix, and the viewport size...,)
		/// </summary>
		/// <param name="width">Window width</param>
		/// <param name="height">Window height</param>
		virtual void Resize(unsigned int width, unsigned int height) {
			BH3D_LOGGER(__func__ << "( width = " << width << " - height = " << height << ")");
			m_camera.Resize(width, height);
		}

		/// <summary>
		/// Init what you want
		/// </summary>
		virtual void Init() { InitOpenGL(); }

		/// <summary>
		/// Virtual function to display something to the screen
		/// </summary>
		virtual void Display() {};

		/// <summary>
		/// Clear what you want
		/// </summary>
		virtual void Clear(){}

	protected:
		/// <summary>
		/// By default some opengl init :
		///		- the back ground color ( see m_clearColor )
		///		- default vertex color in shader ( see m_defaultShaderColor )
		///		- Enable the depth test with depht function as glDepthFunc(GL_LESS);
		///		- Draw only the front face ( glEnable(GL_CULL_FACE) )
		///		- Enable the blending with blend function as glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		/// </summary>
		virtual void InitOpenGL();

	};
}
#endif