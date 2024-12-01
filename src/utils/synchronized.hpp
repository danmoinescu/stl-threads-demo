#pragma once

/* Synchronized wrapper for any class, mostly copied from
   https://vorbrodt.blog/2021/08/17/how-to-synchronize-data-access/
*/

#include <mutex>
#include <utility>
#include <type_traits>


namespace detail
{
	template<typename U>
	class locker
	{
        private:
            U& m_value;
            std::recursive_mutex& m_lock;

        public:
            locker(U& v, std::recursive_mutex& l) noexcept
            : m_value(v), m_lock(l) { m_lock.lock(); }

            locker(const locker&) = delete;
            locker& operator = (const locker&) = delete;

            ~locker() noexcept { m_lock.unlock(); }

            U* operator -> () noexcept { return &m_value; }
	};
}


template<typename T>
class Synchronized
{
    private:
        T m_value;
        std::recursive_mutex m_lock;

    public:
        Synchronized() = default;
        Synchronized(const T& v) : m_value(v) {}
        Synchronized(T&& v) : m_value(std::move(v)) {}

        // constructors that build m_value in-place from arguments
        template<
            typename... A,
            std::enable_if_t<std::is_constructible_v<T, A...>>* = nullptr>
        Synchronized(A&&... a) : m_value(std::forward<A>(a)...) {}
        template<
            typename V,
            std::enable_if_t<
                std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
        Synchronized(std::initializer_list<V> l) : m_value(l) {}

        // disallow copies
        Synchronized(const Synchronized&) = delete;
        Synchronized& operator = (const Synchronized&) = delete;

        detail::locker<T> operator -> () noexcept
        {
            return detail::locker(m_value, m_lock);
        }

        detail::locker<T> get_instance() noexcept
        {
            return detail::locker(m_value, m_lock);
        }

        // export the mutex in case other entities need to sync on it
        std::recursive_mutex& mutex() { return m_lock; }
};
