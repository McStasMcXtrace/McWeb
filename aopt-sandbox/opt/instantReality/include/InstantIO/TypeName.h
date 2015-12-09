#ifndef __INSTANTIO_TYPENAME_H
#define __INSTANTIO_TYPENAME_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: TypeName.h                                                      //
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
#include <typeinfo>


namespace InstantIO
{


/**
 * @class TypeName TypeName.h InstantIO/TypeName.h
 *
 * Helper class that transforms data types
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <class T> class INSTANTIO_DLLMAPPING TypeName
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName()
	{ return typeid(T).name(); };

private:

	/** Default constructor (not implemented!) */
	TypeName();

	/** Copy constructor (not implemented!) */
	TypeName(const TypeName &);

	/** Copy operator (not implemented!) */
	const TypeName &operator=(const TypeName &);
};


/**
 * Helper class that transforms the bool data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<bool>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();

private:

	/** Default constructor (not implemented!) */
	TypeName();

	/** Copy constructor (not implemented!) */
	TypeName(const TypeName &);

	/** Copy operator (not implemented!) */
	const TypeName &operator=(const TypeName &);
};


/**
 * Helper class that transforms the double data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<double>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();

private:

	/** Default constructor (not implemented!) */
	TypeName();

	/** Copy constructor (not implemented!) */
	TypeName(const TypeName &);

	/** Copy operator (not implemented!) */
	const TypeName &operator=(const TypeName &);
};


/**
 * Helper class that transforms the float data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<float>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();

private:

	/** Default constructor (not implemented!) */
	TypeName();

	/** Copy constructor (not implemented!) */
	TypeName(const TypeName &);

	/** Copy operator (not implemented!) */
	const TypeName &operator=(const TypeName &);
};


/**
 * Helper class that transforms the long data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<long>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();

private:

	/** Default constructor (not implemented!) */
	TypeName();

	/** Copy constructor (not implemented!) */
	TypeName(const TypeName &);

	/** Copy operator (not implemented!) */
	const TypeName &operator=(const TypeName &);
};


/**
 * Helper class that transforms the std::string data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<std::string>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();

private:

	/** Default constructor (not implemented!) */
	TypeName();

	/** Copy constructor (not implemented!) */
	TypeName(const TypeName &);

	/** Copy operator (not implemented!) */
	const TypeName &operator=(const TypeName &);
};


} // namespace InstantIO


#endif // __INSTANTIO_TYPENAME_H
