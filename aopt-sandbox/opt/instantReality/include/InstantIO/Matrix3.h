#ifndef __INSTANTIO_MATRIX3_H
#define __INSTANTIO_MATRIX3_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Matrix3.h                                                       //
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
#include <iostream>
#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4351)
#endif


namespace InstantIO
{


/**
 * @class Matrix3 Matrix3.h InstantIO/Matrix3.h
 *
 * Helper class for exchanging 3x3 transform matrices.
 * %Matrix3 is a basic helper class for exchanging transformations
 * between different software components. It does
 * not contain any means for matrix arithmetics, only methods for
 * setting and getting the transformation. It is not meant to be used
 * directly by software components for matrix representation. Instead,
 * software components should use their own, appropriate classes for
 * handling matrices. Only when sending matrices to an OutSlot, or when
 * receiving matrices from an InSlot, the internal representation of
 * matrices should be converted to Matrix3's. This ensures the
 * interoperability between different software components that use
 * different internal representations for matrices.
 *
 * Matrices are internally stored as 3x3 matrices. You can access
 * these matrices directly, or you can use methods that convert
 * a translation vector, a quaternion axis and a scale vector to a %Matrix3.
 *
 * @author Patrick D&auml;hne
 */
template <class T>
class Matrix3
{
public:

	/** Defines symbolic indices for the elements of the matrix */
	enum MatrixElementIndex
	{
		M_00 = 0, M_01 = 1, M_02 = 2,
		M_10 = 3, M_11 = 4, M_12 = 5,
		M_20 = 6, M_21 = 7, M_22 = 8
	};

	/**
	 * Creates a new %Matrix3 object that contains an identity
	 * matrix.
	 */
	inline Matrix3(): matrix_()
	{ setIdentity(); }

	/**
	 * Creates a new %Matrix3 object that is an exact
	 * copy of another %Matrix3 object.
	 * @param obj The other %Matrix3 object
	 */
	inline Matrix3(const Matrix3 &obj): matrix_()
	{
		matrix_[M_00] = obj.matrix_[M_00];
		matrix_[M_01] = obj.matrix_[M_01];
		matrix_[M_02] = obj.matrix_[M_02];
		matrix_[M_10] = obj.matrix_[M_10];
		matrix_[M_11] = obj.matrix_[M_11];
		matrix_[M_12] = obj.matrix_[M_12];
		matrix_[M_20] = obj.matrix_[M_20];
		matrix_[M_21] = obj.matrix_[M_21];
		matrix_[M_22] = obj.matrix_[M_22];
	}

	/**
	 * Creates a new %Matrix3 object that gets
	 * initialized by a given 9 component float array representing a 3x3 matrix.
	 * @param matrix The nine components of the matrix
	 * @param transposed Flags that must be set to true when the array
	 * needs to be transposed when copying it into the %Matrix3 object
	 */
	inline Matrix3(const T matrix[9], bool transposed = false): matrix_()
	{
		if (transposed == true)
		{
			matrix_[M_00] = matrix[M_00];
			matrix_[M_01] = matrix[M_10];
			matrix_[M_02] = matrix[M_20];
			matrix_[M_10] = matrix[M_01];
			matrix_[M_11] = matrix[M_11];
			matrix_[M_12] = matrix[M_21];
			matrix_[M_20] = matrix[M_02];
			matrix_[M_21] = matrix[M_12];
			matrix_[M_22] = matrix[M_22];
		}
		else
		{
			matrix_[M_00] = matrix[M_00];
			matrix_[M_01] = matrix[M_01];
			matrix_[M_02] = matrix[M_02];
			matrix_[M_10] = matrix[M_10];
			matrix_[M_11] = matrix[M_11];
			matrix_[M_12] = matrix[M_12];
			matrix_[M_20] = matrix[M_20];
			matrix_[M_21] = matrix[M_21];
			matrix_[M_22] = matrix[M_22];
		}
	}

	/**
	 * Assigns another %Matrix3 to this %Matrix3.
	 * @param obj The other %Matrix3 object
	 * @return The other %Matrix3 object
	 */
	inline const Matrix3 &operator=(const Matrix3 &obj)
	{
		matrix_[M_00] = obj.matrix_[M_00];
		matrix_[M_01] = obj.matrix_[M_01];
		matrix_[M_02] = obj.matrix_[M_02];
		matrix_[M_10] = obj.matrix_[M_10];
		matrix_[M_11] = obj.matrix_[M_11];
		matrix_[M_12] = obj.matrix_[M_12];
		matrix_[M_20] = obj.matrix_[M_20];
		matrix_[M_21] = obj.matrix_[M_21];
		matrix_[M_22] = obj.matrix_[M_22];
		return obj;
	}

	/**
	 * Assigns the nine components of an float array to
	 * this %Matrix3.
	 * @param matrix The nine components of the matrix
	 * @return The nine components of the matrix
	 */
	inline const T *operator=(const T matrix[9])
	{
		matrix_[M_00] = matrix[M_00];
		matrix_[M_01] = matrix[M_01];
		matrix_[M_02] = matrix[M_02];
		matrix_[M_10] = matrix[M_10];
		matrix_[M_11] = matrix[M_11];
		matrix_[M_12] = matrix[M_12];
		matrix_[M_20] = matrix[M_20];
		matrix_[M_21] = matrix[M_21];
		matrix_[M_22] = matrix[M_22];
		return matrix;
	}

	/** Sets the %Matrix3 to identity */
	void setIdentity();

	/**
	 * Returns a component of the %Matrix3.
	 * @param i The index of the component
	 * @return The component
	 */
	inline T operator[](MatrixElementIndex i) const { return matrix_[i]; }

	/**
	 * Returns a component of the %Matrix3.
	 * @param i The index of the component
	 * @return The component
	 */
	inline T &operator[](MatrixElementIndex i) { return matrix_[i]; }

	/**
	 * Returns the components of the %Matrix3 as an
	 * array of nine float values.
	 * @return The components of the %Matrix3
	 */
	inline const T *getValue() const { return matrix_; }

	/**
	 * Returns the components of the %Matrix3 as an
	 * array of nine float values.
	 * @return The components of the %Matrix3
	 */
	inline T *getValue() { return matrix_; }

	/**
	 * Transforms the %Matrix3 to an array of 9 float values.
	 * @return The array of 9 float values.
	 */
	inline operator T *() { return &matrix_[0]; }

	/**
	 * Transforms the %Matrix3 to an array of 9 float values.
	 * @return The array of 9 float values.
	 */
	inline operator const T *() const { return &matrix_[0]; }

private:

	/** The components of the matrix. */
	T matrix_[9];
};


template <class T>
void Matrix3<T>::setIdentity()
{
	matrix_[M_01] = matrix_[M_02] = 0.f;
	matrix_[M_10] = matrix_[M_12] = 0.f;
	matrix_[M_20] = matrix_[M_21] = 0.f;
	matrix_[M_00] = matrix_[M_11] = matrix_[M_22] = 1.f;
}


typedef Matrix3<double> Matrix3d;


typedef Matrix3<float> Matrix3f;


/**
 * Helper class that transforms the Matrix3d data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<Matrix3d>
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
 * Helper class that transforms the Matrix3f data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<Matrix3f>
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


template <class T>
std::ostream &operator <<(std::ostream &os, const Matrix3<T> &obj)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
			os << obj[j * 3 + i] << ' ';
		os << std::endl;
	}
	return os;
}


template <class T>
std::istream &operator >>(std::istream &is, Matrix3<T> &obj)
{
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if (is.good())
				is >> obj[j * 3 + i];
	return is;
}


} // namespace InstantIO


#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif // __INSTANTIO_MATRIX3_H
