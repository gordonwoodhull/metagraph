/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga  2007
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_INTRUSIVE_PARENT_FROM_MEMBER_HPP
#define BOOST_INTRUSIVE_PARENT_FROM_MEMBER_HPP

#include <boost/intrusive/detail/config_begin.hpp>
#include <cstddef>

#if defined(BOOST_MSVC) || (defined (BOOST_WINDOWS) && defined(BOOST_INTEL))
#define BOOST_INTRUSIVE_MSVC_COMPLIANT_PTR_TO_MEMBER
#include <boost/cstdint.hpp>
#endif

namespace boost {
namespace detail {

template<class Parent, class Member>
inline std::ptrdiff_t offset_from_pointer_to_member(const Member Parent::* ptr_to_member)
{
   //The implementation of a pointer to member is compiler dependent.
   // gw: it seems like this could be a metafunction except that the implementations
   // depend on either local variables or taking the address of ptr_to_member
   // presumably it optimizes away anyway
   #if defined(BOOST_INTRUSIVE_MSVC_COMPLIANT_PTR_TO_MEMBER)
   //msvc compliant compilers use their the first 32 bits as offset (even in 64 bit mode)
   return *(const boost::int32_t*)(void*)&ptr_to_member;
   //This works with gcc, msvc, ac++, ibmcpp
   #elif defined(__GNUC__)   || defined(__HP_aCC) || defined(BOOST_INTEL) || \
         defined(__IBMCPP__) || defined(__DECCXX)
   const Parent * const parent = 0;
   const char *const member = reinterpret_cast<const char*>(&(parent->*ptr_to_member));
   return std::ptrdiff_t(member - reinterpret_cast<const char*>(parent));
   #else
   //This is the traditional C-front approach: __MWERKS__, __DMC__, __SUNPRO_CC
   return (*(const std::ptrdiff_t*)(void*)&ptr_to_member) - 1;
   #endif
}

template<class Parent, class Member>
inline Parent *parent_from_member(Member *member, const Member Parent::* ptr_to_member)
{
   return (Parent*)((char*)member - 
      offset_from_pointer_to_member(ptr_to_member));
}

template<class Parent, class Member>
inline const Parent *parent_from_member(const Member *member, const Member Parent::* ptr_to_member)
{
   return (const Parent*)((const char*)member - 
      offset_from_pointer_to_member(ptr_to_member));
}

template<class Parent, class Member, Member Parent::* PointerToMember>
struct pointer_to_member_metadata
{
    typedef Parent parent_type;
    typedef Member member_type;
    static const Member Parent::* value = PointerToMember;
}

struct pointer_to_member_offset_accumulator
{
    std::ptrdiff_t sum_;
    template<class PointerToMemberMetadata>
    void operator()(mpl::identity<PointerToMemberMetadata>)
    {
        sum_ += offset_from_pointer_to_member(PointerToMemberMetadata::value);
    }
}

template<class PointerToMemberSequence>
inline std::ptrdiff_t offset_from_pointers_to_members()
{
    // unlikely that this will optimize away as it would
    // if offset_from_pointer_to_member were a metafunction
    pointer_to_member_offset_accumulator sum;
    mpl::for_each<PointerToMemberSequence>(sum);
    return sum.sum_;
}


}  //namespace detail {
}  //namespace boost {

#ifdef BOOST_INTRUSIVE_MSVC_COMPLIANT_PTR_TO_MEMBER
#undef BOOST_INTRUSIVE_MSVC_COMPLIANT_PTR_TO_MEMBER
#endif

#include <boost/intrusive/detail/config_end.hpp>

#endif   //#ifndef BOOST_INTRUSIVE_PARENT_FROM_MEMBER_HPP
