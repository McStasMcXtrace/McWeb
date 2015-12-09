#ifndef __INSTANTIO_BASICREFCTR_H
#define __INSTANTIO_BASICREFCTR_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: BasicRefCtr.h                                                   //
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
 * @class BasicRefCtr BasicRefCtr.h InstantIO/BasicRefCtr.h
 *
 * Abstract base class of the RefCtr class.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING BasicRefCtr
{
public:

	/** Defines the data type used for the reference counter */
	typedef unsigned long ReferenceCounter;

	/**
	 * Increments the reference counter.
	 * @return The value of the reference counter after
	 * incrementing it.
	 */
	ReferenceCounter incrementReferenceCounter();

	/**
	 * Decrements the reference counter.
	 * @return The value of the reference counter after
	 * decrementing it.
	 */
	ReferenceCounter decrementReferenceCounter();

	/**
	 * Returns the value of the reference counter.
	 * @return The value of the reference counter.
	 */
	ReferenceCounter referenceCounter() const;

protected:

	/**
	 * Creates a new %BasicRefCtr object with a
	 * reference count of 0.
	 */
	BasicRefCtr();

	/**
	 * Destroys the %BasicRefCtr object.
	 */
	~BasicRefCtr();

private:

	/** Copy constructor (not implemented!) */
	BasicRefCtr(const BasicRefCtr &);

	/** Copy operator (not implemented!) */
	const BasicRefCtr &operator=(const BasicRefCtr &);

	/** Mutex that protects the reference counter */
	Medusa::Mutex *mutex_;

	/** The reference counter */
	volatile ReferenceCounter referenceCounter_;
};


} // namespace InstantIO


#endif // __INSTANTIO_BASICREFCTR_H
