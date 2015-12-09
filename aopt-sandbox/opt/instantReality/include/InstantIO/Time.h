#ifndef __INSTANTIO_TIME_H
#define __INSTANTIO_TIME_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Time.h                                                          //
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
#include "TypeName.h"


namespace InstantIO
{


/**
 * @class Time Time.h InstantIO/Time.h
 *
 * Helper class for exchanging time values.
 * Time is a basic helper class for exchanging time values between
 * different software components. It is not meant to be used
 * directly by software components for representation of time values.
 * Instead, software components should use their own, appropriate classes
 * for handling time values. Only when sending time values to an OutSlot,
 * or when receiving time values from an InSlot, the internal
 * representation of time values should be converted to Time's. This
 * ensures the interoperability between different software components
 * that use different internal representations for time values.
 *
 * @author Patrick D&auml;hne
 */

class INSTANTIO_DLLMAPPING Time
{
public:

	/**
	 * Constructor that initializes the Time object
	 * with the current time.
	 */
	inline Time(): time_(getCurrentTime()) {}

	/**
	 * Constructor that initializes the Time object
	 * with a given time value.
	 * @param time The time value used to initialize the
	 * Time object.
	 */
	inline Time(double time): time_(time) {}

	/**
	 * Constructor that initializes the Time object
	 * with the time value of another Time object.
	 * @param time The other Time object.
	 */
	inline Time(const Time &time): time_(time.time_) {}

	/**
	 * Copy operator. Sets the time value of this object to
	 * the time value of another Time object.
	 * @param time The other Time object.
	 */
	inline const Time &operator=(const Time &time)
	{
		time_ = time.time_;
		return time;
	}

	/**
	 * Sets the time value.
	 * @param time The new time value.
	 */
	inline void set(double time)
	{
		time_ = time;
	}

	/**
	 * Sets the time value.
	 * @param time The Time object that contains the
	 * new time value.
	 */
	inline void set(Time time)
	{
		time_ = time.time_;
	}

	/**
	 * Returns the time value.
	 * @return The time value.
	 */
	inline double get() const
	{
		return time_;
	}

	/**
	 * Equality operator.
	 * @param t1 The first Time object.
	 * @param t2 The second Time object.
	 * @return true when the Time objects are equal, false otherwise.
	 */
	friend bool operator==(const Time &t1, const Time &t2);

	/**
	 * Inequality operator.
	 * @param t1 The first Time object.
	 * @param t2 The second Time object.
	 * @return true when the Time objects are not equal, false otherwise.
	 */
	friend inline bool operator!=(const Time &t1, const Time &t2)
	{
		return !(t1 == t2);
	}

	/**
	 * Returns the current time.
	 * @return The current time.
	 */
	static double getCurrentTime();

	/**
	 * Output operator.
	 * @param os The output stream.
	 * @param t The Time object.
	 * @return The output stream.
	 */
	friend INSTANTIO_DLLMAPPING std::ostream &operator<<(std::ostream &os, const Time &t);

private:

	/**
	 * The time value.
	 */
	double time_;
};


/**
 * Helper class that transforms the Time data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<Time>
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


#endif // __INSTANTIO_TIME_H
