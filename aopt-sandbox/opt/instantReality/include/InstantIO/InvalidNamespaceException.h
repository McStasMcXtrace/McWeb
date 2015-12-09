#ifndef __INSTANTIO_INVALIDNAMESPACEEXCEPTION_H
#define __INSTANTIO_INVALIDNAMESPACEEXCEPTION_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: InvalidNamespaceException.h                                     //
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
 * @class InvalidNamespaceException InvalidNamespaceException.h InstantIO/InvalidNamespaceException.h
 *
 * Gets thrown when an application tries to access a Namespace
 * using an unknown name.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING InvalidNamespaceException: public Exception
{
public:

	/**
	 * Creates a new %InvalidNamespaceException object with an empty error message.
	 */
	inline InvalidNamespaceException(): Exception() {}

	/**
	 * Creates a new %InvalidNamespaceException object with an error message.
	 * @param msg A message describing the error that occurred.
	 */
	inline InvalidNamespaceException(const std::string &msg): Exception(msg) {}

	/**
	 * Creates a new %InvalidNamespaceException object that is an exact copy of
	 * another %InvalidNamespaceException object.
	 * @param other The other %InvalidNamespaceException object
	 */
	inline InvalidNamespaceException(const InvalidNamespaceException &other): Exception(other) {}

private:

	/** Copy operator (not implemented!) */
	const InvalidNamespaceException &operator=(const InvalidNamespaceException &);
};


} // namespace InstantIO


#endif // __INSTANTIO_INVALIDNAMESPACEEXCEPTION_H
