#ifndef __INSTANTIO_COLORRGBA_H
#define __INSTANTIO_COLORRGBA_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: ColorRGBA.h                                                     //
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
#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4351)
#endif


namespace InstantIO
{


/**
 * @class ColorRGBA ColorRGBA.h InstantIO/ColorRGBA.h
 *
 * Helper class for exchanging RGBA colors of three float components.
 * %ColorRGBA is a basic helper class for exchanging colors of three
 * float components between different software components. It does
 * not contain any means for color arithmetics, only methods for
 * setting and getting the components. It is not meant to be used
 * directly by software components for color representation. Instead,
 * software components should use their own, appropriate classes for
 * handling colors. Only when sending colors to an OutSlot, or when
 * receiving colors from an InSlot, the internal representation of
 * colors should be converted to ColorRGBA's. This ensures the
 * interoperability between different software components that use
 * different internal representations for colors.
 *
 * @author Helmut Seibert
 */
class INSTANTIO_DLLMAPPING ColorRGBA
{
public:

	/**
	 * Constructor that initializes all components of the color
	 * with 0.
	 */
	inline ColorRGBA(): color_()
	{ color_[0] = color_[1] = color_[2] = 0.f; color_[3] = 1.f; }

	/**
	 * Constructor that initializes the components of the color
	 * with the given float values.
	 * @param r Value of the r component of the new color.
	 * @param g Value of the g component of the new color.
	 * @param b Value of the b component of the new color.
	 * @param a Value of the a component of the new color.
	 */
	inline ColorRGBA(float r, float g, float b, float a): color_()
	{ color_[0] = r; color_[1] = g; color_[2] = b; color_[3] = a; }

	/**
	 * Constructor that initializes the components of the color
	 * with the values of a given float array.
	 * @param vec An array of at least three float values. The r component
	 * of the color is set to the value of the first element of the array,
	 * the g component is set to the value of the second element of the
	 * array, the b component is set to the value of the third element of the
	 * array and the a component is set to the value of the fourth element of the
	 * array.
	 */
	inline ColorRGBA(const float *color): color_()
	{ color_[0] = color[0]; color_[1] = color[1]; color_[2] = color[2]; color_[3] = color[3]; }

	/**
	 * Constructor that initializes the components of the color
	 * with the components taken from another color.
	 * @param vec The other color used to initialize the new color.
	 */
	inline ColorRGBA(const ColorRGBA &color): color_()
	{ color_[0] = color.color_[0]; color_[1] = color.color_[1]; color_[2] = color.color_[2];  color_[3] = color.color_[3]; }

	/**
	 * Assigns the components of another %ColorRGBA object to
	 * this object's components.
	 * @param vec The other %ColorRGBA object
	 * @return The other %ColorRGBA object
	 */
	inline const ColorRGBA &operator=(const ColorRGBA &color)
	{
		color_[0] = color.color_[0]; color_[1] = color.color_[1]; color_[2] = color.color_[2]; color_[3] = color.color_[3];
		return color;
	}

	/**
	 * Assigns an array of floats to this object's components.
	 * @param vec An array of at least three float values. The r component
	 * of the color is set to the value of the first element of the array,
	 * the g component is set to the value of the second element of the
	 * array, the g component is set to the value of the third element of the
	 * array, and the a component is set to the value of the fourth element
	 * of the array.
	 * @return The array of floats
	 */
	inline const float *operator =(const float *color)
	{
		color_[0] = color[0]; color_[1] = color[1]; color_[2] = color[2]; color_[3] = color[3];
		return color;
	}

	/**
	 * Sets the value of the r component.
	 * @param r The new value of the r component
	 */
	inline void setR(float r) { color_[0] = r; }

	/**
	 * Returns the value of the r component.
	 * @return The value of the r component
	 */
	inline float getR() const { return color_[0]; }

	/**
	 * Sets the value of the g component.
	 * @param g The new value of the g component
	 */
	inline void setG(float g) { color_[1] = g; }

	/**
	 * Returns the value of the g component.
	 * @return The value of the g component
	 */
	inline float getG() const { return color_[1]; }

	/**
	 * Sets the value of the b component.
	 * @param b The new value of the b component
	 */
	inline void setB(float b) { color_[2] = b; }

	/**
	 * Returns the value of the b component.
	 * @return The value of the b component
	 */
	inline float getB() const { return color_[2]; }
	/**
	 * Sets the value of the a component.
	 * @param a The new value of the a component
	 */
	inline void setA(float a) { color_[3] = a; }

	/**
	 * Returns the value of the a component.
	 * @return The value of the a component
	 */
	inline float getA() const { return color_[3]; }

	/**
	 * Sets the values of the r, g and b components.
	 * @param r The new value of the r component
	 * @param g The new value of the g component
	 * @param b The new value of the b component
	 * @param a The new value of the a component
	 */
	inline void set(float r, float g, float b, float a)
	{ color_[0] = r; color_[1] = g; color_[2] = b; color_[3] = a; }

	/**
	 * Sets the values of the r, g and b components.
	 * @param vec An array of at least three float values. The r component
	 * of the color is set to the value of the first element of the array,
	 * the g component is set to the value of the second element of the
	 * array, and the b component is set to the value of the third element
	 * of the array.
	 */
	inline void set(const float *color)
	{ color_[0] = color[0]; color_[1] = color[1]; color_[2] = color[2]; color_[3] = color[3]; }

	/**
	 * Sets the values of the r, g and b components.
	 * @param vec The other color used to set the components of this color.
	 */
	void set(const ColorRGBA &color)
	{
		color_[0] = color.color_[0];
		color_[1] = color.color_[1];
		color_[2] = color.color_[2];
		color_[2] = color.color_[3];
	}

	/**
	 * Returns the values of the r, b and g components.
	 * @param x Returns the r component of the color
	 * @param y Returns the g component of the color
	 * @param z Returns the b component of the color
	 */
	inline void get(float &r, float &g, float &b, float &a) const
	{ r = color_[0]; g = color_[1]; b = color_[2]; a = color_[3]; }

	/**
	 * Returns the values of the r, g and b components.
	 * @param vec A float array that gets filled with the components of the
	 * color. This array must have at least three elements. The first
	 * element of the array is set to the r component of the color, the
	 * second element of the array is set to the g component of the color,
	 * and the third element of the array is set to the b component of the
	 * color.
	 */
	inline void get(float *color) const
	{ color[0] = color_[0]; color[1] = color_[1]; color[2] = color_[2]; color[3] = color_[3]; }

	/**
	 * Returns the values of the r, g and b components.
	 * @param vec The other color whose components are set to
	 * components of this color.
	 */
	inline void get(ColorRGBA &color) const
	{ color.color_[0] = color_[0]; color.color_[1] = color_[1]; color.color_[2] = color_[2]; color.color_[3] = color_[3]; }

	/**
	 * Returns the values of the r, g and b components.
	 * @return An array of three float elements. The first element of the
	 * array is the r component of the color, the second element of the
	 * array is the g component of the color, and the third element of the
	 * array is the b component of the color.
	 */
	inline const float *get() const { return color_; }

	/**
	 * Returns the values of the r, g and b components.
	 * @return An array of three float elements. The first element of the
	 * array is the r component of the color, the second element of the
	 * array is the g component of the color, and the third element of the
	 * array is the b component of the color.
	 */
	inline float *get() { return color_; }

	/**
	 * Returns a component of the color specified by an index.
	 * @param i The index of the component (must be 0, 1 or 2)
	 * @return The component
	 */
	inline float operator[](int i) const { return color_[i]; }

	/**
	 * Returns a component of the color specified by an index.
	 * @param i The index of the component (must be 0, 1 or 2)
	 * @return The component
	 */
	inline float &operator[](int i) { return color_[i]; }

	/**
	 * Converts the %ColorRGBA object to an array of floats.
	 * @return Array of floats
	 */
	inline operator float *() { return &color_[0]; }

	/**
	 * Converts the %ColorRGBA object to an array of floats.
	 * @return Array of floats
	 */
	inline operator const float *() const { return &color_[0]; }

	/**
	 * Checks if all components of two colors are the same.
	 * @param a The first color
	 * @param b The second color
	 * @return true when all components are the same,
	 * false otherwise
	 */
	friend inline bool operator==(const ColorRGBA &a, const ColorRGBA &b)
	{
		return (a.color_[0] == b.color_[0]) && (a.color_[1] == b.color_[1]) && (a.color_[2] == b.color_[2])&& (a.color_[3] == b.color_[3]);
	}

	/**
	 * Checks if two colors differ at least in one component.
	 * @param a The first color
	 * @param b The second color
	 * @return true when the two colors differ at least in one
	 * component, false otherwise
	 */
	friend inline bool operator!=(const ColorRGBA &a, const ColorRGBA &b)
	{
		return (a.color_[0] != b.color_[0]) || (a.color_[1] != b.color_[1]) || (a.color_[2] != b.color_[2])|| (a.color_[3] != b.color_[3]);
	}

private:

	/** The components of the color */
	float color_[4];
};


/**
 * Helper class that transforms the ColorRGBA data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<ColorRGBA>
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


#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif // __INSTANTIO_COLORRGBA_H
