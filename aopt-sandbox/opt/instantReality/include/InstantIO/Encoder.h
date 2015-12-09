#ifndef __INSTANTIO_ENCODER_H
#define __INSTANTIO_ENCODER_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Encoder.h                                                       //
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
#include "BasicEncoder.h"
#include "InSlot.h"
#include <string>
#ifdef __sgi
# include <assert.h>
#else
# include <cassert>
#endif


namespace InstantIO
{


class BasicInSlot;


/**
 * @class Encoder Encoder.h InstantIO/Encoder.h
 *
 * Abstract superclass of all classes that encode objects.
 * The {@link NetworkNode NetworkNode} class uses
 * encoders to encode values before they are sent to other
 * InstantIO components on the network. Before any data type can
 * be transferred via the network, an %Encoder has to be written
 * that transforms the data values to a system and hardware
 * independent byte stream. For some frequently used data types,
 * encoders already exist in the InstantIO package, but when you need
 * to transfer your own data types, you have to implement an
 * %Encoder.
 * <p>
 * For example, let's say you want to transfer boolean values
 * via the network (an %Encoder for boolean of course already
 * exists in the InstantIO package, but for the sake of demonstrating
 * the procedure whe nevertheless demonstrate how to create your
 * own). First, you have to create a class that inherits from
 * the %Encoder class and implements the abstract {@link #encode
 * encode} method:
 * <code><pre>
 * class MyIntEncoder: public Encoder<bool>
 * {
 *   virtual void encode(ostream &os, const bool &value)
 *   {
 *     os.put(value == true ? 1 : 0);
 *   }
 * }
 * </pre></code>
 * Then, you have to create a corresponding {@link Decoder
 * Decoder} class (see the documentation of this class for
 * more information about how to do that). Finally, you have
 * to create an instance of the {@link Codec Codec} class so
 * that the NetworkNode can actually access your %Encoder
 * (see documentation of the Codec class).
 *
 * @author Patrick D&auml;hne
 * @see NetworkNode NetworkNode
 * @see Decoder Decoder
 * @see Codec Codec
 */
template <class T>
class Encoder: public BasicEncoder
{
public:

	/** Destroys the %Encoder object */
	virtual ~Encoder()
	{ delete inSlot_; };

	/**
	 * Initializes the %Encoder object.
	 * @param label The label of the InSlot
	 * @param description The description of the InSlot
	 */
	virtual void initialize(const std::string &label, const std::string &description)
	{
		BasicEncoder::initialize(label, description);
		inSlot_ = new InSlot<T>(description);
	};

	/**
	 * Returns the InSlot we read the values that have to be
	 * encoded from.
	 * @return The InSlot
	 */
	virtual BasicInSlot *getInSlot() const
	{
		assert(inSlot_ != 0);
		return inSlot_;
	};

	/**
	 * Encodes objects.
	 * This method encodes objects into their binary representation
	 * and writes this binary representation into an output stream.
	 * This method needs to be overwritten by your own implementations.
	 * @param os The output stream you write the system and hardware
	 * independent byte representation to.
	 * @param value The object you have to encode.
	 */
	virtual void encode(std::ostream &os, const T &value) = 0;

protected:

	/** Creates a new %Encoder object */
	inline Encoder(): BasicEncoder(), inSlot_(0) {}

	/**
	 * Reads a value from the InSlot and encodes it
	 * into an output stream.
	 * @param os The stream we put the result into.
	 * @return The time stamp of the data value
	 */
	virtual TimeStamp popAndEncode(std::ostream &os)
	{
		assert(inSlot_ != 0);
		Data<T> data = inSlot_->pop();
		encode(os, data.getValue());
		return data.getTimeStamp();
	};

private:

	/** Copy constructor (not implemented!) */
	Encoder(const Encoder<T> &);

	/** Copy operator (not implemented!) */
	const Encoder<T> &operator=(const Encoder<T> &);

	/** The InSlot we read the values that have to be encoded from */
	InSlot<T> *inSlot_;
};


} // namespace InstantIO


#endif // __INSTANTIO_ENCODER_H
