/*
 * MMemberVariable.hh
 *
 *  Created on: Dec 4, 2014
 *      Author: nsoblath
 *  This version has been specialized for Monarch: variables are mutable and setters are const.
 */

#ifndef MMEMBERVARIABLE_HH_
#define MMEMBERVARIABLE_HH_


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
#define MMEMBERVARIABLE_NOSET(TYPE, NAME) \
        private: \
            mutable TYPE f##NAME; \
        public: \
            inline TYPE Get##NAME() const {return f##NAME;} \

#define MMEMBERVARIABLE(TYPE, NAME) \
        MMEMBERVARIABLE_NOSET(TYPE, NAME) \
            inline void Set##NAME(TYPE var) const {f##NAME = var; return;}

#define MMEMBERVARIABLE_PROTECTED_NOSET(TYPE, NAME) \
        protected: \
            TYPE f##NAME; \
        public: \
            inline TYPE Get##NAME() const {return f##NAME;} \

#define MMEMBERVARIABLE_PROTECTED(TYPE, NAME) \
        MMEMBERVARIABLE_PROTECTED_NOSET(TYPE, NAME) \
            inline void Set##NAME(TYPE var) const {f##NAME = var; return;}



/**
 * Creates a member variable with type TYPE name f[NAME], plus getters and setters.
 * This version has been specialized for Monarch: variables are mutable and setters are const.
 * MMEMBERVARIABLEREF_NOSET will provide the variable and getter, but no setter, allowing you to provide a custom setter.
 *
 * Usage example, in a class header file:
 *     MMEMBERVARIABLEREF(std::string, MyVar)
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
#define MMEMBERVARIABLEREF_NOSET(TYPE, NAME) \
        private: \
            mutable TYPE f##NAME; \
        public: \
            inline const TYPE& Get##NAME() const {return f##NAME;}

#define MMEMBERVARIABLEREF(TYPE, NAME) \
        MMEMBERVARIABLEREF_NOSET(TYPE, NAME) \
            inline void Set##NAME(const TYPE& var) const {f##NAME = var; return;}

#define MMEMBERVARIABLEREF_PROTECTED_NOSET(TYPE, NAME) \
        protected: \
            TYPE f##NAME; \
        public: \
            inline const TYPE& Get##NAME() const {return f##NAME;}

#define MMEMBERVARIABLEREF_PROTECTED(TYPE, NAME) \
        MMEMBERVARIABLEREF_PROTECTED_NOSET(TYPE, NAME) \
            inline void Set##NAME(const TYPE& var) const {f##NAME = var; return;}


#endif /* MMEMBERVARIABLE_HH_ */
