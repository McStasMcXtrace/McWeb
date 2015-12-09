#ifndef __INSTANTIO_DATA_H
#define __INSTANTIO_DATA_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Data.h                                                          //
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
#include "BasicData.h"


namespace InstantIO
{


/**
 * @class Data Data.h InstantIO/Data.h
 *
 * Helper class that combines a data value with a timestamp.	
 * This class is used when writing values into {@link InSlot
 * InSlots} or when reading values from {@link OutSlot OutSlots}
 * to transfer timestamps between different software components.
 * The timestamp is a <code>long</code> value that specifies the
 * number of milliseconds since midnight January 1., 1970 UTC.
 * <p>
 * The following code snippet demonstrates how to use the Data
 * class when pushing values into OutSlots:
 * <p>
 * <code><pre>
 * ...
 * bool value = ...;
 * TimeStamp timeStamp = ...;
 * Data<bool> data(value, timeStamp);
 * outSlot.push(data);
 * ...
 * </pre></code>
 * <p>
 * The following code snippet demonstrates how to use the Data
 * class when receiving values from InSlots:
 * <p>
 * <code><pre>
 * ...
 * Data<bool> data = inSlot.pop();
 * bool value = data.getValue();
 * TimeStamp timeStamp = data.getTimeStamp();
 * ...
 * </pre></code>
 *
 * @author Patrick D&auml;hne
 * @see OutSlot OutSlot
 * @see InSlot InSlot
 */
template <class T> class Data: public BasicData
{
public:

	/**
	 * Creates a new %Data object that does not contain any
	 * data and whose timestamp is 0.
	 */
	inline Data(): BasicData(), value_() {}

	/**
	 * Creates a new %Data object that contains a given value
	 * and whose timestamp is set to the current system time.
	 * @param value The value that is put into the Data object.
	 */
	inline Data(const T &value): BasicData(), value_(value)
	{ setTimeStamp(); }

	/**
	 * Creates a new %Data object that is an exact copy of another
	 * %Data object.
	 * @param other The other %Data object we take the value and the
	 * timestamp from.
	 */
	inline Data(const Data &other): BasicData(other.timeStamp_), value_(other.value_)
	{}

	/**
	 * Assignment operator that copies value and timestamp from
	 * another %Data object.
	 * @param other The other Data object
	 * @return Itself
	 */
	inline const Data<T> &operator=(const Data<T> &other)
	{
		if (this != &other)
		{
			value_ = other.value_;
			timeStamp_ = other.timeStamp_;
		}
		return *this;
	}

	/**
	 * Assignment operator that sets the value of the %Data object.
	 * Sets the timestamp to the current system time.
	 * @param value The value
	 * @return The value
	 */
	inline const T &operator=(const T &value)
	{
		value_ = value;
		setTimeStamp();
		return value;
	}

	/**
	 * Typecast operator that transforms the %Data object
	 * to the value contained in the %Data object.
	 * @return The data value contained in the %Data object.
	 */
	inline operator const T&() const
	{ return value_; }

	/**
	 * Typecast operator that transforms the %Data object
	 * to the value contained in the %Data object.
	 * @return The data value contained in the %Data object.
	 */
	inline operator T&()
	{ return value_; }

	/**
	 * Returns the data value.
	 * @return The data value contained in the %Data object.
	 */
	inline const T &getValue() const
	{ return value_; }

	/**
	 * Returns the data value.
	 * @return The data value contained in the %Data object.
	 */
	inline T &getValue()
	{ return value_; }

	/**
	 * Sets the data value.
	 * @param value The value that is put into the %Data object.
	 */
	inline void setValue(const T &value)
	{ value_ = value; }

private:

	/** The data value */
	T value_;
};


} // namespace InstantIO


#endif // __INSTANTIO_DATA_H
