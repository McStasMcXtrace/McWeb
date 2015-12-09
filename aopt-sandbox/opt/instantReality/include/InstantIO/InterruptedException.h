#ifndef __INSTANTIO_INTERRUPTEDEXCEPTION_H
#define __INSTANTIO_INTERRUPTEDEXCEPTION_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: InterruptedException.h                                          //
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
#include "Exception.h"
#include <string>


namespace InstantIO
{


/**
 * @class InterruptedException InterruptedException.h InstantIO/InterruptedException.h
 *
 * Gets thrown when a thread blocked at an OutSlot or InSlot gets
 * interrupted.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING InterruptedException: public Exception
{
public:

	/**
	 * Creates a new %InterruptedException object with an empty error message.
	 */
	inline InterruptedException(): Exception() {}

	/**
	 * Creates a new %InterruptedException object with an error message.
	 * @param msg A message describing the error that occurred.
	 */
	inline InterruptedException(const std::string &msg): Exception(msg) {}

	/**
	 * Creates a new %InterruptedException object that is an exact copy of
	 * another %InterruptedException object.
	 * @param other The other %InterruptedException object
	 */
	inline InterruptedException(const InterruptedException &other): Exception(other) {}

private:

	/** Copy operator (not implemented!) */
	const InterruptedException &operator=(const InterruptedException &);
};


} // namespace InstantIO


#endif // __INSTANTIO_INTERRUPTEDEXCEPTION_H
