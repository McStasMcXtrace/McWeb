#ifndef __INSTANTIO_INVALIDVALUEEXCEPTION_H
#define __INSTANTIO_INVALIDVALUEEXCEPTION_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: InvalidValueException.h                                         //
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
 * @class InvalidValueException InvalidValueException.h InstantIO/InvalidValueException.h
 *
 * Gets thrown when an application tries to set a field
 * using an invalid value.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING InvalidValueException: public Exception
{
public:

	/**
	 * Creates a new %InvalidValueException object with an empty error message.
	 */
	inline InvalidValueException(): Exception() {}

	/**
	 * Creates a new %InvalidValueException object with an error message.
	 * @param msg A message describing the error that occurred.
	 */
	inline InvalidValueException(const std::string &msg): Exception(msg) {}

	/**
	 * Creates a new %InvalidValueException object that is an exact copy of
	 * another %InvalidValueException object.
	 * @param other The other %InvalidValueException object
	 */
	inline InvalidValueException(const InvalidValueException &other): Exception(other) {}

private:

	/** Copy operator (not implemented!) */
	const InvalidValueException &operator=(const InvalidValueException &);
};


} // namespace InstantIO


#endif // __INSTANTIO_INVALIDVALUEEXCEPTION_H
