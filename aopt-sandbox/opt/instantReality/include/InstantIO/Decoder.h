#ifndef __INSTANTIO_DECODER_H
#define __INSTANTIO_DECODER_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Decoder.h                                                       //
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
#include "BasicDecoder.h"
#include "OutSlot.h"
#include <string>
#ifdef __sgi
# include <assert.h>
#else
# include <cassert>
#endif


namespace InstantIO
{


class BasicOutSlot;


/**
 * @class Decoder Decoder.h InstantIO/Decoder.h
 *
 * Abstract superclass of all classes that decode objects.
 * The {@link NetworkNode NetworkNode} class uses
 * decoders to decode values received from other InstantIO components
 * on the network. Before any data type can be transferred via
 * the network, a %Decoder has to be written that transforms a
 * system and hardware independent byte stream to data values.
 * For some frequently used data types, decoders already exist
 * in the InstantIO package, but when you need to transfer your own
 * data types, you have to implement a %Decoder.
 * <p>
 * For example, let's say you want to transfer boolean values via
 * the network (a Decoder for boolean of course already exists
 * in the InstantIO package, but for the sake of demonstrating the
 * procedure whe nevertheless demonstrate how to create your own).
 * First, you have to create a class that inherits from the
 * Decoder class and implements the abstract {@link #decode
 * decode} method:
 * <code><pre>
 * class MyIntDecoder: public Decoder<bool>
 * {
 *   virtual bool decode(istream &is)
 *   {
 *     int c = is.get();
 *     return c != 0;
 *   }
 * }
 * </pre></code>
 * Then, you have to create a corresponding {@link Encoder
 * Encoder} class (see the documentation of this class for
 * more information about how to do that). Finally, you have
 * to create an instance of the {@link Codec Codec} class so
 * that the NetworkNode can actually access your Decoder
 * (see documentation of the Codec class).
 *
 * @author Patrick D&auml;hne
 * @see NetworkNode NetworkNode
 * @see Encoder Encoder
 * @see Codec Codec
 */
template <class T>
class Decoder: public BasicDecoder
{
public:

	/** Destroys the %Decoder object */
	virtual ~Decoder()
	{ delete outSlot_; };

	/**
	 * Returns the OutSlot we write the decoded values to.
	 * @return The OutSlot
	 */
	virtual BasicOutSlot *getOutSlot() const
	{
		assert(outSlot_ != 0);
		return outSlot_;
	};

	/**
	 * Initializes the %Decoder object.
	 * @param label The label of the OutSlot
	 * @param description The description of the OutSlot
	 */
	virtual void initialize(const std::string &label, const std::string &description)
	{
		BasicDecoder::initialize(label, description);
		outSlot_ = new OutSlot<T>(description);
	};

	/**
	 * Decodes objects.
	 * This method reads the binary representation of objects from
	 * an input stream and decodes objects from this binary representation.
	 * This method needs to be overwritten by your own implementations.
	 * @param is The input stream you read the system and hardware independent
	 * byte stream from.
	 * @return The decoded object you create from the byte stream
	 */
	virtual T decode(std::istream &is) = 0;

protected:

	/** Creates a new Decoder object */
	inline Decoder(): BasicDecoder(), outSlot_(0) {}

	/**
	 * Gets the binary representation of an object from a stream,
	 * decodes the object, and writes it to the OutSlot.
	 * @param is The stream we get the binary representation from.
	 * @param timeStamp The timestamp of the data value.
	 */
	virtual void decodeAndPush(std::istream &is, TimeStamp timeStamp)
	{
		assert(outSlot_ != 0);
		Data<T> data;
		data.setValue(decode(is));
		data.setTimeStamp(timeStamp);
		outSlot_->push(data);
	};

private:

	/** Copy constructor (not implemented!) */
	Decoder(const Decoder<T> &);

	/** Copy operator (not implemented!) */
	const Decoder<T> &operator=(const Decoder<T> &);

	/** The OutSlot we write the values that have to be decoded to */
	OutSlot<T> *outSlot_;
};


} // namespace InstantIO


#endif // __INSTANTIO_DECODER_H
