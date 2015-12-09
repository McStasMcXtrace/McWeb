#ifndef __INSTANTIO_SAMPLE_H
#define __INSTANTIO_SAMPLE_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Sample.h                                                        //
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
 * @class Sample Sample.h InstantIO/Sample.h
 *
 * %Sample class
 */
class INSTANTIO_DLLMAPPING Sample
{
public:

	/** Creates a new %Sample object */
	Sample();

	/** Destroys the %Sample object */
	~Sample();

	/**
	 * Sets the parameters of the sample
	 * @param samplerate The samplerate
	 * @param bits The number of bits
	 * @param channels The number of channels
	 * @param numFrames The number of frames
	 */
	void setParameters(unsigned int samplerate, unsigned char bits, unsigned char channels, unsigned long numFrames);

	/**
	 * Returns the samplerate.
	 * @return The samplerate
	 */
	inline unsigned int getSamplerate() const { return samplerate_; }

	/**
	 * Returns the number of bits.
	 * @return The number of bits
	 */
	inline unsigned char getBits() const { return bits_; }

	/**
	 * Returns the number of channels.
	 * @return The number of channels
	 */
	inline unsigned char getChannels() const { return channels_; }

	/**
	 * Returns the number of frames.
	 * @return The number of frames.
	 */
	inline unsigned long getNumFrames() const { return numFrames_; }

	/**
	 * Returns the number of bytes per frame.
	 * @return The number of bytes per frame.
	 */
	inline unsigned int getBytesPerFrame() const
	{ return ((bits_ + 7) >> 3) * channels_; }

	/**
	 * Returns a pointer to the buffer that holds the
	 * sample data.
	 * @return The buffer
	 */
	inline const char *getBuffer() const { return buffer_; }

	/**
	 * Returns a pointer to the buffer that holds the
	 * sample data.
	 * @return The buffer
	 */
	inline char *getBuffer() { return buffer_; }

	/**
	 * Returns the size of the buffer that holds the sample
	 * data in bytes.
	 * @return The size
	 */
	inline unsigned long getBufferSize() const
	{ return getBytesPerFrame() * numFrames_; }

private:

	/** Copy constructor (not implemented!) */
	Sample(const Sample &);

	/** Copy operator (not implemented!) */
	const Sample &operator=(const Sample &);

	/** The buffer that holds the sample data */
	char *buffer_;

	/** The size of the buffer */
	unsigned long bufferSize_;

	/** The samplerate */
	unsigned int samplerate_;

	/** The number of bits */
	unsigned char bits_;

	/** The number of channels */
	unsigned char channels_;

	/** The number of frames in the buffer */
	unsigned long numFrames_;
};


template <class T> class SmartPtr;


/** Defines a smartpointer to a Sample object */
typedef SmartPtr<Sample> SamplePtr;


/**
 * Helper class that transforms the SamplePtr data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<SamplePtr>
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


#endif // __INSTANTIO_SAMPLE_H
