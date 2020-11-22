
#pragma once
#ifndef _BH3D_BINDGLETON_OPENCV_H_
#define _BH3D_BINDGLETON_OPENCV_H_

#include <cassert>

namespace bh3d
{
	template <typename T>
	class Bindgleton
	{
		inline thread_local static T * m_instance = nullptr;
	public:

		Bindgleton() {};
		Bindgleton(T * ptr) {
			Bind(*ptr);
		};

		virtual ~Bindgleton(){};

		static void Bind(T & bind) {
			m_instance = &bind;
		}
		static void UnBind() {
			m_instance = nullptr;
		}
		static auto & Instance() {
			assert(m_instance != nullptr && "Bind a instance before using it");
			return *m_instance;
		}

		Bindgleton(const Bindgleton&) = delete;
		Bindgleton& operator=(const Bindgleton&) = delete;

		void Bind() {
			Bind(*static_cast<T*>(this));
		}
	};
}
#endif

