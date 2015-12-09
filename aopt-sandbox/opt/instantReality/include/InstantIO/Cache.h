#ifndef __INSTANTIO_CACHE_H
#define __INSTANTIO_CACHE_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Cache.h                                                         //
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
#include "BasicCache.h"
#include "RefCtr.h"
#include <stack>
#include <exception>
#ifdef __sgi
# include <assert.h>
#else
# include <cassert>
#endif


namespace InstantIO
{


/**
 * @class Cache Cache.h InstantIO/Cache.h
 *
 * Cache is a helper class used to cache objects.
 * @author Patrick D&auml;hne
 */
template <class T> class Cache: public BasicCache
{
public:

	/**
	 * Destroys the %Cache object. Automatically
	 * destroys all objects stored in the cache.
	 * Throws an exception when there is any object
	 * still in use by the application.
	 */
	~Cache()
	{
		if (cache_.size() != memory_.size())
#if (defined(__GNUC__) || defined(__sgi))
			throw std::exception();
#else
			throw std::exception("Trying to release a Cache object that has unreleased data objects");
#endif
		while (!memory_.empty())
		{
			delete memory_.top();
			memory_.pop();
		}
	};

	/**
	 * Returns the single instance of the %Cache class
	 * %Cache is a singleton, i.e. only one instance of
	 * this class exists. You can access this instance
	 * by calling this method.
	 * @return The single instance of the %Cache class
	 */
	inline static Cache<T> &the()
	{ return the_; }

	/**
	 * Acquires a new object. Call this method when
	 * you need a new object. When there is an unused
	 * object available in the cache, returns this
	 * object. Otherwise, creates a new object. Because
	 * you may get an object already used before, you
	 * have to initialize the object! Call the \e
	 * release method when you do not need the object
	 * any more.
	 * @return The new object
	 * @see release
	 */
	RefCtr<T> *acquire()
	{
		RefCtr<T> *value;

		lock();
		if (cache_.empty())
		{
			value = new RefCtr<T>;
			memory_.push(value);
		}
		else
		{
			value = cache_.top();
			cache_.pop();
		}
		unlock();

		return value;
	};

	/**
	 * Releases an object. Call this method when you
	 * do not need an object you got from the
	 * acquire method any more. The object is
	 * inserted into the cache and reused when you
	 * call acquire again. Do not try to access
	 * the object after calling this method!
	 * @param value The object
	 * @see acquire
	 */
	void release(RefCtr<T> *value)
	{
		assert(value != 0);
		lock();
		cache_.push(value);
		unlock();
	};

// for some reason, protected does not
// work at this point -pdaehne
//protected:

	/**
	 * Creates a new %Cache object. The Constructor is protected
	 * because you are not allowed to create %Cache objects.
	 * %Cache is a singleton, i.e. only one instance of %Cache exists.
	 * You can access this instance by calling the the method.
	 * @see the
	 */
	inline Cache(): BasicCache() {}

private:

	/** Copy constructor (not implemented!) */
	Cache(const Cache<T> &);

	/** Copy operator (not implemented!) */
	const Cache<T> &operator=(const Cache<T> &);

	/** The single instance of the %Cache template class */
	static Cache<T> the_;

	/** Stack of all object managed by this Cache object */
	std::stack<RefCtr<T>*> memory_;

	/** Stack of all unused objects */
	std::stack<RefCtr<T>*> cache_;
};


/** The single instance of the %Cache template class */
template <class T> Cache<T> Cache<T>::the_;


} // namespace InstantIO


#endif // __INSTANTIO_CACHE_H
