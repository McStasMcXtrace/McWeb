#ifndef __INSTANTIO_BASICENCODER_H
#define __INSTANTIO_BASICENCODER_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: BasicEncoder.h                                                  //
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
#include <iosfwd>
#include <string>


namespace InstantIO
{


class BasicInSlot;
class SetStateEvent;


/**
 * @class BasicEncoder BasicEncoder.h InstantIO/BasicEncoder.h
 *
 * Abstract base class of all Encoder templates.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING BasicEncoder
{
public:

	/** Destroys the %BasicEncoder object */
	virtual ~BasicEncoder();

	/**
	 * Initializes the encoder object.
	 * @param label The label of the InSlot
	 * @param description The description of the InSlot
	 */
	virtual void initialize(const std::string &label, const std::string &description);

	/**
	 * Returns the label of the InSlot we read the values we
	 * have to encode from.
	 * @return The label of the InSlot
	 */
	inline const std::string &getLabel() const { return label_; }

	/**
	 * Returns the InSlot we read the values that have to be
	 * encoded from.
	 * @return The InSlot
	 */
	virtual BasicInSlot *getInSlot() const = 0;

	/**
	 * Reads a value from the InSlot, encodes it, and puts the
	 * result into an event.
	 * @param event The event we put the result into.
	 */
	virtual void popAndEncode(SetStateEvent &event);

protected:

	/** Creates a new BasicEncoder object */
	inline BasicEncoder(): label_() {}

	/**
	 * Reads a value from the InSlot and encodes it
	 * into an output stream.
	 * @param os The stream we put the result into.
	 * @return The time stamp of the data value
	 */
	virtual TimeStamp popAndEncode(std::ostream &os) = 0;

private:

	/** Copy constructor (not implemented!) */
	BasicEncoder(const BasicEncoder &);

	/** Copy operator (not implemented!) */
	const BasicEncoder &operator=(const BasicEncoder &);

	/** The label of the InSlot */
	std::string label_;
};


} // namespace InstantIO


#endif // __INSTANTIO_BASICENCODER_H
