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
 * Creates a member variable with type TYPE name f[NAME], plus getter and setter.
 * This version has been specialized for Monarch: variables are mutable and setters are const.
 * MEMBERVARIABLE_NOSET will provide the variable and getter, but no setter, allowing you to provide a custom setter.
 *
 * Usage example, in a class header file:
 *     MEMBERVARIABLE(double, MyVar)
 *
 * You still need to initialize the variables in the class constructors.
 *
 * The generated code is:
 *     private:
 *         TYPE f[NAME];
 *     public:
 *         inline TYPE Get[NAME]() const
 *         {
 *             return f[NAME];
 *         }
 *         inline void Set[NAME](TYPE var)
 *         {
 *             f[NAME] = var;
 *         }
 */
#define M3MEMBERVARIABLE_NOSET(TYPE, NAME) \
        private: \
            mutable TYPE f##NAME; \
        public: \
            inline TYPE Get##NAME() const {return f##NAME;} \

#define M3MEMBERVARIABLE(TYPE, NAME) \
        M3MEMBERVARIABLE_NOSET(TYPE, NAME) \
            inline void Set##NAME(TYPE var) const {f##NAME = var; return;}

#define M3MEMBERVARIABLE_PROTECTED_NOSET(TYPE, NAME) \
        protected: \
            TYPE f##NAME; \
        public: \
            inline TYPE Get##NAME() const {return f##NAME;} \

#define M3MEMBERVARIABLE_PROTECTED(TYPE, NAME) \
        M3MEMBERVARIABLE_PROTECTED_NOSET(TYPE, NAME) \
            inline void Set##NAME(TYPE var) const {f##NAME = var; return;}



/**
 * Creates a member variable with type TYPE name f[NAME], plus getters and setters.
 * This version has been specialized for Monarch: variables are mutable and setters are const.
 * M3MEMBERVARIABLEREF_NOSET will provide the variable and getter, but no setter, allowing you to provide a custom setter.
 *
 * Usage example, in a class header file:
 *     M3MEMBERVARIABLEREF(std::string, MyVar)
 *
 * You still need to initialize the variables in the class constructors.
 *
 * The generated code is:
 *     private:
 *         mutable TYPE f[NAME];
 *     public:
 *         inline const TYPE& Get[NAME]() const
 *         {
 *             return f[NAME];
 *         }
 *         inline void Set[NAME](const TYPE& var) const
 *         {
 *             f[NAME] = var;
 *         }
 */
#define M3MEMBERVARIABLEREF_NOSET(TYPE, NAME) \
        private: \
            mutable TYPE f##NAME; \
        public: \
            inline const TYPE& Get##NAME() const {return f##NAME;}

#define M3MEMBERVARIABLEREF(TYPE, NAME) \
        M3MEMBERVARIABLEREF_NOSET(TYPE, NAME) \
            inline void Set##NAME(const TYPE& var) const {f##NAME = var; return;}

#define M3MEMBERVARIABLEREF_PROTECTED_NOSET(TYPE, NAME) \
        protected: \
            TYPE f##NAME; \
        public: \
            inline const TYPE& Get##NAME() const {return f##NAME;}

#define M3MEMBERVARIABLEREF_PROTECTED(TYPE, NAME) \
        M3MEMBERVARIABLEREF_PROTECTED_NOSET(TYPE, NAME) \
            inline void Set##NAME(const TYPE& var) const {f##NAME = var; return;}


#endif /* M3MEMBERVARIABLE_HH_ */
