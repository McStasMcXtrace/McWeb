#ifndef __INSTANTIO_VEC3_H
#define __INSTANTIO_VEC3_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Vec3.h                                                          //
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
 * @class Vec3 Vec3.h InstantIO/Vec3.h
 *
 * Helper class for exchanging vectors of three float components.
 * %Vec3 is a basic helper class for exchanging vectors of three
 * float components between different software components. It does
 * not contain any means for vector arithmetics, only methods for
 * setting and getting the components. It is not meant to be used
 * directly by software components for vector representation. Instead,
 * software components should use their own, appropriate classes for
 * handling vectors. Only when sending vectors to an OutSlot, or when
 * receiving vectors from an InSlot, the internal representation of
 * vectors should be converted to Vec3's. This ensures the
 * interoperability between different software components that use
 * different internal representations for vectors.
 *
 * @author Patrick D&auml;hne
 */
template <class T> class Vec3
{
public:

	/**
	 * Constructor that initializes all components of the vector
	 * with 0.
	 */
	inline Vec3(): vec_()
	{ vec_[0] = vec_[1] = vec_[2] = static_cast<T>(0); }

	/**
	 * Constructor that initializes the components of the vector
	 * with the given float values.
	 * @param x Value of the x component of the new vector.
	 * @param y Value of the y component of the new vector.
	 * @param z Value of the z component of the new vector.
	 */
	inline Vec3(T x, T y, T z): vec_()
	{ vec_[0] = x; vec_[1] = y; vec_[2] = z; }

	/**
	 * Constructor that initializes the components of the vector
	 * with the values of a given float array.
	 * @param vec An array of at least three float values. The x component
	 * of the vector is set to the value of the first element of the array,
	 * the y component is set to the value of the second element of the
	 * array, and the z component is set to the value of the third element of the
	 * array.
	 */
	inline Vec3(const T *vec): vec_()
	{ vec_[0] = vec[0]; vec_[1] = vec[1]; vec_[2] = vec[2]; }
	
	/**
	 * Constructor that initializes the components of the vector
	 * with the components taken from another vector.
	 * @param vec The other vector used to initialize the new vector.
	 */
	inline Vec3(const Vec3 &vec): vec_()
	{ vec_[0] = vec.vec_[0]; vec_[1] = vec.vec_[1]; vec_[2] = vec.vec_[2]; }

	/**
	 * Assigns the components of another %Vec3 object to
	 * this object's components.
	 * @param vec The other %Vec3 object
	 * @return The other %Vec3 object
	 */
	inline const Vec3 &operator=(const Vec3 &vec)
	{
		vec_[0] = vec.vec_[0]; vec_[1] = vec.vec_[1]; vec_[2] = vec.vec_[2];
		return vec;
	}

	/**
	 * Assigns an array of floats to this object's components.
	 * @param vec An array of at least three float values. The x component
	 * of the vector is set to the value of the first element of the array,
	 * the y component is set to the value of the second element of the
	 * array, and the z component is set to the value of the third element of the
	 * array.
	 * @return The array of floats
	 */
	inline const T *operator =(const T *vec)
	{
		vec_[0] = vec[0]; vec_[1] = vec[1]; vec_[2] = vec[2];
		return vec;
	}

	/**
	 * Sets the value of the x component.
	 * @param x The new value of the x component
	 */
	inline void setX(T x) { vec_[0] = x; }

	/**
	 * Returns the value of the x component.
	 * @return The value of the x component
	 */
	inline T getX() const { return vec_[0]; }

	/**
	 * Sets the value of the y component.
	 * @param y The new value of the y component
	 */
	inline void setY(T y) { vec_[1] = y; }

	/**
	 * Returns the value of the y component.
	 * @return The value of the y component
	 */
	inline T getY() const { return vec_[1]; }

	/**
	 * Sets the value of the z component.
	 * @param z The new value of the z component
	 */
	inline void setZ(T z) { vec_[2] = z; }

	/**
	 * Returns the value of the z component.
	 * @return The value of the z component
	 */
	inline T getZ() const { return vec_[2]; }

	/**
	 * Sets the values of the x, y and z components.
	 * @param x The new value of the x component
	 * @param y The new value of the y component
	 * @param z The new value of the z component
	 */
	inline void set(T x, T y, T z)
	{ vec_[0] = x; vec_[1] = y; vec_[2] = z; }
	
	/**
	 * Sets the values of the x, y and z components.
	 * @param vec An array of at least three float values. The x component
	 * of the vector is set to the value of the first element of the array,
	 * the y component is set to the value of the second element of the
	 * array, and the z component is set to the value of the third element
	 * of the array.
	 */
	inline void set(const T *vec)
	{ vec_[0] = vec[0]; vec_[1] = vec[1]; vec_[2] = vec[2]; }
	
	/**
	 * Sets the values of the x, y and z components.
	 * @param vec The other vector used to set the components of this vector.
	 */
	void set(const Vec3 &vec)
	{
		vec_[0] = vec.vec_[0];
		vec_[1] = vec.vec_[1];
		vec_[2] = vec.vec_[2];
	}

	/**
	 * Returns the values of the x, y and z components.
	 * @param x Returns the x component of the vector
	 * @param y Returns the y component of the vector
	 * @param z Returns the z component of the vector
	 */
	inline void get(T &x, T &y, T &z) const
	{ x = vec_[0]; y = vec_[1]; z = vec_[2]; }

	/**
	 * Returns the values of the x, y and z components.
	 * @param vec A float array that gets filled with the components of the
	 * vector. This array must have at least three elements. The first
	 * element of the array is set to the x component of the vector, the
	 * second element of the array is set to the y component of the vector,
	 * and the third element of the array is set to the z component of the
	 * vector.
	 */
	inline void get(T *vec) const
	{ vec[0] = vec_[0]; vec[1] = vec_[1]; vec[2] = vec_[2]; }

	/**
	 * Returns the values of the x, y and z components.
	 * @param vec The other vector whose components are set to
	 * components of this vector.
	 */
	inline void get(Vec3 &vec) const
	{ vec.vec_[0] = vec_[0]; vec.vec_[1] = vec_[1]; vec.vec_[2] = vec_[2]; }

	/**
	 * Returns the values of the x, y and z components.
	 * @return An array of three float elements. The first element of the
	 * array is the x component of the vector, the second element of the
	 * array is the y component of the vector, and the third element of the
	 * array is the z component of the vector.
	 */
	inline const T *get() const { return vec_; }

	/**
	 * Returns the values of the x, y and z components.
	 * @return An array of three float elements. The first element of the
	 * array is the x component of the vector, the second element of the
	 * array is the y component of the vector, and the third element of the
	 * array is the z component of the vector.
	 */
	inline T *get() { return vec_; }

	/**
	 * Returns a component of the vector specified by an index.
	 * @param i The index of the component (must be 0, 1 or 2)
	 * @return The component
	 */
	inline T operator[](int i) const { return vec_[i]; }

	/**
	 * Returns a component of the vector specified by an index.
	 * @param i The index of the component (must be 0, 1 or 2)
	 * @return The component
	 */
	inline T &operator[](int i) { return vec_[i]; }

	/**
	 * Converts the %Vec3 object to an array of floats.
	 * @return Array of floats
	 */
	inline operator T *() { return &vec_[0]; }

	/**
	 * Converts the %Vec3 object to an array of floats.
	 * @return Array of floats
	 */
	inline operator const T *() const { return &vec_[0]; }

	/**
	 * Checks if all components of two vectors are the same.
	 * @param a The first vector
	 * @param b The second vector
	 * @return true when all components are the same,
	 * false otherwise
	 */
	friend inline bool operator==(const Vec3 &a, const Vec3 &b)
	{
		return (a.vec_[0] == b.vec_[0]) && (a.vec_[1] == b.vec_[1]) && (a.vec_[2] == b.vec_[2]);
	}

	/**
	 * Checks if two vectors differ at least in one component.
	 * @param a The first vector
	 * @param b The second vector
	 * @return true when the two vectors differ at least in one
	 * component, false otherwise
	 */
	friend inline bool operator!=(const Vec3 &a, const Vec3 &b)
	{
		return (a.vec_[0] != b.vec_[0]) || (a.vec_[1] != b.vec_[1]) || (a.vec_[2] != b.vec_[2]);
	}

private:

	/** The components of the vector */
	T vec_[3];
};


typedef Vec3<double> Vec3d;


typedef Vec3<float> Vec3f;


/**
 * Helper class that transforms the Vec3d data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<Vec3d>
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


/**
 * Helper class that transforms the Vec3f data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<Vec3f>
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

#endif // __INSTANTIO_VEC3_H
