#ifndef __INSTANTIO_INVALIDNODEEXCEPTION_H
#define __INSTANTIO_INVALIDNODEEXCEPTION_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: InvalidNodeException.h                                          //
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
 * @class InvalidNodeException InvalidNodeException.h InstantIO/InvalidNodeException.h
 *
 * Gets thrown when an application tries to create a
 * Node using an unknown name.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING InvalidNodeException: public Exception
{
public:

	/**
	 * Creates a new %InvalidNodeException object with an empty error message.
	 */
	inline InvalidNodeException(): Exception() {}

	/**
	 * Creates a new %InvalidNodeException object with an error message.
	 * @param msg A message describing the error that occurred.
	 */
	inline InvalidNodeException(const std::string &msg): Exception(msg) {}

	/**
	 * Creates a new %InvalidNodeException object that is an exact copy of
	 * another %InvalidNodeException object.
	 * @param other The other %InvalidNodeException object
	 */
	inline InvalidNodeException(const InvalidNodeException &other): Exception(other) {}

private:

	/** Copy operator (not implemented!) */
	const InvalidNodeException &operator=(const InvalidNodeException &);
};


} // namespace InstantIO


#endif // __INSTANTIO_INVALIDNODEEXCEPTION_H
