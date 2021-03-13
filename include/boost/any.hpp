// See http://www.boost.org/libs/any for Documentation.

#ifndef BOOST_ANY_INCLUDED
#define BOOST_ANY_INCLUDED

#if defined(_MSC_VER)
# pragma once
#endif

// what:  variant type boost::any
// who:   contributed by Kevlin Henney,
//        with features contributed and bugs found by
//        Antony Polukhin, Ed Brey, Mark Rodgers, 
//        Peter Dimov, and James Curran
//        dispatch table rewrite by Bjorn Reese
// when:  July 2001, April 2013 - 2020

#include <boost/config.hpp>
#include <boost/type_index.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/decay.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/throw_exception.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/core/addressof.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/conditional.hpp>

#include <boost/core/exchange.hpp>
#include <boost/core/swap.hpp>
#include <boost/type_traits/is_copy_constructible.hpp>
#include <boost/move/utility_core.hpp>

#ifndef BOOST_NO_CXX11_HDR_INITIALIZER_LIST
#include <initializer_list>
#endif

namespace boost
{
namespace detail
{

template <typename T>
struct any_decay
{
    typedef typename boost::remove_cv<typename boost::remove_reference<T>::type>::type type;
};

} // namespace detail

class any
{
public:
    BOOST_CONSTEXPR any() BOOST_NOEXCEPT {};

    ~any()
    {
        if (has_value())
        {
            interface->destroy(storage);
        }
    }

    any(const any& other)
        : interface(other.interface)
    {
        if (other.interface)
        {
            other.interface->copy(storage, other.storage);
        }
    }

    any& operator=(const any& other)
    {
        any(other).swap(*this);
        return *this;
    }

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
    any(any&& other)
        : storage(boost::move(other.storage)),
          interface(boost::exchange(other.interface, (struct interface *)0))
    {
    }

    any& operator=(any&& other)
    {
        any(boost::move(other)).swap(*this);
        return *this;
    }
#endif

    // Perfect forwarding of ValueType
    template<typename ValueType,
             typename = typename boost::enable_if_c<!boost::is_same<any, typename boost::decay<ValueType>::type>::value &&
                                                    boost::is_copy_constructible<typename boost::decay<ValueType>::type>::value>::type>
    any(ValueType&& value)
        : interface(boost::addressof(dispatcher<typename boost::decay<ValueType>::type>::instance()))
    {
        dispatcher<typename boost::decay<ValueType>::type>::create(storage, boost::forward<ValueType>(value));
    }

    template <typename ValueType,
             typename = typename boost::enable_if_c<!boost::is_same<any, typename boost::decay<ValueType>::type>::value &&
                                                    boost::is_copy_constructible<typename boost::decay<ValueType>::type>::value>::type>
    any& operator=(ValueType&& value)
    {
        any(boost::forward<ValueType>(value)).swap(*this);
        return *this;
    }

#ifdef BOOST_NO_CXX11_VARIADIC_TEMPLATES
    // Only one argument supported for pre-C++11
    template <typename ValueType,
              typename Arg1,
              typename DecayType = typename boost::decay<ValueType>::type,
              typename = typename boost::enable_if_c<boost::is_constructible<DecayType, Arg1>::value &&
                                                     boost::is_copy_constructible<DecayType>::value>::type>
    DecayType& emplace(Arg1&& arg1)
    {
        interface = boost::addressof(dispatcher<DecayType>::instance());
        dispatcher<DecayType>::create(storage, boost::forward<Arg1>(arg1));
        return *dispatcher<DecayType>::cast(storage);
    }
#else
    template <typename ValueType,
              typename... Args,
              typename DecayType = typename boost::decay<ValueType>::type,
              typename = typename boost::enable_if_c<boost::is_constructible<DecayType, Args...>::value &&
                                                     boost::is_copy_constructible<DecayType>::value>::type>
    DecayType& emplace(Args&&... args)
    {
        interface = boost::addressof(dispatcher<DecayType>::instance());
        dispatcher<DecayType>::create(storage, boost::forward<Args>(args)...);
        return *dispatcher<DecayType>::cast(storage);
    }

#ifndef BOOST_NO_CXX11_HDR_INITIALIZER_LIST
    template <typename ValueType,
              typename U,
              typename... Args,
              typename DecayType = typename boost::decay<ValueType>::type,
              typename = typename boost::enable_if_c<boost::is_constructible<DecayType, std::initializer_list<U>&, Args...>::value &&
                                                     boost::is_copy_constructible<DecayType>::value>::type>
    DecayType& emplace(std::initializer_list<U> il, Args&&... args)
    {
        interface = boost::addressof(dispatcher<DecayType>::instance());
        dispatcher<DecayType>::create(storage, boost::move(il), boost::forward<Args>(args)...);
        return *dispatcher<DecayType>::cast(storage);
    }

#endif
#endif

    bool has_value() const BOOST_NOEXCEPT
    {
        return static_cast<bool>(interface);
    }

    bool empty() const BOOST_NOEXCEPT
    {
        return !has_value();
    }

    // Checks if object has content of type ValueType.
    //
    // holds<void>() checks if object has no content.

    template <typename ValueType>
    bool holds() const BOOST_NOEXCEPT
    {
        typedef BOOST_DEDUCED_TYPENAME boost::decay<ValueType>::type DecayType;
        return dispatcher<DecayType>::holds(*this);
    }

    any& swap(any& other) BOOST_NOEXCEPT
    {
        using boost::swap;
        swap(interface, other.interface);
        swap(storage, other.storage);
        return *this;
    }

    void reset() BOOST_NOEXCEPT
    {
        any().swap(*this);
    }

    void clear() BOOST_NOEXCEPT
    {
        reset();
    }

    const boost::typeindex::type_info& type() const BOOST_NOEXCEPT
    {
        if (has_value())
        {
            return interface->type();
        }
        return boost::typeindex::type_id<void>().type_info();
    }

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS

private: // representation

    template<typename ValueType>
    friend ValueType * any_cast(any *) BOOST_NOEXCEPT;

    template<typename ValueType>
    friend const ValueType * any_cast(const any *) BOOST_NOEXCEPT;

    template<typename ValueType>
    friend ValueType * unsafe_any_cast(any *) BOOST_NOEXCEPT;

#else

public: // representation (public so any_cast can be non-friend)

#endif

    union storage_type
    {
        BOOST_CONSTEXPR BOOST_DEFAULTED_FUNCTION(storage_type() BOOST_NOEXCEPT, {});

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
        BOOST_DEFAULTED_FUNCTION(storage_type(storage_type&&) BOOST_NOEXCEPT, {});
        storage_type& BOOST_DEFAULTED_FUNCTION(operator=(storage_type&&) BOOST_NOEXCEPT, {});
#endif

        // First alternative
        void *pointer = 0;

        // Second alternative
        BOOST_ALIGNMENT(void *) unsigned char buffer[sizeof(void *)];
    } storage;

    // Type-erased interface points to dispatch table for dispatcher<T>
    struct BOOST_SYMBOL_VISIBLE interface
    {
        void (*destroy)(storage_type&);
        void (*copy)(storage_type&, const storage_type&);
        const boost::typeindex::type_info& (*type)();
    } const * interface = 0;

    // Allocated types
    template <typename ValueType, typename = void>
    struct dispatcher
    {
        static ValueType * cast(storage_type& self)
        {
            return static_cast<ValueType *>(self.pointer);
        }

        static const ValueType * cast(const storage_type& self)
        {
            return static_cast<const ValueType *>(self.pointer);
        }

        static bool holds(const any& self) BOOST_NOEXCEPT
        {
            return self.interface == boost::addressof(instance());
        }

#ifdef BOOST_NO_CXX11_VARIADIC_TEMPLATES
        template <typename Arg1>
        static void create(storage_type& self, BOOST_FWD_REF(Arg1) arg1)
        {
            self.pointer = new ValueType(boost::forward<Arg1>(arg1));
        }
#else
        template <typename... Args>
        static void create(storage_type& self, Args&&... args)
        {
            self.pointer = new ValueType{boost::forward<Args>(args)...};
        }
#endif

        static void destroy(storage_type& self)
        {
            delete cast(self);
        }

        static void copy(storage_type& self, const storage_type& other)
        {
            create(self, *cast(other));
        }

        static const boost::typeindex::type_info& type()
        {
            return boost::typeindex::type_id<ValueType>().type_info();
        }

        static const struct interface& instance()
        {
            BOOST_STATIC_CONSTEXPR struct interface table = { &dispatcher::destroy, &dispatcher::copy, &dispatcher::type };
            return table;
        }
    };

    template <typename Unused>
    struct dispatcher<void, Unused>
    {
        static bool holds(const any& self) BOOST_NOEXCEPT
        {
            return !self.has_value();
        }
    };
};

class BOOST_SYMBOL_VISIBLE bad_any_cast :
#ifndef BOOST_NO_RTTI
        public std::bad_cast
#else
        public std::exception
#endif
{
public:
    const char * what() const BOOST_NOEXCEPT_OR_NOTHROW BOOST_OVERRIDE
    {
        return "boost::bad_any_cast: failed conversion using boost::any_cast";
    }
};

template <typename ValueType>
ValueType * any_cast(any * operand) BOOST_NOEXCEPT
{
    typedef BOOST_DEDUCED_TYPENAME boost::detail::any_decay<ValueType>::type DecayType;
    if (operand->holds<DecayType>())
    {
        return any::dispatcher<DecayType>::cast(operand->storage);
    }
    return 0;
}

template <typename ValueType>
const ValueType * any_cast(const any * operand) BOOST_NOEXCEPT
{
    typedef BOOST_DEDUCED_TYPENAME boost::detail::any_decay<ValueType>::type DecayType;
    if (operand->holds<DecayType>())
    {
        return any::dispatcher<DecayType>::cast(operand->storage);
    }
    return 0;
}

template <typename ValueType,
          typename DecayType = typename boost::detail::any_decay<ValueType>::type,
          typename = typename boost::enable_if_c<boost::is_constructible<ValueType, const DecayType&>::value>::type>
ValueType any_cast(const any & operand)
{
    const DecayType *p = any_cast<DecayType>(boost::addressof(operand));
    if (p)
        return static_cast<ValueType>(*p);

    boost::throw_exception(bad_any_cast());
}

template <typename ValueType,
          typename DecayType = typename boost::detail::any_decay<ValueType>::type,
          typename = typename boost::enable_if_c<boost::is_constructible<ValueType, DecayType&>::value>::type>
ValueType any_cast(any & operand)
{
    DecayType *p = any_cast<DecayType>(boost::addressof(operand));
    if (p)
        return static_cast<ValueType>(*p);

    boost::throw_exception(bad_any_cast());
}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
template <typename ValueType,
          typename DecayType = typename boost::detail::any_decay<ValueType>::type,
          typename = typename boost::enable_if_c<boost::is_constructible<ValueType, DecayType>::value>::type>
ValueType any_cast(any&& operand)
{
    DecayType *p = any_cast<DecayType>(boost::addressof(operand));
    if (p)
        return static_cast<ValueType>(boost::move(*p));

    boost::throw_exception(bad_any_cast());
}
#endif

// Note: The "unsafe" versions of any_cast are not part of the
// public interface and may be removed at any time. They are
// required where we know what type is stored in the any and can't
// use typeid() comparison, e.g., when our types may travel across
// different shared libraries.
template<typename ValueType>
ValueType * unsafe_any_cast(any * operand) BOOST_NOEXCEPT
{
    typedef BOOST_DEDUCED_TYPENAME boost::decay<ValueType>::type DecayType;
    return any::dispatcher<DecayType>::cast(operand->storage);
}

template<typename ValueType>
const ValueType * unsafe_any_cast(const any * operand) BOOST_NOEXCEPT
{
    return unsafe_any_cast<ValueType>(const_cast<any *>(operand));
}

} // namespace boost

// Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
// Copyright Antony Polukhin, 2013-2021.
// Copyright Bjorn Reese, 2021.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#endif
