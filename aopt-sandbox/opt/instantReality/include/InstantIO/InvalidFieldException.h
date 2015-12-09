#ifndef __INSTANTIO_INVALIDFIELDEXCEPTION_H
#define __INSTANTIO_INVALIDFIELDEXCEPTION_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: InvalidFieldException.h                                         //
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
 * @class InvalidFieldException InvalidFieldException.h InstantIO/InvalidFieldException.h
 *
 * Gets thrown when an application tries to access a field
 * using an unknown name.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING InvalidFieldException: public Exception
{
public:

	/**
	 * Creates a new %InvalidFieldException object with an empty error message.
	 */
	inline InvalidFieldException(): Exception() {}

	/**
	 * Creates a new %InvalidFieldException object with an error message.
	 * @param msg A message describing the error that occurred.
	 */
	inline InvalidFieldException(const std::string &msg): Exception(msg) {}

	/**
	 * Creates a new %InvalidFieldException object that is an exact copy of
	 * another %InvalidFieldException object.
	 * @param other The other %InvalidFieldException object
	 */
	inline InvalidFieldException(const InvalidFieldException &other): Exception(other) {}

private:

	/** Copy operator (not implemented!) */
	const InvalidFieldException &operator=(const InvalidFieldException &);
};


} // namespace InstantIO


#endif // __INSTANTIO_INVALIDFIELDEXCEPTION_H
