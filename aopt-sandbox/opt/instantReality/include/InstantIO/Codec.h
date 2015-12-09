#ifndef __INSTANTIO_CODEC_H
#define __INSTANTIO_CODEC_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Codec.h                                                         //
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
#include "BasicCodec.h"
#include "TypeName.h"
#include <string>


namespace InstantIO
{


class BasicOutSlot;
class BasicInSlot;
template <class T> class OutSlot;
template <class T> class InSlot;


/**
 * @class Codec Codec.h InstantIO/Codec.h
 *
 * Contains meta information about Encoders and Decoders.
 * {@link Encoder Encoders} and {@link Decoder Decoders} are
 * used by the {@link NetworkNode NetworkNode} to transfer
 * data values between InstantIO components on the network. Before any
 * data type can be transferred via the network, Encoders and
 * Decoders have to be written that transform data values to
 * system and hardware independent byte streams and vice versa.
 * For some frequently used data types, encoders and decoders
 * already exist in the InstantIO package, but when you need to
 * transfer your own data types, you have to implement encoders
 * and decoders, and you have to create an instance of the Codec
 * class that contains meta information about the encoders and
 * decoders. This meta information consists of:
 * <ul>
 * <li> A label that is used to identify the codec.
 * <li> The data type handled by this codec.
 * <li> The Encoder class used to encode the data values.
 * <li> The Decoder class used to decode the data values.
 * <li> The author who wrote the Encoder and Decoder classes.
 * This information is not used by the InstantIO system in any way,
 * it simply serves documentation purposes.
 * </ul>
 * <p>
 * For example, let's say you want to transfer <code>Integer
 * </code> values via the network (a Codec for <code>
 * Integer</code> of course already exists in the InstantIO package,
 * but for the sake of  demonstrating the procedure whe
 * nevertheless demonstrate how to create your own). First, you
 * have to create classes that inherit from the Encoder and
 * Decoder classes and implement the encoding and decoding of
 * data values (see the documentation for these classes to get
 * more information about how to do that). Then, you have to
 * create an instance of the Codec class that specifies the
 * meta information:
 * <code><pre>
 * static Codec myIntCodec = new Codec(
 *   "Integer",
 *   Integer.class,
 *   MyIntEncoder.class,
 *   MyIntDecoder.class,
 *   "Patrick D&amp;auml;hne");
 * </pre></code>
 *
 * @author Patrick D&auml;hne
 * @see NetworkNode NetworkNode
 * @see Encoder Encoder
 * @see Decoder Decoder
 */
template <class T>
class Codec: public BasicCodec
{
public:

	/**
	 * Creates a new %Codec object.
	 * @param label The label of the %Codec. This label is used
	 * to identify the %Codec. Case does not matter.
	 * @param createEncoderMethod The method used to create new Encoders.
	 * @param createDecoderMethod The method used to create new Decoders.
	 * @param author The name of the person who wrote the codec. This
	 * information is not used by the InstantIO system in any way, it
	 * simply serves documentation purposes.
	 */
	inline Codec(const std::string &label, CreateEncoderMethod createEncoderMethod, CreateDecoderMethod createDecoderMethod, const std::string &author = std::string())
	: BasicCodec(label, createEncoderMethod, createDecoderMethod, author)
	{}

	/** Destroys the %Codec object */
	virtual ~Codec() {};

	/**
	 * Returns a human-readable string that describes the type
	 * that can be encoded and decoded by this %Codec.
	 * @return A string that describes the type that can be
	 * encoded or decoded by this %Codec.
	 */
	virtual const char *getTypeName() const
	{ return TypeName<T>::getName(); };

	/**
	 * Checks if a the %Codec has the same type as a given OutSlot.
	 * @param outSlot The OutSlot.
	 * @return true when the %Codec and the OutSlot have the same
	 * type, false otherwise.
	 */
	virtual bool sameType(const BasicOutSlot &outSlot) const
	{ return dynamic_cast<const OutSlot<T>*>(&outSlot) != 0; };

	/**
	 * Checks if a the %Codec has the same type as a given InSlot.
	 * @param inSlot The InSlot.
	 * @return true when the %Codec and the InSlot have the same
	 * type, false otherwise.
	 */
	virtual bool sameType(const BasicInSlot &inSlot) const
	{ return dynamic_cast<const InSlot<T>*>(&inSlot) != 0; };

private:

	/** Copy constructor (not implemented!) */
	Codec(const Codec<T> &);

	/** Copy operator (not implemented!) */
	const Codec<T> &operator=(const Codec<T> &);
};


} // namespace InstantIO


#endif // __INSTANTIO_CODEC_H
