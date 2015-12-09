#ifndef __INSTANTIO_FIELDACCESSOR_H
#define __INSTANTIO_FIELDACCESSOR_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: FieldAccessor.h                                                 //
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
#include "BasicFieldAccessor.h"
#include <string>


namespace InstantIO
{


class Node;

/**
 * @class FieldAccessor FieldAccessor.h InstantIO/FieldAccessor.h
 *
 * Helper class used to access contents of fields.
 * @author Patrick D&auml;hne
 */
template <class ClassType, class ValueType>
class FieldAccessor: public BasicFieldAccessor
{
public:

	/** Defines the method that sets the field value */
	typedef void (ClassType::*SetMethod)(const ValueType &);

	/** Defines the method that returns the field value */
	typedef ValueType (ClassType::*GetMethod)() const;

	/**
	 * Creates a new %FieldAccessor object.
	 * @param setMethod Pointer to the method that sets the
	 * field value.
	 * @param getMethod Pointer to the method that returns
	 * the field value.
	 */
	inline FieldAccessor(SetMethod setMethod, GetMethod getMethod)
	: BasicFieldAccessor(), setMethod_(setMethod), getMethod_(getMethod)
	{}

	/**
	 * Creates a new %FieldAccessor object that is an exact
	 * copy of another %FieldAccessor object.
	 * @param other The other %FieldAccessor object.
	 */
	inline FieldAccessor(const FieldAccessor<ClassType, ValueType> &other)
	: BasicFieldAccessor(other), setMethod_(other.setMethod_), getMethod_(other.getMethod_)
	{}

	/** Destroys the %FieldAccessor object */
	virtual ~FieldAccessor() {};

	/**
	 * Sets the value of the field.
	 * @param node The Node that contains the field.
	 * @param value The new value of the field.
	 */
	virtual void set(Node &node, const std::string &value) const
	{
		ValueType result;
		string2Value(value, result);
		ClassType &typedNode = dynamic_cast<ClassType&>(node);
		(typedNode.*setMethod_)(result);
	};

	/**
	 * Returns the value of the field.
	 * @param node The Node that contains the field.
	 * @return The value of the field.
	 */
	virtual std::string get(const Node &node) const
	{
		const ClassType &typedNode = dynamic_cast<const ClassType&>(node);
		return value2String((typedNode.*getMethod_)());
	};

	/**
	 * Checks if a field is set to given value.
	 * @param node The Node that contains the field.
	 * @param value The value we compare the field with.
	 * @return true when the field has the given value,
	 * false otherwise.
	 */
	virtual bool sameValue(const Node &node, const std::string &value) const
	{
		ValueType v;
		string2Value(value, v);
		const ClassType &typedNode = dynamic_cast<const ClassType&>(node);
		return v == (typedNode.*getMethod_)();
	};

	/**
	 * Creates a copy a %FieldAccessor object.
	 * @return The new copy
	 */
	virtual BasicFieldAccessor *clone() const
	{
		return new FieldAccessor<ClassType, ValueType>(*this);
	};

private:

	/** Copy operator (not implemented!) */
	const FieldAccessor<ClassType, ValueType> &operator=(const FieldAccessor<ClassType, ValueType> &);

	/** Pointer to a method that sets the field value */
	SetMethod setMethod_;

	/** Pointer to a method that returns the field value */
	GetMethod getMethod_;
};


} // namespace InstantIO


#endif // __INSTANTIO_FIELDACCESSOR_H
