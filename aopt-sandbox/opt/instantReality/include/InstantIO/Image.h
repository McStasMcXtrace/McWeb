#ifndef __INSTANTIO_IMAGE_H
#define __INSTANTIO_IMAGE_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Image.h                                                         //
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
#include <string>


namespace InstantIO
{


/**
 * @class Image Image.h InstantIO/Image.h
 *
 * %Image class
 */
class INSTANTIO_DLLMAPPING Image
{
public:

	/**
	 * Defines the different kinds of image formats the
	 * %Image class can handle
	 */
	typedef enum
	{
		FMT_NONE,

		// RGB formats
		FMT_BGR232,
		FMT_BGR555,
		FMT_BGR565,
		FMT_BGR24,
		FMT_RGB24,
		FMT_BGR32,
		FMT_RGB32,

		// other formats
		FMT_SHORT16,
		FMT_FLOAT16,
		FMT_FLOAT32,

		// YUV Formats
		FMT_Y444, // this *must* be the first YUV format!
		FMT_IYU2,
		FMT_I420,
		FMT_YVU9,
		FMT_YUV9,
		FMT_Y800,
		FMT_Y1600,
		FMT_Y422,
		FMT_Y411,
		FMT_Y41P,
		FMT_YV12,
		FMT_YUY2,
		FMT_YUV422P,
		FMT_YUV411P
	}
	Format;

	/// Constructor
	/**
	 * Creates a new %Image object.
	 */
	Image();

	/** Copy constructor */
	Image(const Image &image);

	/** Copy operator */
	const Image &operator=(const Image &image);

	/// Destructor
	/**
	 * Destroys the %Image object.
	 */
	~Image();

	/// Sets the parameters of the image
	/**
	 * This method sets the parameters of the image.
	 * @param width The width of the image in pixels
	 * @param height The height of the image in pixels
	 * @param format The format of the image data
	 */
	void setParameters(unsigned int width, unsigned int height, Format format);

	/// Returns the width
	/**
	 * Returns the width of the image data in pixels.
	 * @return The width
	 */
	inline unsigned int getWidth() const { return width_; }

	/// Returns the height
	/**
	 * Returns the height of the image data in pixels.
	 * @return The height
	 */
	inline unsigned int getHeight() const { return height_; }

	/// Returns the format
	/**
	 * Returns the format of the image data.
	 * @return The format
	 */
	inline Format getFormat() const { return format_; }

	/// Returns the buffer
	/**
	 * Returns a pointer to the buffer that holds the
	 * image data.
	 * @return The buffer
	 */
	inline char *getBuffer() const { return buffer_; }

	/// Returns the size of the data in the buffer
	/**
	 * Returns the size of the data in the buffer in bytes.
	 * @return The size
	 */
	inline unsigned long getSize() const { return size_; }

	/**
	 * Converts a Format enumeration value to a
	 * human-readable string.
	 * @param format The Format enumeration value
	 * @return The human-readable string
	 */
	static const char *format2String(Format format);

	/**
	 * Converts a human-readable string to a Format
	 * enumeration value.
	 * @param str The human-readable string
	 * @return The Format enumeration value
	 */
	static Format string2Format(const std::string &str);

	/**
	 * Converts the %Image object to another format.
	 * @param format The new image format
	 * @param image An %Image object that gets filled
	 * with the converted image data.
	 */
	void convertFormat(Format format, Image &image) const;

	/**
	 * Equality operator.
	 * @param i1 The first Image object.
	 * @param i2 The second Image object.
	 * @return true when the Image objects are equal, false otherwise.
	 */
	friend bool operator==(const Image &i1, const Image &i2);

	/**
	 * Inequality operator.
	 * @param i1 The first Image object.
	 * @param i2 The second Image object.
	 * @return true when the Image objects are not equal, false otherwise.
	 */
	friend inline bool operator!=(const Image &i1, const Image &i2)
	{
		return !(i1 == i2);
	}

private:

	/// The buffer that holds the image data
	char *buffer_;

	/// The size of the buffer
	unsigned long bufferSize_;

	/// The size of the data in the buffer
	unsigned long size_;

	/// The width (in pixels) of the image data
	unsigned int width_;

	/// The height (in pixels) of the image data
	unsigned int height_;

	/// The format of the image data
	Format format_;

	void convert_BGR232_to_RGB24(Image &image) const;

	void convert_BGR555_to_RGB24(Image &image) const;

	void convert_BGR565_to_RGB24(Image &image) const;

	void convert_BGR32_to_RGB24(Image &image) const;

        void convert_BGR24_to_RGB24(Image &image) const;

        void convert_RGB24_to_Y800(Image &image) const;

        void convert_BGR24_to_Y800(Image &image) const;

	void convert_IYU2_to_Y444(Image &image) const;

	void convert_Y422_to_Y444(Image &image) const;

	void convert_Y422_to_Y800(Image &image) const;

	void convert_Y444_to_Y800(Image &image) const;

	void convert_Y411_to_Y444(Image &image) const;

	void convert_Y411_to_Y800(Image &image) const;

	void convert_Y41P_to_Y444(Image &image) const;

	void convert_Y800_to_Y444(Image &image) const;

	void convert_Y1600_to_Y444(Image &image) const;

	void convert_I420_to_Y444(Image &image) const;

	void convert_I420_to_Y800(Image &image) const;

	void convert_YVU9_to_Y444(Image &image) const;

	void convert_YV12_to_Y444(Image &image) const;

	void convert_YUY2_to_Y444(Image &image) const;

};


template <class T> class SmartPtr;


typedef SmartPtr<Image> ImagePtr;


/**
 * Helper class that transforms the ImagePtr data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<ImagePtr>
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


#endif // __INSTANTIO_IMAGE_H
