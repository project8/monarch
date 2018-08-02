/*
 * M3MemberVariable.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 *  This version has been specialized for Monarch: variables are mutable and setters are const.
 */

#ifndef M3MEMBERVARIABLE_HH_
#define M3MEMBERVARIABLE_HH_

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
 *  - M3MEMBERVARIABLE
 *  - M3MEMBERVARIABLE_NOSET
 *  - M3MEMBERVARIABLE_STATIC
 *  - M3MEMBERVARIABLE_STATIC_NOSET
 *
 * For variables accessed by reference
 * Defines accessors [const type& MyVar() const], [type& MyVar()], and member variable [type fMyVar]
 * The non-const function is not available if the _CONST macros are used
 *  - M3MEMBERVARIABLE_REF
 *  - M3MEMBERVARIABLE_REF_CONST
 *  - M3MEMBERVARIABLE_REF_STATIC
 *  - M3MEMBERVARIABLE_REF_STATIC_CONST
 *
 * For pointer variables
 * Defines accessors [type* GetMyVar() const], [void SetMyVar( type* )], and member variable [type* fMyVar]
 * The Set function is not available if the _NOSET macros are used
 *  - M3MEMBERVARIABLE_PTR
 *  - M3MEMBERVARIABLE_PTR_NOSET
 *  - M3MEMBERVARIABLE_PTR_STATIC
 *  - M3MEMBERVARIABLE_PTR_STATIC_NOSET
 *
 * For shared_ptr's
 * Defines accessors [const std::shared_ptr< type > MyVar() const], [std::shared_ptr< type > MyVar()], and member variable [std::shared_ptr< type > fMyVar]
 * The non-const function is not available if the _CONST macros are used
 *  - M3MEMBERVARIABLE_SHARED_PTR
 *  - M3MEMBERVARIABLE_SHARED_PTR_CONST
 *  - M3MEMBERVARIABLE_SHARED_PTR_STATIC
 *  - M3MEMBERVARIABLE_SHARED_PTR_STATIC_CONST
 *
 * For atomic variables
 * Defines accessors [type GetMyVar() const], [void SetMyVar( type )], and member variable [std::atomic< type > fMyVar]
 * The Set function is not available if the _NOSET macros are used
 *  - M3MEMBERVARIABLE_ATOMIC
 *  - M3MEMBERVARIABLE_ATOMIC_NOSET
 *  - M3MEMBERVARIABLE_ATOMIC_STATIC
 *  - M3MEMBERVARIABLE_ATOMIC_STATIC_NOSET
 *
 */

#define set_prefix Set
#define get_prefix Get
#define var_prefix f
#define static_prefix s

#include "_member_variables.hh"

#define M3MEMBERVARIABLE                         mv_accessible_mutable
#define M3MEMBERVARIABLE_NOSET                   mv_accessible_mutable_noset
#define M3MEMBERVARIABLE_STATIC                  mv_accessible_static
#define M3MEMBERVARIABLE_STATIC_NOSET            mv_accessible_static_noset

#define M3MEMBERVARIABLE_REF                     mv_referrable_mutable
#define M3MEMBERVARIABLE_REF_CONST               mv_referrable_mutable
#define M3MEMBERVARIABLE_REF_STATIC              mv_referrable_static
#define M3MEMBERVARIABLE_REF_STATIC_CONST        mv_referrable_static_const

#define M3MEMBERVARIABLE_PTR                     mv_assignable_mutable
#define M3MEMBERVARIABLE_PTR_NOSET               mv_assignable_mutable_noset
#define M3MEMBERVARIABLE_PTR_STATIC              mv_assignable_static
#define M3MEMBERVARIABLE_PTR_STATIC_NOSET        mv_assignable_static_noset

#define M3MEMBERVARIABLE_SHARED_PTR              mv_shared_ptr_mutable
#define M3MEMBERVARIABLE_SHARED_PTR_NOSET        mv_shared_ptr_mutable
#define M3MEMBERVARIABLE_SHARED_PTR_STATIC       mv_shared_ptr_static
#define M3MEMBERVARIABLE_SHARED_PTR_STATIC_NOSET mv_shared_ptr_static

#define M3MEMBERVARIABLE_ATOMIC                  mv_atomic_mutable
#define M3MEMBERVARIABLE_ATOMIC_NOSET            mv_atomic_mutable_noset
#define M3MEMBERVARIABLE_ATOMIC_STATIC           mv_atomic_static
#define M3MEMBERVARIABLE_ATOMIC_STATIC_NOSET     mv_atomic_static_noset

#endif /* M3MEMBERVARIABLE_HH_ */
