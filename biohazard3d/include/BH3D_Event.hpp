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
#ifndef _BH3D_EVENT_H_
#define _BH3D_EVENT_H_

#include <map>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace bh3d
{	
	//Class to manage the mouse input
	class Mouse
	{
	public:
		enum class Event
		{
			NONE	= 0b00000000,
			DOWN	= 0b00000001,
			UP		= 0b00000010,
			MOVE	= 0b00000100,
			WHEEL	= 0b00001000,
		}; 

		//Status of a button 
		enum class Button
		{
			NONE		= 0b00000000,
			LEFT		= 0b00000001,
			RIGHT		= 0b00000010,
			MIDDLE		= 0b00000100,
			X1			= 0b00001000,
			X2			= 0b00010000
		};

		struct Status {
			int m_event		= 0;			//! The kind of the last event
			int m_button	= 0;			//! Down status of a button in function of enum class Button

			int m_posX		= 0;			//! Mouse position in X
			int m_posY		= 0;			//! Mouse position in Y
			int m_data		= 0;			//! Some extra data

			inline bool operator==(Event e)		const { return m_event == (int)e; }
			inline bool operator==(Button b)	const { return m_button == (int)b; }
			inline int operator&(Button b)		const { return m_button & (int)b; }
			inline int operator|(Button b)		const { return m_button | (int)b; }
			inline operator Event()				const { return (Event)m_event; }
			inline operator Button()			const { return (Button)m_button; }


		};

		Status m_status;					//! Current mouse status
		Status m_lastStatus;				//! The last mouse status

	public:
		inline void StatusSwap() { 
			if (m_status.m_event) { m_lastStatus = m_status; } 
			m_status.m_event &= ~((int)Event::WHEEL);
		}
		inline int GetEvent()				const { return m_status.m_event; }
		inline int GetButton()				const { return m_status.m_button; }
		inline int GetPosX()				const { return m_status.m_posX; }
		inline int GetPosY()				const { return m_status.m_posY; }
		inline int GetRelativePosX()		const { return m_status.m_posX - m_lastStatus.m_posX; }
		inline int GetRelativePosY()		const { return m_status.m_posY - m_lastStatus.m_posY; }
		inline int GetData()				const { return m_status.m_data; }

		//Compare operator
		inline bool operator==(Event e)		const { return GetEvent() == (int)e; }
		inline bool operator==(Button b)	const { return GetButton() == (int)b; }
		inline int operator&(Button b)		const { return (int)GetButton() & (int)b; }
		inline int operator|(Button b)		const { return (int)GetButton()| (int)b; }
		inline operator Event()				const { return (Event)GetEvent(); }
		inline operator Button()			const { return (Button)GetButton(); }

		//Mouse event update operator
		inline Mouse& operator<<(Event e)	{ m_status.m_event = (int)e; return *this;  }
		inline Mouse& operator>>(Event e)	{ m_status.m_event &= ~(int)e; return *this; }
		inline Mouse& operator=(Event e)	{ m_status.m_event = (int)e; return *this;  }

		//Mouse button update operator
		inline Mouse& operator<<(Button b)	{ m_status.m_button |= (int)b; return *this; }
		inline Mouse& operator>>(Button b)	{ m_status.m_button &= ~(int)b; return *this; }
		inline Mouse& operator=(Button b)	{ m_status.m_button = (int)b; return *this;  }
		inline Mouse& operator<<(int b)		{ operator<<((Button)b); return *this;}
		inline Mouse& operator>>(int b)		{ operator>>((Button)b); return *this; }
		inline Mouse& operator=(int data)	{ m_status.m_data = data; return *this; }

		inline Mouse& operator<<(const glm::u32vec2 & position) { 
			m_status.m_posX = position.x;
			m_status.m_posY = position.y;
			return *this;
		}
	};

}
#endif //
