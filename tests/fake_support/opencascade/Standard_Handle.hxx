// Copyright (c) 2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#pragma once

#include <type_traits>

class Standard_Transient;

namespace opencascade {

//! Trait yielding true if class T1 is base of T2 but not the same
template <class T1, class T2, class Dummy = void>
struct is_base_but_not_same : std::is_base_of <T1, T2> {};

//! Explicit specialization of is_base_of trait to workaround the
//! requirement of type to be complete when T1 and T2 are the same.
template <class T1, class T2>
struct is_base_but_not_same <T1, T2, typename std::enable_if <std::is_same <T1, T2>::value>::type> : std::false_type {};

template <class T>
class handle
{
public:
    typedef T element_type;

public:
    handle() {}
    handle(const T*) {}
    handle(const handle&) {}
    handle(handle&&) {}
    ~handle() {}

    void Nullify() {}
    bool IsNull() const { return true; }
    void reset(T*) {}

    handle& operator=(const handle&) { return *this; }
    handle& operator=(const T*) { return *this; }
    handle& operator= (handle&&) { return *this; }

    const T* get() const { return 0; }
    T* get() { return 0; }
    T* operator->() const { return 0; }
    T& operator* () { return *get(); }
    const T& operator*() const { return *get(); }

    template<class T2> bool operator==(const handle<T2>&) const { return false; }
    template<class T2> bool operator==(const T2*) const  { return false; }
    template<class T2> friend bool operator==(const T2*, const handle&) { return false; }
    template<class T2> bool operator!=(const handle<T2>&) const { return false; }
    template<class T2> bool operator!=(const T2*) const { return false; }
    template<class T2> friend bool operator!=(const T2*, const handle&) { return false; }
    template<class T2> bool operator<(const handle<T2>&) const { return false; }

    template<class T2>
    static typename std::enable_if<is_base_but_not_same<T2, T>::value, handle>::type
    DownCast (const handle<T2>& theObject)
    { return handle (dynamic_cast<T*>(const_cast<T2*>(theObject.get()))); }

    template <class T2>
    static typename std::enable_if<is_base_but_not_same<T2, T>::value, handle>::type
    DownCast (const T2* thePtr)
    { return handle (dynamic_cast<T*>(const_cast<T2*>(thePtr))); }

    //! For compatibility, define down casting operator from non-base type, as deprecated
    template <class T2>
    static handle DownCast (const handle<T2>& theObject, typename std::enable_if<!is_base_but_not_same<T2, T>::value, void*>::type = 0)
    { return handle (dynamic_cast<T*>(const_cast<T2*>(theObject.get()))); }

    //! For compatibility, define down casting operator from non-base type, as deprecated
    template <class T2>
    static handle DownCast (const T2* thePtr, typename std::enable_if<!is_base_but_not_same<T2, T>::value, void*>::type = 0)
    { return handle (dynamic_cast<T*>(const_cast<T2*>(thePtr))); }

#if (defined(__clang__)) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1300) || \
    (defined(_MSC_VER) && _MSC_VER >= 1800) || \
    (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))

    //! Conversion to bool for use in conditional expressions
    explicit operator bool () const { return entity != nullptr; }

#else /* fallback version for compilers not supporting explicit conversion operators (VC10, VC11, GCC below 4.5) */

    //! Conversion to bool-compatible type for use in conditional expressions
    operator Standard_Transient* handle::* () const
    {
        return entity ? &handle::entity : 0;
    }

#endif

    // Support of conversions to handle of base type:
    // - copy and move constructors and assignment operators if OCCT_HANDLE_NOCAST is defined
    // - operators of upcast to const reference to base type otherwise
#if (defined(__clang__)) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1206) || \
    (defined(_MSC_VER) && _MSC_VER >= 1800) || \
    (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)))

#ifdef OCCT_HANDLE_NOCAST

    //! Generalized copy constructor.
    //! Constructs handle holding entity of base type (T) from the one which holds entity of derived type (T2).
    template <class T2, typename = typename std::enable_if <is_base_but_not_same <T, T2>::value>::type>
    handle (const handle<T2>& theHandle) :
        entity(theHandle.entity)
    {
        BeginScope();
    }

    //! Generalized move constructor
    template <class T2, typename = typename std::enable_if <is_base_but_not_same <T, T2>::value>::type>
    handle (handle<T2>&& theHandle)
        : entity(theHandle.entity)
    {
        theHandle.entity = 0;
    }

    //! Generalized assignment operator
    template <class T2, typename = typename std::enable_if <is_base_but_not_same <T, T2>::value>::type>
    handle operator = (const handle<T2>& theHandle)
    {
        Assign (theHandle.entity);
        return *this;
    }

    //! Generalized move operator
    template <class T2, typename = typename std::enable_if <is_base_but_not_same <T, T2>::value>::type>
    handle& operator= (handle<T2>&& theHandle)
    {
        std::swap (this->entity, theHandle.entity);
        return *this;
    }

#else

    //! Upcast to const reference to base type.
    template <class T2, typename = typename std::enable_if<is_base_but_not_same<T2, T>::value>::type>
    operator const handle<T2>& () const
    {
        return reinterpret_cast<const handle<T2>&>(*this);
    }

    //! Upcast to non-const reference to base type.
    //! NB: this cast can be dangerous, but required for legacy code; see #26377
    template <class T2, typename = typename std::enable_if<is_base_but_not_same<T2, T>::value>::type>
    operator handle<T2>& ()
    {
        return reinterpret_cast<handle<T2>&>(*this);
    }

#endif /* OCCT_HANDLE_NOCAST */

#else /* fallback version for compilers not supporting default arguments of function templates (VC10, VC11, GCC below 4.3) */

#ifdef OCCT_HANDLE_NOCAST

    //! Generalized copy constructor.
    //! Constructs handle holding entity of base type (T) from the one which holds entity of derived type (T2).
    template <class T2>
    handle (const handle<T2>& theHandle, typename std::enable_if <is_base_but_not_same <T, T2>::value>::type* = nullptr) :
        entity(theHandle.entity)
    {
        BeginScope();
    }

    //! Generalized move constructor
    template <class T2>
    handle (handle<T2>&& theHandle, typename std::enable_if <is_base_but_not_same <T, T2>::value>::type* = nullptr)
        : entity(theHandle.entity)
    {
        theHandle.entity = 0;
    }

    //! Generalized assignment operator.
    template <class T2>
    handle operator = (const handle<T2>& theHandle)
    {
        std::enable_if <is_base_but_not_same <T, T2>::value, void*>::type aTypeCheckHelperVar;
        (void)aTypeCheckHelperVar;
        Assign (theHandle.entity);
        return *this;
    }

    //! Generalized move operator
    template <class T2>
    handle& operator= (handle<T2>&& theHandle)
    {
        std::enable_if <is_base_but_not_same <T, T2>::value, void*>::type aTypeCheckHelperVar;
        (void)aTypeCheckHelperVar;
        std::swap (this->entity, theHandle.entity);
        return *this;
    }

#else

    //! Upcast to const reference to base type.
    //! NB: this implementation will cause ambiguity errors on calls to overloaded
    //! functions accepting handles to different types, since compatibility is
    //! checked in the cast code rather than ensured by SFINAE (possible with C++11)
    template <class T2>
    operator const handle<T2>& () const
    {
        // error "type is not a member of enable_if" will be generated if T2 is not sub-type of T
        // (handle is being cast to const& to handle of non-base type)
        return reinterpret_cast<typename std::enable_if<is_base_but_not_same<T2, T>::value, const handle<T2>&>::type>(*this);
    }

    //! Upcast to non-const reference to base type.
    //! NB: this cast can be dangerous, but required for legacy code; see #26377
    template <class T2>
    Standard_DEPRECATED("Passing non-const reference to handle of base type in function is unsafe; use variable of exact type")
    operator handle<T2>& ()
    {
        // error "type is not a member of enable_if" will be generated if T2 is not sub-type of T
        // (handle is being cast to const& to handle of non-base type)
        return reinterpret_cast<typename std::enable_if<is_base_but_not_same<T2, T>::value, handle<T2>&>::type>(*this);
    }

#endif /* OCCT_HANDLE_NOCAST */

#endif /* compiler switch */

private:
    void Assign (Standard_Transient*) {}
    void BeginScope() {}
    void EndScope() {}

    template <class T2> friend class handle;

private:
    Standard_Transient* entity;
};

} // namespace opencascade
