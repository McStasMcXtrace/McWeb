#ifndef __INSTANTIO_REFCTR_H
#define __INSTANTIO_REFCTR_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: RefCtr.h                                                        //
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
#include "BasicRefCtr.h"


namespace InstantIO
{


/**
 * @class RefCtr RefCtr.h InstantIO/RefCtr.h
 *
 * A simple helper class that performs reference counting.
 * @author Patrick D&auml;hne
 */
template <class T> class RefCtr: public BasicRefCtr
{
public:

	/**
	 * Initializes the reference counter with 0.
	 */
	inline RefCtr(): BasicRefCtr(), value_() {}

	/**
	 * Returns the data object managed by the referencecounter object.
	 * @return The data object
	 */
	inline const T &getValue() const { return value_; }

	/**
	 * Returns the data object managed by the referencecounter object.
	 * @return The data object
	 */
	inline T &getValue() { return value_; }

private:

	/** Copy constructor (not implemented!) */
	RefCtr(const RefCtr<T> &);

	/** Copy operator (not implemented!) */
	const RefCtr<T> &operator=(const RefCtr<T> &);

	/** The data object managed by the referencecounter object */
	T value_;
};


} // namespace InstantIO


#endif // __INSTANTIO_REFCTR_H
