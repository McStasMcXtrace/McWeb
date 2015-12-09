#ifndef __INSTANTIO_BASICDECODER_H
#define __INSTANTIO_BASICDECODER_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: BasicDecoder.h                                                  //
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


class BasicOutSlot;
class SetStateEvent;


/**
 * @class BasicDecoder BasicDecoder.h InstantIO/BasicDecoder.h
 *
 * Abstract base class of all Decoder templates.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING BasicDecoder
{
public:

	/** Destroys the %BasicDecoder object */
	virtual ~BasicDecoder();

	/**
	 * Initializes the Decoder object.
	 * @param label The label of the OutSlot
	 * @param description The description of the OutSlot
	 */
	virtual void initialize(const std::string &label, const std::string &description);

	/**
	 * Returns the label of the OutSlot we write the decoded
	 * values to.
	 * @return The label of the OutSlot
	 */
	inline const std::string getLabel() const { return label_; }

	/**
	 * Returns the OutSlot we write the decoded values to.
	 * @return The OutSlot
	 */
	virtual BasicOutSlot *getOutSlot() const = 0;

	/**
	 * Gets the binary representation of an object from the event,
	 * decodes the object, and writes it to the OutSlot.
	 * @param event The event we get the binary representation from.
	 */
	virtual void decodeAndPush(const SetStateEvent &event);

protected:

	/** Creates a new %BasicDecoder object */
	inline BasicDecoder(): label_() {}

	/**
	 * Gets the binary representation of an object from a stream,
	 * decodes the object, and writes it to the OutSlot.
	 * @param is The stream we get the binary representation from.
	 * @param timeStamp The timestamp of the data value.
	 */
	virtual void decodeAndPush(std::istream &is, TimeStamp timeStamp) = 0;

private:

	/** Copy constructor (not implemented!) */
	BasicDecoder(const BasicDecoder &);

	/** Copy operator (not implemented!) */
	const BasicDecoder &operator=(const BasicDecoder &);

	/** The label of the OutSlot */
	std::string label_;
};


} // namespace InstantIO


#endif // __INSTANTIO_BASICDECODER_H
