#ifndef __INSTANTIO_EXCEPTION_H
#define __INSTANTIO_EXCEPTION_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Exception.h                                                     //
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
#include <string>


namespace InstantIO
{


/**
 * @class Exception Exception.h InstantIO/Exception.h
 *
 * Base class of all exceptions thrown by the InstantIO library.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING Exception
{
public:

	/**
	 * Creates a new %Exception object with an empty error message.
	 */
	inline Exception(): msg_() {}

	/**
	 * Creates a new %Exception object with an error message.
	 * @param msg A message describing the error that occurred.
	 */
	inline explicit Exception(const std::string &msg): msg_(msg) {}

	/**
	 * Creates a new %Exception object that is an exact copy of
	 * another %Exception object.
	 * @param other The other %Exception object
	 */
	inline Exception(const Exception &other): msg_(other.msg_) {}

	/**
	 * Returns an error message describing the error that occurred.
	 * @return The error message.
	 */
	inline const char *what() const { return msg_.c_str(); }

protected:

	/** The error message */
	std::string msg_;

private:

	/** Copy operator (not implemented!) */
	const Exception &operator=(const Exception &);
};


} // namespace InstantIO


#endif // __INSTANTIO_EXCEPTION_H
