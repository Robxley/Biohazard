#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <cassert>

template <typename T>
struct CDataGuard
{
	std::shared_ptr<T> m_proc;

	CDataGuard(std::shared_ptr<T>&& ptr) :
		m_proc(ptr) { }

	CDataGuard(std::shared_ptr<T>& ptr) :
		m_proc(ptr) { }

	auto operator->() {
		return lock_guard();
	}

	auto operator->() const {
		return lock_guard();
	}

	auto lock_guard() const {
		return CLocker{ m_protector, m_proc.get() };
	}

	auto get() {
		return m_proc.get();
	}

	struct CLocker
	{
		std::lock_guard<std::mutex> m_locker;
		T* m_ptr;

		template <typename Locker, typename Ptr>
		CLocker(Locker&& lc, Ptr&& ptr) :
			m_locker(std::forward<Locker>(lc)),
			m_ptr(std::forward<Ptr>(ptr))
		{
			assert(ptr != nullptr && "invalid pointer to protect");
		}

		T* operator->() {
			return m_ptr;
		}
		const T* operator->() const {
			return m_ptr;
		}
	};

protected:
	mutable std::mutex m_protector;
};