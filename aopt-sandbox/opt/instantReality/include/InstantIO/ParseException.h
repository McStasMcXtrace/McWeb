#ifndef __INSTANTIO_PARSEEXCEPTION_H
#define __INSTANTIO_PARSEEXCEPTION_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: ParseException.h                                                //
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
 * @class ParseException ParseException.h InstantIO/ParseException.h
 *
 * Gets thrown when a StateKeeper object cannot parse the
 * InstantIO state stored in a file.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING ParseException: public Exception
{
public:

	/**
	 * Creates a new %ParseException object with an empty error message.
	 */
	inline ParseException(): Exception() {}

	/**
	 * Creates a new %ParseException object with an error message.
	 * @param msg A message describing the error that occurred.
	 */
	inline ParseException(const std::string &msg): Exception(msg) {}

	/**
	 * Creates a new %ParseException object that is an exact copy of
	 * another %ParseException object.
	 * @param other The other %ParseException object
	 */
	inline ParseException(const ParseException &other): Exception(other) {}

private:

	/** Copy operator (not implemented!) */
	const ParseException &operator=(const ParseException &);
};


} // namespace InstantIO


#endif // __INSTANTIO_PARSEEXCEPTION_H
