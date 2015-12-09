#ifndef __INSTANTIO_BASICFIELDACCESSOR_H
#define __INSTANTIO_BASICFIELDACCESSOR_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: BasicFieldAccessor.h                                            //
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
#include "InvalidValueException.h"
#include <string>
#include <sstream>


namespace InstantIO
{

#define INSTANTIO_SETTER(type, function_name, variable_name) \
  void set##function_name(const type &temp) \
  { \
    variable_name = temp; \
  }

#define INSTANTIO_GETTER(type, function_name, variable_name) \
  type get##function_name() const \
  { \
    return variable_name; \
  }

#define INSTANTIO_SETTER_GETTER(type, function_name, variable_name) \
  INSTANTIO_GETTER(type, function_name, variable_name) \
  INSTANTIO_SETTER(type, function_name, variable_name)

class Node;

#define STRING_DELIMITER " "


/**
 * @class BasicFieldAccessor BasicFieldAccessor.h InstantIO/BasicFieldAccessor.h
 *
 * Abstract base class of all FieldAccessor templates.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING BasicFieldAccessor
{
public:

	/** Destroys the %BasicFieldAccessor object */
	virtual ~BasicFieldAccessor();

	/**
	 * Sets the value of the field.
	 * @param node The Node that contains the field.
	 * @param value The new value of the field.
	 */
	virtual void set(Node &node, const std::string &value) const = 0;

	/**
	 * Returns the value of the field.
	 * @param node The Node that contains the field.
	 * @return The value of the field.
	 */
	virtual std::string get(const Node &node) const = 0;

	/**
	 * Checks if a field is set to given value.
	 * @param node The Node that contains the field.
	 * @param value The value we compare the field with.
	 * @return true when the field has the given value,
	 * false otherwise.
	 */
	virtual bool sameValue(const Node &node, const std::string &value) const = 0;

	/**
	 * Creates a copy a FieldAccessor object.
	 * @return The new copy
	 */
	virtual BasicFieldAccessor *clone() const = 0;

protected:

	/** Creates a new %BasicFieldAccessor object */
	inline BasicFieldAccessor() {}

	/**
	 * Creates a new %BasicFieldAccessor object that is
	 * an exact copy of another %BasicFieldAccessor object.
	 * @param other The other %BasicFieldAccessor object.
	 */
	inline BasicFieldAccessor(const BasicFieldAccessor &other) {}

	/**
	 * Converts a value to a string.
	 * @param value The value.
	 * @return The resulting string.
	 */
	template <class T> static inline const std::string value2String(const T &value)
	{
		// Transform the value to a string
		std::ostringstream os;
		os << value;
		return os.str();
	}

	/**
	 * Converts a value Container to a string.
	 * @param value The value.
	 * @return The resulting string.
	 */
	template <class C> static inline const std::string valueContainer2String(const C &value)
	{
    std::ostringstream os;

    // Transforms multiple values to a string
    for(typename C::const_iterator iter = value.begin(); iter != value.end(); ++iter)
      os << *iter << STRING_DELIMITER;

    return os.str();
	}

	/**
	 * Converts a boolean value to a string
	 * @param value The boolean value.
	 * @return The resulting string.
	 */
	static const std::string &value2String(bool value);

	/**
	 * Converts a string value to a string.
	 * @param value The string value.
	 * @return The resulting string.
	 */
	// Don't change the declaration to return a reference -
	// MS C++ needs it this way! -pdaehne
	static inline const std::string value2String(const std::string &value)
	{ return value; }

	/**
	 * Converts a string to a value. When the the conversion fails,
	 * a default value gets returned.
	 * @param strValue The string.
	 * @param value Gets filled with the converted value.
	 */
	template <class T> static inline void string2Value(const std::string &strValue, T &value)
	{
		// Transform value to requested data type
		std::istringstream is(strValue);
		is >> value;
		if (is.fail())
			throw InvalidValueException(std::string("Cannot convert \"") + strValue + std::string("\" to a value"));
		// The following line just enforces eof.
		is.get();
		if (!is.eof())
			throw InvalidValueException(std::string("Cannot convert \"") + strValue + std::string("\" to a value"));
	}

	/**
	 * Converts a string Container to a value array. When the the conversion fails,
	 * a default value gets returned.
	 * @param strValue The string.
	 * @param value Gets filled with the converted value.
	 */
	template <class T, class C> static inline void string2ValueContainer(const std::string &strValue, C &value)
	{
    unsigned offset = 0;
    while(offset < strValue.size())
    {
      const unsigned begin = strValue.find_first_not_of(STRING_DELIMITER, offset);
      const unsigned end = strValue.find_first_of(STRING_DELIMITER, begin);
      const int length = (end == -1) ? -1 : end - begin;

      T result;
      string2Value(strValue.substr(begin, length), result);

      value.push_back(result);
      offset = end;
    }
  }

	/**
	 * Converts a string to a boolean value. When the the conversion
	 * fails, a default value gets returned.
	 * @param strValue The string.
	 * @param value Gets filled with the converted boolean value.
	 */
	static void string2Value(const std::string &strValue, bool &value);

	/**
	 * Converts a string to a string value.
	 * @param strValue The string.
	 * @param value Gets filled with the string value.
	 */
	static inline void string2Value(const std::string &strValue, std::string &value)
	{ value = strValue; }

private:

	/** Copy operator (not implemented!) */
	const BasicFieldAccessor &operator=(const BasicFieldAccessor &);
};


} // namespace InstantIO


#endif // __INSTANTIO_BASICFIELDACCESSOR_H
