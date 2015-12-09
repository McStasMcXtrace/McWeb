#ifndef __INSTANTIO_BASICDATA_H
#define __INSTANTIO_BASICDATA_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: BasicData.h                                                     //
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
#include "TimeStamp.h"


namespace InstantIO
{


/**
 * @class BasicData BasicData.h InstantIO/BasicData.h
 *
 * Abstract ancestor of all Data objects. Applications do not use
 * this class directly, instead they use its descendant (Data).
 * @see Data
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING BasicData
{
public:

	/**
	 * Returns the timestamp.
	 * @return Returns the timestamp of the value contained in
	 * the data object. This is a <code>long</code> value that
	 * specifies the number of milliseconds since midnight
	 * January 1., 1970 UTC.
	 */
	inline TimeStamp getTimeStamp() const { return timeStamp_; }

	/**
	 * Sets the timestamp to the current system time.
	 */
	void setTimeStamp();

	/**
	 * Sets the timestamp.
	 * @param timeStamp The new timestamp of the data value.
	 * This is a <code>long</code> value that specifies the
	 * number of milliseconds since midnight January 1.,
	 * 1970 UTC.
	 */
	inline void setTimeStamp(TimeStamp timeStamp)
	{ timeStamp_ = timeStamp; }

protected:

	/**
	 * Creates a new %BasicData object.
	 * @param timeStamp The timestamp of the new data value.
	 * This is a <code>long</code> value that specifies the
	 * number of milliseconds since midnight January 1.,
	 * 1970 UTC.
	 */
	inline explicit BasicData(TimeStamp timeStamp = 0): timeStamp_(timeStamp) {}

	/** The timestamp */
	volatile TimeStamp timeStamp_;
};


} // namespace InstantIO


#endif // __INSTANTIO_BASICDATA_H
