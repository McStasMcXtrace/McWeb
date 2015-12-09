#ifndef __INSTANTIO_BASICCODEC_H
#define __INSTANTIO_BASICCODEC_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: BasicCodec.h                                                    //
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
#include "TypeName.h"
#include <map>
#include <string>
#ifdef __sgi
# include <assert.h>
#else
# include <cassert>
#endif


namespace InstantIO
{


class BasicDecoder;
class BasicOutSlot;
class BasicInSlot;


/**
 * @class BasicCodec BasicCodec.h InstantIO/BasicCodec.h
 *
 * Abstract base class of all Codec templates.
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING BasicCodec
{
public:

	/** Defines a method that creates a new Encoder */
	typedef BasicEncoder *(*CreateEncoderMethod)();

	/** Defines a method that creates a new Decoder */
	typedef BasicDecoder *(*CreateDecoderMethod)();

	/** Defines the map of all codecs */
	typedef std::map<std::string, BasicCodec*> CodecMap;

	/** Destroys the %BasicCodec object */
	virtual ~BasicCodec();

	/**
	 * Returns the label of the %Codec. This label is used
	 * to identify the %Codec.
	 * @return The label of the %Codec.
	 */
	inline const std::string &getLabel() const { return label_; }

	/**
	 * Returns the name of the person who wrote the %Codec.
	 * This information is not used by the InstantIO system in any
	 * way, it simply serves documentation purposes.
	 * @return The name.
	 */
	inline const std::string &getAuthor() const { return author_; }

	/**
	 * Returns a human-readable string that describes the type
	 * that can be encoded and decoded by this %Codec.
	 * @return A string that describes the type that can be
	 * encoded or decoded by this %Codec.
	 */
	virtual const char *getTypeName() const = 0;

	/**
	 * Creates a new Encoder object.
	 * @return The new Encoder object.
	 */
	inline BasicEncoder *createEncoder() const
	{
		assert(createEncoderMethod_ != 0);
		return createEncoderMethod_();
	}

	/**
	 * Creates a new Decoder object.
	 * @return The new Decoder object.
	 */
	inline BasicDecoder *createDecoder() const
	{
		assert(createDecoderMethod_ != 0);
		return createDecoderMethod_();
	}

	/**
	 * Returns the %Codec that matches a given label.
	 * @param label The label of the %Codec. Case does not
	 * matter.
	 * @return The %Codec that matches the label or null when
	 * no such %Codec exists.
	 */
	static BasicCodec *findCodec(const std::string &label);

	/**
	 * Returns the %Codec whose type matches the type of a given
	 * OutSlot.
	 * @param outSlot The OutSlot.
	 * @return The %Codec whose type matches the type of the
	 * OutSlot or null when no such %Codec exists.
	 */
	static BasicCodec *findCodec(const BasicOutSlot &outSlot);

	/**
	 * Returns the %Codec whose type matches the type of a given
	 * InSlot.
	 * @param inSlot The InSlot.
	 * @return The %Codec whose type matches the type of the
	 * InSlot or null when no such %Codec exists.
	 */
	static BasicCodec *findCodec(const BasicInSlot &inSlot);

	/**
	 * Checks if a the Codec has the same type as a given OutSlot.
	 * @param outSlot The OutSlot.
	 * @return true when the Codec and the OutSlot have the same
	 * type, false otherwise.
	 */
	virtual bool sameType(const BasicOutSlot &outSlot) const = 0;

	/**
	 * Checks if a the Codec has the same type as a given InSlot.
	 * @param inSlot The InSlot.
	 * @return true when the Codec and the InSlot have the same
	 * type, false otherwise.
	 */
	virtual bool sameType(const BasicInSlot &inSlot) const = 0;

	/**
	 * Returns the map of all %Codecs currently available.
	 * @return The map of all %Codecs
	 */
	static inline CodecMap *getCodecs()
	{ return codecMap_; }

protected:

	/**
	 * Creates a new %BasicCodec object.
	 * @param label The label of the Codec. This label is used
	 * to identify the Codec. Case does not matter.
	 * @param createEncoderMethod The method used to create new Encoders.
	 * @param createDecoderMethod The method used to create new Decoders.
	 * @param author The name of the person who wrote the codec. This
	 * information is not used by the InstantIO system in any way, it
	 * simply serves documentation purposes.
	 */
	BasicCodec(const std::string &label, CreateEncoderMethod createEncoderMethod, CreateDecoderMethod createDecoderMethod, const std::string &author = std::string());

private:
	
	/** Copy constructor (not implemented!) */
	BasicCodec(const BasicCodec &);

	/** Copy operator (not implemented!) */
	const BasicCodec &operator=(const BasicCodec &);

	/** Map of all Codecs known to the system */
	static CodecMap *codecMap_;

	/** The label of the %Codec */
	std::string label_;

	/** Method used to create new Encoders */
	CreateEncoderMethod createEncoderMethod_;

	/** Method used to create new Decoders */
	CreateDecoderMethod createDecoderMethod_;

	/** The name of the person who wrote the %Codec */
	std::string author_;
};


} // namespace InstantIO


#endif // __INSTANTIO_BASICCODEC_H
