#ifndef __INSTANTIO_SMARTPTR_H
#define __INSTANTIO_SMARTPTR_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: SmartPtr.h                                                      //
//                                                                           //
//                                                                           //
// Author: Fraunhofer Institut fuer Graphische Datenverarbeitung (IGD)       //
//         Department Virtual and Augmented Reality                          //
//         Fraunhoferstrasse 5                                               //
//         64283 Darmstadt, Germany                                          //
//                                                                           //
// Rights: Copyright (c) 2009 by Fraunhofer IGD.                             //
//         All rights reserved.                                              //
//         Fraunhofer IGD provides this product without warranty of any kind //
//         and shall not be liable for any damages caused by the use         //
//         of this product.                                                  //
//                                                                           //
// ========================================================================= //

#include "InstantIODef.h"
#include "RefCtr.h"
#include "Cache.h"


namespace InstantIO
{


/**
 * @class SmartPtr SmartPtr.h InstantIO/SmartPtr.h
 *
 * Smartpointer class
 */
template <class T> class SmartPtr
{
public:
    typedef T ValueType;

    inline bool isValid() const
    {
        return refCtr_ != 0 && cache_ != 0;
    }

	/// Constructor
	/**
	 * Creates a new %SmartPtr object.
	 */
	inline SmartPtr(): refCtr_(0), cache_(0) {}

	/// Copy constructor
	/**
	 * \overload
	 */
	inline SmartPtr(const SmartPtr &other)
	: refCtr_(other.refCtr_), cache_(other.cache_)
	{ incrementReferenceCounter(); }

	/// Destructor
	/**
	 * Destroys a %SmartPtr object.
	 */
	inline ~SmartPtr()
	{ decrementReferenceCounter(); }

	/// Copy operator
	/**
	 * Copies the value from another smartpointer.
	 * \param other The other smartpointer
	 * \return Itself
	 */
	inline const SmartPtr<T> &operator=(const SmartPtr<T> &other)
	{
		if (this != &other)
		{
			decrementReferenceCounter();
			refCtr_ = other.refCtr_;
			cache_ = other.cache_;
			incrementReferenceCounter();
		}
		return *this;
	}

	/// Dereferenciation operator
	/**
	 * Returns the value the smartpointer points at.
	 * \return The value
	 */
	inline const T &operator*() const
	{
		//if (refCtr_ == 0)
		//	acquire();
		assert(refCtr_ != 0);
		return refCtr_->getValue();
	}

	/// Dereferenciation operator
	/**
	 * \overload
	 */
	inline T &operator*()
	{
		if (refCtr_ == 0)
			acquire();
		return refCtr_->getValue();
	}

	/// Pointer operator
	/**
	 * Returns a pointer of the value the smartpointer points at.
	 * \return The pointer to the value
	 */
	inline const T *operator->() const
	{
		//if (refCtr_ == 0)
		//	acquire();
		assert(refCtr_ != 0);
		return &(refCtr_->getValue());
	}

	/// Pointer operator
	/**
	 * \overload
	 */
	inline T *operator->()
	{
		if (refCtr_ == 0)
			acquire();
		return &(refCtr_->getValue());
	}

	/// Lets the pointer point to a new object
	/**
	 * Calling this methods lets the pointer point
	 * to a new object.
	 */
	inline void newObject()
	{
		decrementReferenceCounter();
		refCtr_ = 0;
		cache_ = 0;
	}

private:

	/// Reference counter
	RefCtr<T> *refCtr_;

	/// The cache that is responsible the data object
	// Ugly, but this is the only way to make it DLL-safe -pdaehne
	Cache<T> *cache_;

	/// Acquires a new reference counter
	void acquire()
	{
		cache_ = &(Cache<T>::the());
		refCtr_ = cache_->acquire();
		refCtr_->incrementReferenceCounter();
	};

	/// Increments the reference counter
	inline void incrementReferenceCounter()
	{
		if (refCtr_ != 0)
			refCtr_->incrementReferenceCounter();
	}

	/// Decrements the reference counter
	inline void decrementReferenceCounter()
	{
		if ((refCtr_ != 0) && (refCtr_->decrementReferenceCounter() == 0))
			cache_->release(refCtr_);
	}
};


} // namespace InstantIO


#endif // __INSTANTIO_SMARTPTR_H
