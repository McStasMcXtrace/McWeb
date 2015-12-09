#ifndef __INSTANTIO_BASICCACHE_H
#define __INSTANTIO_BASICCACHE_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: BasicCache.h                                                    //
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
namespace Medusa
{
	class Mutex;
}


namespace InstantIO
{

/**
 * @class BasicCache BasicCache.h InstantIO/BasicCache.h
 *
 * Abstract base class of all Cache templates.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING BasicCache
{
protected:

	/** Creates a new %BasicCache object */
	BasicCache();

	/** Destroys the %BasicCache object */
	~BasicCache();

	/** Locks the Cache object */
	void lock();

	/** Unlocks the Cache object */
	void unlock();

private:

	/** Copy constructor (not implemented!) */
	BasicCache(const BasicCache &);

	/** Copy operator (not implemented!) */
	const BasicCache &operator=(const BasicCache &);

	/** Mutex that locks the Cache object */
	Medusa::Mutex *mutex_;
};


} // namespace InstantIO


#endif // __INSTANTIO_BASICCACHE_H
