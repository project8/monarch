/*
 * M4MemberVariable.hh
 *
 *  Created on: Apr 12, 2023
 *      Author: N.S. Oblath
 *  This version has been specialized for Monarch: variables are mutable and setters are const.
 */

#ifndef M4MEMBERVARIABLE_HH_
#define M4MEMBERVARIABLE_HH_

/**
 * Macros for class member variables
 *
 * Special setup for Monarch: all of the variables are mutable so that they can be modified when a file is being read.
 * This follows the Monarch use of "const" for read-only mode.
 *
 * In all cases remember to initialize the variables!
 *
 * For "normal" variables
 * Defines accessors [type GetMyVar() const], [void SetMyVar( type )], and member variable [type fMyVar]
 * The Set function is not available if the _NOSET macros are used
 *  - M4MEMBERVARIABLE
 *  - M4MEMBERVARIABLE_NOSET
 *  - M4MEMBERVARIABLE_STATIC
 *  - M4MEMBERVARIABLE_STATIC_NOSET
 *
 * For variables accessed by reference
 * Defines accessors [const type& MyVar() const], [type& MyVar()], and member variable [type fMyVar]
 * The non-const function is not available if the _CONST macros are used
 *  - M4MEMBERVARIABLE_REF
 *  - M4MEMBERVARIABLE_REF_CONST
 *  - M4MEMBERVARIABLE_REF_STATIC
 *  - M4MEMBERVARIABLE_REF_STATIC_CONST
 *
 * For pointer variables
 * Defines accessors [type* GetMyVar() const], [void SetMyVar( type* )], and member variable [type* fMyVar]
 * The Set function is not available if the _NOSET macros are used
 *  - M4MEMBERVARIABLE_PTR
 *  - M4MEMBERVARIABLE_PTR_NOSET
 *  - M4MEMBERVARIABLE_PTR_STATIC
 *  - M4MEMBERVARIABLE_PTR_STATIC_NOSET
 *
 * For shared_ptr's
 * Defines accessors [const std::shared_ptr< type > MyVar() const], [std::shared_ptr< type > MyVar()], and member variable [std::shared_ptr< type > fMyVar]
 * The non-const function is not available if the _CONST macros are used
 *  - M4MEMBERVARIABLE_SHARED_PTR
 *  - M4MEMBERVARIABLE_SHARED_PTR_CONST
 *  - M4MEMBERVARIABLE_SHARED_PTR_STATIC
 *  - M4MEMBERVARIABLE_SHARED_PTR_STATIC_CONST
 *
 * For atomic variables
 * Defines accessors [type GetMyVar() const], [void SetMyVar( type )], and member variable [std::atomic< type > fMyVar]
 * The Set function is not available if the _NOSET macros are used
 *  - M4MEMBERVARIABLE_ATOMIC
 *  - M4MEMBERVARIABLE_ATOMIC_NOSET
 *  - M4MEMBERVARIABLE_ATOMIC_STATIC
 *  - M4MEMBERVARIABLE_ATOMIC_STATIC_NOSET
 *
 */

#include "_camel_case_member_variables.hh"

#define M4MEMBERVARIABLE                         camel_case_mv_accessible_mutable
#define M4MEMBERVARIABLE_NOSET                   camel_case_mv_accessible_mutable_noset
#define M4MEMBERVARIABLE_STATIC                  camel_case_mv_accessible_static
#define M4MEMBERVARIABLE_STATIC_NOSET            camel_case_mv_accessible_static_noset

#define M4MEMBERVARIABLE_REF                     camel_case_mv_referrable_mutable
#define M4MEMBERVARIABLE_REF_CONST               camel_case_mv_referrable_mutable
#define M4MEMBERVARIABLE_REF_STATIC              camel_case_mv_referrable_static
#define M4MEMBERVARIABLE_REF_STATIC_CONST        camel_case_mv_referrable_static_const

#define M4MEMBERVARIABLE_PTR                     camel_case_mv_assignable_mutable
#define M4MEMBERVARIABLE_PTR_NOSET               camel_case_mv_assignable_mutable_noset
#define M4MEMBERVARIABLE_PTR_STATIC              camel_case_mv_assignable_static
#define M4MEMBERVARIABLE_PTR_STATIC_NOSET        camel_case_mv_assignable_static_noset

#define M4MEMBERVARIABLE_SHARED_PTR              camel_case_mv_shared_ptr_mutable
#define M4MEMBERVARIABLE_SHARED_PTR_NOSET        camel_case_mv_shared_ptr_mutable
#define M4MEMBERVARIABLE_SHARED_PTR_STATIC       camel_case_mv_shared_ptr_static
#define M4MEMBERVARIABLE_SHARED_PTR_STATIC_NOSET camel_case_mv_shared_ptr_static

#define M4MEMBERVARIABLE_ATOMIC                  camel_case_mv_atomic_mutable
#define M4MEMBERVARIABLE_ATOMIC_NOSET            camel_case_mv_atomic_mutable_noset
#define M4MEMBERVARIABLE_ATOMIC_STATIC           camel_case_mv_atomic_static
#define M4MEMBERVARIABLE_ATOMIC_STATIC_NOSET     camel_case_mv_atomic_static_noset

#endif /* M4MEMBERVARIABLE_HH_ */
