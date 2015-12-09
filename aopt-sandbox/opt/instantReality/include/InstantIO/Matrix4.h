#ifndef __INSTANTIO_MATRIX4_H
#define __INSTANTIO_MATRIX4_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Matrix4.h                                                       //
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
#include <cmath>
#include <iostream>
#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4351)
#endif


namespace InstantIO
{


/**
 * @class Matrix4 Matrix4.h InstantIO/Matrix4.h
 *
 * Helper class for exchanging 4x4 transform matrices.
 * %Matrix4 is a basic helper class for exchanging transformations
 * between different software components. It does
 * not contain any means for matrix arithmetics, only methods for
 * setting and getting the transformation. It is not meant to be used
 * directly by software components for matrix representation. Instead,
 * software components should use their own, appropriate classes for
 * handling matrices. Only when sending matrices to an OutSlot, or when
 * receiving matrices from an InSlot, the internal representation of
 * matrices should be converted to Matrix4's. This ensures the
 * interoperability between different software components that use
 * different internal representations for matrices.
 *
 * Matrices are internally stored as 4x4 matrices. You can access
 * these matrices directly, or you can use methods that convert
 * a translation vector, a quaternion axis and a scale vector to a %Matrix4.
 *
 * @author Helmut Seibert
 */
template <class T>
class Matrix4
{
public:

	/** Defines symbolic indices for the elements of the matrix */
	enum MatrixElementIndex
	{
		M_00 =  0, M_01 =  1, M_02 =  2, M_03 =  3,
		M_10 =  4, M_11 =  5, M_12 =  6, M_13 =  7,
		M_20 =  8, M_21 =  9, M_22 = 10, M_23 = 11,
		M_30 = 12, M_31 = 13, M_32 = 14, M_33 = 15
	};

	/** Defines symbolic indices for the elements of a quaternion */
	enum QuaternionElementIndex { Q_X = 0, Q_Y = 1, Q_Z = 2, Q_W = 3 };

	/** Defines symbolic indices for the elements of a vector */
	enum VectorElementIndex { V_X = 0, V_Y = 1, V_Z = 2 };

	/** Epsilon value used for comparisons */
	static const T epsilon;

	/**
	 * Creates a new %Matrix4 object that contains an identity
	 * matrix.
	 */
	inline Matrix4(): matrix_()
	{ setIdentity(); }

	/**
	 * Creates a new %Matrix4 object that is an exact
	 * copy of another %Matrix4 object.
	 * @param obj The other %Matrix4 object
	 */
	inline Matrix4(const Matrix4 &obj): matrix_()
	{
		matrix_[M_00] = obj.matrix_[M_00]; matrix_[M_01] = obj.matrix_[M_01];
		matrix_[M_02] = obj.matrix_[M_02]; matrix_[M_03] = obj.matrix_[M_03];
		matrix_[M_10] = obj.matrix_[M_10]; matrix_[M_11] = obj.matrix_[M_11];
		matrix_[M_12] = obj.matrix_[M_12]; matrix_[M_13] = obj.matrix_[M_13];
		matrix_[M_20] = obj.matrix_[M_20]; matrix_[M_21] = obj.matrix_[M_21];
		matrix_[M_22] = obj.matrix_[M_22]; matrix_[M_23] = obj.matrix_[M_23];
		matrix_[M_30] = obj.matrix_[M_30]; matrix_[M_31] = obj.matrix_[M_31];
		matrix_[M_32] = obj.matrix_[M_32]; matrix_[M_33] = obj.matrix_[M_33];
	}

	/**
	 * Creates a new %Matrix4 object that gets
	 * initialized by a given 16 component float array representing a 4x4 matrix.
	 * @param matrix The sixteen components of the matrix
	 * @param transposed Flags that must be set to true when the array
	 * needs to be transposed when copying it into the %Matrix4 object
	 */
	inline Matrix4(const T matrix[16], bool transposed = false): matrix_()
	{
		if (transposed == true)
		{
			matrix_[M_00] = matrix[M_00]; matrix_[M_01] = matrix[M_10];
			matrix_[M_02] = matrix[M_20]; matrix_[M_03] = matrix[M_30];
			matrix_[M_10] = matrix[M_01]; matrix_[M_11] = matrix[M_11];
			matrix_[M_12] = matrix[M_21]; matrix_[M_13] = matrix[M_31];
			matrix_[M_20] = matrix[M_02]; matrix_[M_21] = matrix[M_12];
			matrix_[M_22] = matrix[M_22]; matrix_[M_23] = matrix[M_32];
			matrix_[M_30] = matrix[M_03]; matrix_[M_31] = matrix[M_13];
			matrix_[M_32] = matrix[M_23]; matrix_[M_33] = matrix[M_33];
		}
		else
		{
			matrix_[M_00] = matrix[M_00]; matrix_[M_01] = matrix[M_01];
			matrix_[M_02] = matrix[M_02]; matrix_[M_03] = matrix[M_03];
			matrix_[M_10] = matrix[M_10]; matrix_[M_11] = matrix[M_11];
			matrix_[M_12] = matrix[M_12]; matrix_[M_13] = matrix[M_13];
			matrix_[M_20] = matrix[M_20]; matrix_[M_21] = matrix[M_21];
			matrix_[M_22] = matrix[M_22]; matrix_[M_23] = matrix[M_23];
			matrix_[M_30] = matrix[M_30]; matrix_[M_31] = matrix[M_31];
			matrix_[M_32] = matrix[M_32]; matrix_[M_33] = matrix[M_33];
		}
	}

	/**
	 * Creates a new %Matrix4 object and initializes it with
	 * a given translation and rotation.
	 * @param tx The first component of the translation
	 * @param ty The second component of the translation
	 * @param tz The third component of the translation
	 * @param qx The first component of the quaternion
	 * @param qy The second component of the quaternion
	 * @param qz The third component of the quaternion
	 * @param qw The fourth component of the quaternion
	 */
	inline Matrix4(T tx, T ty, T tz, T qx, T qy, T qz, T qw)
	: matrix_()
	{
		setTransform(tx, ty, tz, qx, qy, qz, qw);
	}

	/**
	 * Creates a new %Matrix4 object and initializes it with
	 * a given translation, rotation and scale.
	 * @param tx The first component of the translation
	 * @param ty The second component of the translation
	 * @param tz The third component of the translation
	 * @param qx The first component of the quaternion
	 * @param qy The second component of the quaternion
	 * @param qz The third component of the quaternion
	 * @param qw The fourth component of the quaternion
	 * @param sx The first component of the scale
	 * @param sy The second component of the scale
	 * @param sz The third component of the scale
	 */
	inline Matrix4(T tx, T ty, T tz, T qx, T qy, T qz, T qw, T sx, T sy, T sz)
	: matrix_()
	{
		setTransform(tx, ty, tz, qx, qy, qz, qw);
		applyScale(sx, sy, sz);
	}

	/**
	 * Assigns another %Matrix4 to this %Matrix4.
	 * @param obj The other %Matrix4 object
	 * @return The other %Matrix4 object
	 */
	inline const Matrix4 &operator=(const Matrix4 &obj)
	{
		matrix_[M_00] = obj.matrix_[M_00]; matrix_[M_01] = obj.matrix_[M_01];
		matrix_[M_02] = obj.matrix_[M_02]; matrix_[M_03] = obj.matrix_[M_03];
		matrix_[M_10] = obj.matrix_[M_10]; matrix_[M_11] = obj.matrix_[M_11];
		matrix_[M_12] = obj.matrix_[M_12]; matrix_[M_13] = obj.matrix_[M_13];
		matrix_[M_20] = obj.matrix_[M_20]; matrix_[M_21] = obj.matrix_[M_21];
		matrix_[M_22] = obj.matrix_[M_22]; matrix_[M_23] = obj.matrix_[M_23];
		matrix_[M_30] = obj.matrix_[M_30]; matrix_[M_31] = obj.matrix_[M_31];
		matrix_[M_32] = obj.matrix_[M_32]; matrix_[M_33] = obj.matrix_[M_33];
		return obj;
	}

	/**
	 * Assigns the sixteen components of an float array to
	 * this %Matrix4.
	 * @param matrix The sixteen components of the matrix
	 * @return The sixteen components of the matrix
	 */
	inline const T *operator=(const T matrix[16])
	{
		matrix_[M_00] = matrix[M_00]; matrix_[M_01] = matrix[M_01];
		matrix_[M_02] = matrix[M_02]; matrix_[M_03] = matrix[M_03];
		matrix_[M_10] = matrix[M_10]; matrix_[M_11] = matrix[M_11];
		matrix_[M_12] = matrix[M_12]; matrix_[M_13] = matrix[M_13];
		matrix_[M_20] = matrix[M_20]; matrix_[M_21] = matrix[M_21];
		matrix_[M_22] = matrix[M_22]; matrix_[M_23] = matrix[M_23];
		matrix_[M_30] = matrix[M_30]; matrix_[M_31] = matrix[M_31];
		matrix_[M_32] = matrix[M_32]; matrix_[M_33] = matrix[M_33];
		return matrix;
	}

	/** Sets the %Matrix4 to identity */
	void setIdentity();

	/**
	 * Sets the %Matrix4 from three components of a translation.
	 * @param tx The first component of the translation
	 * @param ty The second component of the translation
	 * @param tz The third component of the translation
	 */
	void setTransform(T tx, T ty, T tz);

	/**
	 * Sets the %Matrix4 from four components of a quaternion.
	 * @param qx The first component of the quaternion
	 * @param qy The second component of the quaternion
	 * @param qz The third component of the quaternion
	 * @param qw The fourth component of the quaternion
	 */
	void setTransform(T qx, T qy, T qz, T qw);

	/**
	 * Sets the %Matrix4 from three components of a translation and four components of a quaternion.
	 * @param tx The first component of the translation
	 * @param ty The second component of the translation
	 * @param tz The third component of the translation
	 * @param qx The first component of the quaternion
	 * @param qy The second component of the quaternion
	 * @param qz The third component of the quaternion
	 * @param qw The fourth component of the quaternion
	 */
	void setTransform(T tx, T ty, T tz, T qx, T qy, T qz, T qw);


	/**
	 * Sets the %Matrix4 from three components of a translation, four components of a quaternion
	 * and three elements of a scale vector.
	 * @param tx The first component of the translation
	 * @param ty The second component of the translation
	 * @param tz The third component of the translation
	 * @param qx The first component of the quaternion
	 * @param qy The second component of the quaternion
	 * @param qz The third component of the quaternion
	 * @param qw The fourth component of the quaternion
	 * @param sx The first component of the scale
	 * @param sy The second component of the scale
	 * @param sz The third component of the scale
	 */
	inline void setTransform(T tx, T ty, T tz, T qx, T qy, T qz, T qw, T sx, T sy, T sz)
	{
		setTransform(tx, ty, tz, qx, qy, qz, qw);
		applyScale(sx, sy, sz);
	}

	/**
	 * Sets the %Matrix4 from a from a translation , a quaternion and a scale.
	 * @param t The translation (arb. 3 component vector)
	 * @param q The rotation as quatenion (arb. 4 component vector)
	 * @param s The scale (arb. 3 component vector)
	 */
	template <class TranslationT, class QuaternionT, class ScaleT>
	inline void setTransform(const TranslationT &t,const QuaternionT &q, const ScaleT &s)
	{
		setTransform(t, q);
		applyScale(s);
	}


	/**
	 * Sets the %Matrix4 from a translation and a quaternion.
	 * @param t The translation (arb. 3 component vector)
	 * @param q The rotation as quatenion (arb. 4 component vector)
	 */
	template <class TranslationT, class QuaternionT>
	inline void setTransform(const TranslationT &t, const QuaternionT &q)
	{
		setRotationFromQuaternion(q);
		setTranslation(t);
		matrix_[M_30] = 0.f;
		matrix_[M_31] = 0.f;
		matrix_[M_32] = 0.f;
		matrix_[M_33] = 1.f;
	}

	/**
	 * Sets the %Matrix4 from a translation, a rotation axis and a rotation angle
	 * @param translation The translation (arb. 3 component vector)
	 * @param axis The rotation axis (arb. 3 component vector)
	 * @param angle The rotation angle (in radians)
	 */
	template <class Vec3fT>
	void setTranslationAxisAngle(const Vec3fT &translation, const Vec3fT &axis, T angle)
	{
		T qx, qy, qz, qw;
		T length = static_cast<T>(sqrtf(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]));
		T angle2 = angle / 2.f;
		T sangle2l = static_cast<T>(sin(angle2) / length);
		qx = sangle2l * axis[V_X];
		qy = sangle2l * axis[V_Y];
		qz = sangle2l * axis[V_Z];
		qw = static_cast<T>(cos(angle2));
		setTransform(translation[V_X], translation[V_Y], translation[V_Z], qx, qy, qz, qw);
	}

	/**
	 * Sets the %Matrix4 elements from an array of 16 values.
	 * @param array An arb. 16 component array for the matrix elements
	 * @param transposed Flags that must be set to true when the array
	 * needs to be transposed when copying it into the %Matrix4 object
	 */
	template <class ArrayT>
	inline void setMatrixFromArray(const ArrayT &array, bool transposed = false)
	{
		if (transposed == true)
		{
			matrix_[M_00] = array[M_00]; matrix_[M_01] = array[M_10];
			matrix_[M_02] = array[M_20]; matrix_[M_03] = array[M_30];
			matrix_[M_10] = array[M_01]; matrix_[M_11] = array[M_11];
			matrix_[M_12] = array[M_21]; matrix_[M_13] = array[M_31];
			matrix_[M_20] = array[M_02]; matrix_[M_21] = array[M_12];
			matrix_[M_22] = array[M_22]; matrix_[M_23] = array[M_32];
			matrix_[M_30] = array[M_03]; matrix_[M_31] = array[M_13];
			matrix_[M_32] = array[M_23]; matrix_[M_33] = array[M_33];
		}
		else
		{
			matrix_[M_00] = array[M_00]; matrix_[M_01] = array[M_01];
			matrix_[M_02] = array[M_02]; matrix_[M_03] = array[M_03];
			matrix_[M_10] = array[M_10]; matrix_[M_11] = array[M_11];
			matrix_[M_12] = array[M_12]; matrix_[M_13] = array[M_13];
			matrix_[M_20] = array[M_20]; matrix_[M_21] = array[M_21];
			matrix_[M_22] = array[M_22]; matrix_[M_23] = array[M_23];
			matrix_[M_30] = array[M_30]; matrix_[M_31] = array[M_31];
			matrix_[M_32] = array[M_32]; matrix_[M_33] = array[M_33];
		}
	}

	/**
	 * Sets the %Matrix4 elements from a matrix.
	 * @param matrix An arb. 4x4 component matrix for the matrix elements
	 * @param transposed Flags that must be set to true when the matrix
	 * needs to be transposed when copying it into the %Matrix4 object
	 */
	template <class MatrixT>
	inline void setMatrixFromMatrix(const MatrixT &matrix, bool transposed = false)
	{
		if (transposed == true)
		{
			matrix_[M_00] = matrix[0][0]; matrix_[M_01] = matrix[1][0];
			matrix_[M_02] = matrix[2][0]; matrix_[M_03] = matrix[3][0];
			matrix_[M_10] = matrix[0][1]; matrix_[M_11] = matrix[1][1];
			matrix_[M_12] = matrix[2][1]; matrix_[M_13] = matrix[3][1];
			matrix_[M_20] = matrix[0][2]; matrix_[M_21] = matrix[1][2];
			matrix_[M_22] = matrix[2][2]; matrix_[M_23] = matrix[3][2];
			matrix_[M_30] = matrix[0][3]; matrix_[M_31] = matrix[1][3];
			matrix_[M_32] = matrix[2][3]; matrix_[M_33] = matrix[3][3];
		}
		else
		{
			matrix_[M_00] = matrix[0][0]; matrix_[M_01] = matrix[0][1];
			matrix_[M_02] = matrix[0][2]; matrix_[M_03] = matrix[0][3];
			matrix_[M_10] = matrix[1][0]; matrix_[M_11] = matrix[1][1];
			matrix_[M_12] = matrix[1][2]; matrix_[M_13] = matrix[1][3];
			matrix_[M_20] = matrix[2][0]; matrix_[M_21] = matrix[2][1];
			matrix_[M_22] = matrix[2][2]; matrix_[M_23] = matrix[2][3];
			matrix_[M_30] = matrix[3][0]; matrix_[M_31] = matrix[3][1];
			matrix_[M_32] = matrix[3][2]; matrix_[M_33] = matrix[3][3];
		}
	}

	/**
	 * Sets the value of the %Matrix4 from a quaternion.
	 * @param qx The first component of the quaternion
	 * @param qy The second component of the quaternion
	 * @param qz The third component of the quaternion
	 * @param qw The fourth component of the quaternion
	 */
	void setRotationFromQuaternion(T qx, T qy, T qz, T qw);

	/**
	 * Sets the value of the %Matrix4 from a quaternion.
	 * @param q The rotation as quatenion (arb. 4 component vector)
	 */
	template<class QuaternionT>
	inline void setRotationFromQuaternion(const QuaternionT &q)
	{
		setRotationFromQuaternion(q[Q_X], q[Q_Y], q[Q_Z], q[Q_W]);
	}

	/**
	 * Sets the value of the %Matrix4 from a 3x3 rotation matrix.
	 * @param r The rotation as matrix (matrix type with double indexed access e.g. float[3][3])
	 * @param transposed Flags that must be set to true when the matrix
	 * needs to be transposed when copying it into the %Matrix4 object
	 */
	template <class RotationMatrixT>
	inline void setRotationFromRotationMatrix(const RotationMatrixT &r, bool transposed = false)
	{
		if (transposed == true)
		{
			matrix_[M_00] = r[0][0];
			matrix_[M_01] = r[1][0];
			matrix_[M_02] = r[2][0];

			matrix_[M_10] = r[0][1];
			matrix_[M_11] = r[1][1];
			matrix_[M_12] = r[2][1];

			matrix_[M_20] = r[0][2];
			matrix_[M_21] = r[1][2];
			matrix_[M_22] = r[2][2];

		}
		else
		{
			matrix_[M_00] = r[0][0];
			matrix_[M_10] = r[1][0];
			matrix_[M_20] = r[2][0];

			matrix_[M_01] = r[0][1];
			matrix_[M_11] = r[1][1];
			matrix_[M_21] = r[2][1];

			matrix_[M_02] = r[0][2];
			matrix_[M_12] = r[1][2];
			matrix_[M_22] = r[2][2];
		}
	}

	/**
	 * Sets the translation components of the %Matrix4.
	 * @param translation The translation (arb. 3 component vector)
	 */
	template<class TranslationT>
	inline void setTranslation(const TranslationT &translation)
	{
		matrix_[M_03] = translation[V_X];
		matrix_[M_13] = translation[V_Y];
		matrix_[M_23] = translation[V_Z];
	}

	/**
	 * Sets the translation components of the %Matrix4.
	 * @param tx The first component of the translation
	 * @param ty The second component of the translation
	 * @param tz The third component of the translation
	 */
	void setTranslation(T tx, T ty, T tz);

	/**
	 * Scales the %Matrix4.
	 * @param s The scale (arb. 3 component vector)
	 */
	template <class ScaleT>
	inline void applyScale(const ScaleT &s)
	{
		applyScale(s[V_X], s[V_Y], s[V_Z]);
	}

	/**
	 * Scales the %Matrix4.
	 * @param sx The x scale
	 * @param sy The y scale
	 * @param sz The z scale
	 */
	void applyScale(T sx, T sy, T sz);

	/**
	 * Returns a component of the %Matrix4.
	 * @param i The index of the component
	 * @return The component
	 */
	inline T operator[](MatrixElementIndex i) const { return matrix_[i]; }

	/**
	 * Returns a component of the %Matrix4.
	 * @param i The index of the component
	 * @return The component
	 */
	inline T &operator[](MatrixElementIndex i) { return matrix_[i]; }

	/**
	 * Returns the components of the %Matrix4 as an
	 * array of 16 float values.
	 * @return The components of the %Matrix4
	 */
	inline const T *getValue() const { return matrix_; }

	/**
	 * Returns the components of the %Matrix4 as an
	 * array of 16 float values.
	 * @return The components of the %Matrix4
	 */
	inline T *getValue() { return matrix_; }

	/**
	 * Returns the transformation stored in the %Matrix4 as
	 * a translation, a quaternion, and a scale.
	 * @param tx The first component of the translation
	 * @param ty The second component of the translation
	 * @param tz The third component of the translation
	 * @param qx The first component of the quaternion
	 * @param qy The second component of the quaternion
	 * @param qz The third component of the quaternion
	 * @param qw The fourth component of the quaternion
	 * @param sx The first component of the scale
	 * @param sy The second component of the scale
	 * @param sz The third component of the scale
	 */
	void getTransform(T &tx, T &ty, T &tz, T &qx, T &qy, T &qz, T &qw, T &sx, T &sy, T &sz) const;

	/**
	 * Returns the transformation stored in the %Matrix4 as
	 * a translation and a quaternion.
	 * @param tx The first component of the translation
	 * @param ty The second component of the translation
	 * @param tz The third component of the translation
	 * @param qx The first component of the quaternion
	 * @param qy The second component of the quaternion
	 * @param qz The third component of the quaternion
	 * @param qw The fourth component of the quaternion
	 */
	void getTransform(T &tx, T &ty, T &tz, T &qx, T &qy, T &qz, T &qw) const;

	/**
	 * Returns the transformation stored in the %Matrix4 as
	 * a translation, a quaternion, and a scale.
	 * @param t The translation (arb. 3 component vector)
	 * @param q The rotation as quatenion (arb. 4 component vector)
	 * @param s The scale (arb. 3 component vector)
	 */
	template <class TranslationT, class QuaternionT, class ScaleT>
	inline void getTransform(TranslationT &t, QuaternionT &q, ScaleT &s) const
	{
		T tx, ty, tz, qx, qy, qz, qw, sx, sy, sz;
		getTransform(tx, ty, tz, qx, qy, qz, qw, sx, sy, sz);
		t[V_X] = tx; t[V_Y] = ty; t[V_Z] = tz;
		s[V_X] = sx; s[V_Y] = sy; s[V_Z] = sz;
		q[Q_W] = qw; q[Q_X] = qx; q[Q_Y] = qy; q[Q_Z] = qz;
	}

	/**
	 * Returns the transformation stored in the %Matrix4 as
	 * a translation and a quaternion.
	 * @param t The translation (arb. 3 component vector)
	 * @param q The rotation as quatenion (arb. 4 component vector)
	 */
	template <class TranslationT, class QuaternionT>
	inline void getTransform(TranslationT &t, QuaternionT &q) const
	{
		T tx, ty, tz, qx, qy, qz, qw;
		getTransform(tx, ty, tz, qx, qy, qz, qw);
		t[V_X] = tx; t[V_Y] = ty; t[V_Z] = tz;
		q[Q_W] = qw; q[Q_X] = qx; q[Q_Y] = qy; q[Q_Z] = qz;
	}

	/**
	 * Returns the components of the %Matrix4 in an array of 16 values.
	 * @param matrix An arb. 16 component array for the matrix elements
	 * @param transposed Flags that must be set to true when the %Matrix4
	 * components need to be transposed when copying them into the array
	 */
	template <class ArrayT>
	inline void getMatrixAsArray(ArrayT &matrix, bool transposed = false) const
	{
		if (transposed == true)
		{
			matrix[M_00] = matrix_[M_00]; matrix[M_01] = matrix_[M_10];
			matrix[M_02] = matrix_[M_20]; matrix[M_03] = matrix_[M_30];
			matrix[M_10] = matrix_[M_01]; matrix[M_11] = matrix_[M_11];
			matrix[M_12] = matrix_[M_21]; matrix[M_13] = matrix_[M_31];
			matrix[M_20] = matrix_[M_02]; matrix[M_21] = matrix_[M_12];
			matrix[M_22] = matrix_[M_22]; matrix[M_23] = matrix_[M_32];
			matrix[M_30] = matrix_[M_03]; matrix[M_31] = matrix_[M_13];
			matrix[M_32] = matrix_[M_23]; matrix[M_33] = matrix_[M_33];
		}
		else
		{
			matrix[M_00] = matrix_[M_00]; matrix[M_01] = matrix_[M_01];
			matrix[M_02] = matrix_[M_02]; matrix[M_03] = matrix_[M_03];
			matrix[M_10] = matrix_[M_10]; matrix[M_11] = matrix_[M_11];
			matrix[M_12] = matrix_[M_12]; matrix[M_13] = matrix_[M_13];
			matrix[M_20] = matrix_[M_20]; matrix[M_21] = matrix_[M_21];
			matrix[M_22] = matrix_[M_22]; matrix[M_23] = matrix_[M_23];
			matrix[M_30] = matrix_[M_30]; matrix[M_31] = matrix_[M_31];
			matrix[M_32] = matrix_[M_32]; matrix[M_33] = matrix_[M_33];
		}
	}

	/**
	 * Returns the components of the %Matrix4 in a matrix.
	 * @param matrix An arb. 16 component array for the matrix elements
	 * @param transposed Flags that must be set to true when the %Matrix4
	 * components need to be transposed when copying them into the array
	 */
	template <class MatrixT>
	inline void getMatrixAsMatrix(MatrixT &matrix, bool transposed = false) const
	{
		if (transposed == true)
		{
			matrix[0][0] = matrix_[M_00]; matrix[0][1] = matrix_[M_10];
			matrix[0][2] = matrix_[M_20]; matrix[0][3] = matrix_[M_30];
			matrix[1][0] = matrix_[M_01]; matrix[1][1] = matrix_[M_11];
			matrix[1][2] = matrix_[M_21]; matrix[1][3] = matrix_[M_31];
			matrix[2][0] = matrix_[M_02]; matrix[2][1] = matrix_[M_12];
			matrix[2][2] = matrix_[M_22]; matrix[2][3] = matrix_[M_32];
			matrix[3][0] = matrix_[M_03]; matrix[3][1] = matrix_[M_13];
			matrix[3][2] = matrix_[M_23]; matrix[3][3] = matrix_[M_33];
		}
		else
		{
			matrix[0][0] = matrix_[M_00]; matrix[0][1] = matrix_[M_01];
			matrix[0][2] = matrix_[M_02]; matrix[0][3] = matrix_[M_03];
			matrix[1][0] = matrix_[M_10]; matrix[1][1] = matrix_[M_11];
			matrix[1][2] = matrix_[M_12]; matrix[1][3] = matrix_[M_13];
			matrix[2][0] = matrix_[M_20]; matrix[2][1] = matrix_[M_21];
			matrix[2][2] = matrix_[M_22]; matrix[2][3] = matrix_[M_23];
			matrix[3][0] = matrix_[M_30]; matrix[3][1] = matrix_[M_31];
			matrix[3][2] = matrix_[M_32]; matrix[3][3] = matrix_[M_33];
		}
	}

	/**
	 * Transforms the %Matrix4 to an array of 16 float values.
	 * @return The array of 16 float values.
	 */
	inline operator T *() { return &matrix_[0]; }

	/**
	 * Transforms the %Matrix4 to an array of 16 float values.
	 * @return The array of 16 float values.
	 */
	inline operator const T *() const { return &matrix_[0]; }

	/**
	 * Scales the %Matrix4 elements.
	 * @param s The scale factor.
	 */
	void scale(T s);

	/**
	 * Adds a matrix.
	 * @param m A %Matrix4 object.
	 */
	void add(const Matrix4 &m);

	/**
	 * Adds a scaled  %Matrix4.
	 * @param m A %Matrix4 object.
	 * @param scale A scale factor.
	 */
	void addScaled(const Matrix4 &m, T s);

	/**
	 * Negates the %Matrix4.
	 */
	void negate();

	/**
	 * Multiplies the matrix with another matrix.
	 * @param matrix A %Matrix4 object
	 */
	void multLeft(const Matrix4 &m);

	/**
	 * Multiplies the matrix with another matrix.
	 * @param matrix A %Matrix4 object
	 */
	void mult(const Matrix4 &m);

	/**
	 * Multiplies a vector with the matrix.
	 * @param v The vector.
	 */
	template <class Vec3fT>
	void multVec(Vec3fT &v)
	{
		Vec3fT r;
		r[0] =
			v[0] * matrix_[M_00] +
			v[1] * matrix_[M_10] +
			v[2] * matrix_[M_20];
		r[1] =
			v[0] * matrix_[M_01] +
			v[1] * matrix_[M_11] +
			v[2] * matrix_[M_21];
		r[2] =
			v[0] * matrix_[M_02] +
			v[1] * matrix_[M_12] +
			v[2] * matrix_[M_22];
		v = r;
	}

	/**
	 * Multiplies a point with the matrix.
	 * @param v The point.
	 */
	template <class Vec3fT>
	void multPnt(Vec3fT &v)
	{
		Vec3fT r;
		r[0] =
			v[0] * matrix_[M_00] +
			v[1] * matrix_[M_10] +
			v[2] * matrix_[M_20] +
			       matrix_[M_30];
		r[1] =
			v[0] * matrix_[M_01] +
			v[1] * matrix_[M_11] +
			v[2] * matrix_[M_21] +
			       matrix_[M_31];
		r[2] =
			v[0] * matrix_[M_02] +
			v[1] * matrix_[M_12] +
			v[2] * matrix_[M_22] +
			       matrix_[M_32];
		v = r;
	}

	/**
	 * Returns the infinity norm of the %Matrix4.
	 * @return The infinity norm.
	 */
	T normInfinity();

	/**
	 * Returns the determinat of the whole 4x4 matrix.
	 * @return The determinant.
	 */
	T det () const;

	/**
	 * Stores the inverse of the matrix into result.
	 * @param result Gets filled with the inverse of this matrix.
	 * @return true if the matrix is not singular.
	 */
	bool getInverse(Matrix4 &result) const;

	/**
	 * Sets this matrix to its square root, assumes det > 0.
	 * Source taken from OSGMatrix.
	 * @return Always true.
	 */
	bool sqrt();

	/**
	 * Sets this matrix to the exponential of the given matrix.
	 * Source taken from OSGMatrix.
	 * @param m The given matrix.
	*/
	void setExpOf(const Matrix4 &m);

	/**
	 * Sets this %Matrix4 to the logarithm of the given matrix,
	 * assumes det > 0.
	 * @param matrix The given matrix.
	*/
	bool setLogOf(const Matrix4 &matrix);

	/**
	 * Prints the matrix to an output stream.
	 * @param target The output stream.
	*/
	void print(std::ostream &target);

private:

	/** The components of the matrix. */
	T matrix_[16];
};


template <class T>
const T Matrix4<T>::epsilon = static_cast<T>(0.000001);


template <class T>
void Matrix4<T>::setIdentity()
{
	matrix_[M_01] = matrix_[M_02] = matrix_[M_03] = 0.f;
	matrix_[M_10] = matrix_[M_12] = matrix_[M_13] = 0.f;
	matrix_[M_20] = matrix_[M_21] = matrix_[M_23] = 0.f;
	matrix_[M_30] = matrix_[M_31] = matrix_[M_32] = 0.f;
	matrix_[M_00] = matrix_[M_11] = matrix_[M_22] = matrix_[M_33] = 1.f;
}


template <class T>
void Matrix4<T>::setTransform(T tx, T ty, T tz)
{
	setIdentity();
	setTranslation(tx, ty, tz);
}


template <class T>
void Matrix4<T>::setTransform(T qx, T qy, T qz, T qw)
{
	// setIdentity not needed here, because all elements will be set explicitely
	setTranslation(0.f, 0.f, 0.f);
	setRotationFromQuaternion(qx, qy, qz, qw);
	matrix_[M_30] = 0.f;
	matrix_[M_31] = 0.f;
	matrix_[M_32] = 0.f;
	matrix_[M_33] = 1.f;
}


template <class T>
void Matrix4<T>::setTransform(T tx, T ty, T tz, T qx, T qy, T qz, T qw)
{
	// setIdentity not needed here, because all elements will be set explicitely
	setRotationFromQuaternion(qx, qy, qz, qw);
	setTranslation(tx, ty, tz);
	matrix_[M_30] = 0.f;
	matrix_[M_31] = 0.f;
	matrix_[M_32] = 0.f;
	matrix_[M_33] = 1.f;
}


template <class T>
void Matrix4<T>::setTranslation(T tx, T ty, T tz)
{
	matrix_[M_03] = tx;
	matrix_[M_13] = ty;
	matrix_[M_23] = tz;
}


template <class T>
void Matrix4<T>::setRotationFromQuaternion(T qx, T qy, T qz, T qw)
{
	matrix_[M_00] = 1.0f - 2.0f * (qy * qy + qz * qz);
	matrix_[M_10] =        2.0f * (qx * qy + qz * qw);
	matrix_[M_20] =        2.0f * (qz * qx - qy * qw);
	matrix_[M_30] = 0.0f;

	matrix_[M_01] =        2.0f * (qx * qy - qz * qw);
	matrix_[M_11] = 1.0f - 2.0f * (qz * qz + qx * qx);
	matrix_[M_21] =        2.0f * (qy * qz + qx * qw);
	matrix_[M_31] = 0.0f;

	matrix_[M_02] =        2.0f * (qz * qx + qy * qw);
	matrix_[M_12] =        2.0f * (qy * qz - qx * qw);
	matrix_[M_22] = 1.0f - 2.0f * (qy * qy + qx * qx);
	matrix_[M_32] = 0.0f;

	matrix_[M_03] = 0.0f;
	matrix_[M_13] = 0.0f;
	matrix_[M_23] = 0.0f;
	matrix_[M_33] = 1.0f;
}


template <class T>
void Matrix4<T>::applyScale(T sx, T sy, T sz)
{
	matrix_[M_00] *= sx; matrix_[M_10] *= sx; matrix_[M_20] *=sx;
	matrix_[M_01] *= sy; matrix_[M_11] *= sy; matrix_[M_21] *=sy;
	matrix_[M_02] *= sz; matrix_[M_12] *= sz; matrix_[M_22] *=sz;
}


template <class T>
void Matrix4<T>::getTransform(T &tx, T &ty, T &tz, T &qx, T &qy, T &qz, T &qw, T &sx, T &sy, T &sz) const
{
	tx = matrix_[M_03];
	ty = matrix_[M_13];
	tz = matrix_[M_23];
	T r00 = matrix_[M_00], r01 = matrix_[M_01], r02 = matrix_[M_02];
	T r10 = matrix_[M_10], r11 = matrix_[M_11], r12 = matrix_[M_12];
	T r20 = matrix_[M_20], r21 = matrix_[M_21], r22 = matrix_[M_22];
	sx = sqrtf(r00 * r00 + r10 * r10 + r20 * r20);
	sy = sqrtf(r01 * r01 + r11 * r11 + r21 * r21);
	sz = sqrtf(r02 * r02 + r12 * r12 + r22 * r22);
	T sc = 1.f / sx;
	r00 *= sc; r10 *= sc; r20 *= sc;
	sc = 1.f / sy;
	r01 *= sc; r11 *= sc; r21 *= sc;
	sc = 1.f / sz;
	r02 *= sc; r12 *= sc; r22 *= sc;
	T trace = r00 + r11 + r22;
	if (trace > epsilon)
	{
		T sf = 0.5f / sqrtf(trace + 1.f);
		qw = 0.25f / sf;
		qx = (r21 - r12) * sf;
		qy = (r02 - r20) * sf;
		qz = (r10 - r01) * sf;
	}
	else
	{
		if ((r00 > r11) && (r00 > r22))
		{
			T sf = 0.5f / sqrtf(1.f + r00 - r11 - r22);
			qx = 0.25f / sf;
			qy = (r01 + r10) * sf;
			qz = (r02 + r20) * sf;
			qw = (r12 - r21) * sf;
		}
		else if (r11 > r22)
		{
			T sf = 0.5f / sqrtf(1.f + r11 - r00 - r22);
			qx = (r01 + r10) * sf;
			qy = 0.25f / sf;
			qz = (r12 + r21) * sf;
			qw = (r02 - r20) * sf;
		}
		else
		{
			T sf = 0.5f / sqrtf(1.f + r22 - r00 - r11);
			qx = (r02 + r20) * sf;
			qy = (r12 + r21) * sf;
			qz = 0.25f / sf;
			qw = (r01 - r10) * sf;
		}
	}
}


template <class T>
void Matrix4<T>::getTransform(T &tx, T &ty, T &tz, T &qx, T &qy, T &qz, T &qw) const
{
	if (
		(fabs(1.f - (matrix_[M_00] * matrix_[M_00] + matrix_[M_01] * matrix_[M_01] + matrix_[M_02] * matrix_[M_02])) > epsilon) ||
		(fabs(1.f - (matrix_[M_10] * matrix_[M_10] + matrix_[M_11] * matrix_[M_11] + matrix_[M_12] * matrix_[M_12])) > epsilon) ||
		(fabs(1.f - (matrix_[M_20] * matrix_[M_20] + matrix_[M_21] * matrix_[M_21] + matrix_[M_22] * matrix_[M_22])) > epsilon))
	{
		// scaling detected
		//cout << "scaling detected" << endl;
		T sx, sy, sz;
		getTransform(tx, ty, tz, qx, qy, qz, qw, sx, sy, sz);
		return;
	}
	tx = matrix_[M_03];
	ty = matrix_[M_13];
	tz = matrix_[M_23];
	T trace = matrix_[M_00] + matrix_[M_11] + matrix_[M_22];
	if (trace > epsilon)
	{
		T sf = 0.5f / sqrtf(trace + 1.f);
		qw = 0.25f / sf;
		qx = (matrix_[M_21] - matrix_[M_12]) * sf;
		qy = (matrix_[M_02] - matrix_[M_20]) * sf;
		qz = (matrix_[M_10] - matrix_[M_01]) * sf;
	}
	else
	{
		if ((matrix_[M_00] > matrix_[M_11]) && (matrix_[M_00] > matrix_[M_22]))
		{
			T sf = 0.5f / sqrtf(1.f + matrix_[M_00] - matrix_[M_11] - matrix_[M_22]);
			qx = 0.25f / sf;
			qy = (matrix_[M_01] + matrix_[M_10]) * sf;
			qz = (matrix_[M_02] + matrix_[M_20]) * sf;
			qw = (matrix_[M_12] - matrix_[M_21]) * sf;
		}
		else if (matrix_[M_11] > matrix_[M_22])
		{
			T sf = 1.f / sqrtf(1.f + matrix_[M_11] - matrix_[M_00] - matrix_[M_22]);
			qx = (matrix_[M_01] + matrix_[M_10]) * sf;
			qy = 0.25f / sf;
			qz = (matrix_[M_12] + matrix_[M_21]) * sf;
			qw = (matrix_[M_02] - matrix_[M_20]) * sf;
		}
		else
		{
			T sf = 0.5f / sqrtf(1.f + matrix_[M_22] - matrix_[M_00] - matrix_[M_11]);
			qx = (matrix_[M_02] + matrix_[M_20]) * sf;
			qy = (matrix_[M_12] + matrix_[M_21]) * sf;
			qz = 0.25f / sf;
			qw = (matrix_[M_01] - matrix_[M_10]) * sf;
		}
	}
}


template <class T>
void Matrix4<T>::scale(T s)
{
	for (int i = 0; i < 16; ++i)
		matrix_[i] *= s;
}


template <class T>
void Matrix4<T>::add(const Matrix4 &m)
{
	for (int i = 0; i < 16; ++i)
		matrix_[i] += m.matrix_[i];
}


template <class T>
void Matrix4<T>::addScaled(const Matrix4 &m, T s)
{
	for (int i = 0; i < 16; ++i)
		matrix_[i] += s * m.matrix_[i];
}


template <class T>
void Matrix4<T>::negate()
{
	for (int i = 0; i < 16; ++i)
		matrix_[i] *= -1.f;
}


template <class T>
void Matrix4<T>::multLeft(const Matrix4 &m)
{
	/*TODO */
	Matrix4 tmp;
	for (int r = 0; r < 16; r += 4)
		for (int c = 0; c < 4; ++c)
			tmp.matrix_[r+c] = m.matrix_[r    ] * matrix_[     c] +
			                   m.matrix_[r + 1] * matrix_[ 4 + c] +
			                   m.matrix_[r + 2] * matrix_[ 8 + c] +
			                   m.matrix_[r + 3] * matrix_[12 + c];
	for (int i = 0; i < 16; ++i)
		matrix_[i] = tmp.matrix_[i];
}


template <class T>
void Matrix4<T>::mult(const Matrix4 &m)
{
	Matrix4 tmp;
	for (int r = 0; r < 16; r += 4)
		for (int c = 0; c < 4; ++c)
			tmp.matrix_[r + c] = matrix_[r    ] * m.matrix_[     c] +
			                     matrix_[r + 1] * m.matrix_[ 4 + c] +
			                     matrix_[r + 2] * m.matrix_[ 8 + c] +
			                     matrix_[r + 3] * m.matrix_[12 + c];
	for (int i = 0; i < 16; ++i)
		matrix_[i] = tmp.matrix_[i];
}


template <class T>
T Matrix4<T>::normInfinity()
{
	T t, n = 0.0f;
	for (int i = 0; i < 16; ++i)
	{
		t = fabs(matrix_[i]);
		if (t > n)
			n = t;
	}
	return n;
}


template <class T>
static inline T det3(T a1, T a2, T a3, T b1, T b2, T b3, T c1, T c2, T c3)
{
	return
		(a1 * b2 * c3) + (a2 * b3 * c1) + (a3 * b1 * c2) -
		(a1 * b3 * c2) - (a2 * b1 * c3) - (a3 * b2 * c1);
}


template <class T>
T Matrix4<T>::det() const
{
	T a1, a2, a3, a4,
	  b1, b2, b3, b4,
	  c1, c2, c3, c4,
	  d1, d2, d3, d4;

	// changed osg [x][y] -> M_yx !!
	a1 = matrix_[M_00];
	b1 = matrix_[M_01];
	c1 = matrix_[M_02];
	d1 = matrix_[M_03];

	a2 = matrix_[M_10];
	b2 = matrix_[M_11];
	c2 = matrix_[M_12];
	d2 = matrix_[M_13];

	a3 = matrix_[M_20];
	b3 = matrix_[M_21];
	c3 = matrix_[M_22];
	d3 = matrix_[M_23];

	a4 = matrix_[M_30];
	b4 = matrix_[M_31];
	c4 = matrix_[M_32];
	d4 = matrix_[M_33];

	return (a1 * det3(b2, b3, b4, c2, c3, c4, d2, d3, d4) -
	        b1 * det3(a2, a3, a4, c2, c3, c4, d2, d3, d4) +
	        c1 * det3(a2, a3, a4, b2, b3, b4, d2, d3, d4) -
	        d1 * det3(a2, a3, a4, b2, b3, b4, c2, c3, c4));
}


template <class T>
bool Matrix4<T>::getInverse(Matrix4 &result) const
{
	T rDet;

	T a1, a2, a3, a4,
	  b1, b2, b3, b4,
	  c1, c2, c3, c4,
	  d1, d2, d3, d4;

	// changed osg [x][y] -> M_yx !!
	a1 = matrix_[M_00];
	b1 = matrix_[M_01];
	c1 = matrix_[M_02];
	d1 = matrix_[M_03];

	a2 = matrix_[M_10];
	b2 = matrix_[M_11];
	c2 = matrix_[M_12];
	d2 = matrix_[M_13];

	a3 = matrix_[M_20];
	b3 = matrix_[M_21];
	c3 = matrix_[M_22];
	d3 = matrix_[M_23];

	a4 = matrix_[M_30];
	b4 = matrix_[M_31];
	c4 = matrix_[M_32];
	d4 = matrix_[M_33];

	rDet = det();

	if (fabs(rDet) < 1E-30)
	{
		result.setIdentity();
		return false;
	}

	rDet = 1.f / rDet;

	// changed osg [x][y] -> M_yx !!
	result[M_00] =   det3(b2, b3, b4, c2, c3, c4, d2, d3, d4) * rDet;
	result[M_10] = - det3(a2, a3, a4, c2, c3, c4, d2, d3, d4) * rDet;
	result[M_20] =   det3(a2, a3, a4, b2, b3, b4, d2, d3, d4) * rDet;
	result[M_30] = - det3(a2, a3, a4, b2, b3, b4, c2, c3, c4) * rDet;

	result[M_01] = - det3(b1, b3, b4, c1, c3, c4, d1, d3, d4) * rDet;
	result[M_11] =   det3(a1, a3, a4, c1, c3, c4, d1, d3, d4) * rDet;
	result[M_21] = - det3(a1, a3, a4, b1, b3, b4, d1, d3, d4) * rDet;
	result[M_31] =   det3(a1, a3, a4, b1, b3, b4, c1, c3, c4) * rDet;

	result[M_02] =   det3(b1, b2, b4, c1, c2, c4, d1, d2, d4) * rDet;
	result[M_12] = - det3(a1, a2, a4, c1, c2, c4, d1, d2, d4) * rDet;
	result[M_22] =   det3(a1, a2, a4, b1, b2, b4, d1, d2, d4) * rDet;
	result[M_32] = - det3(a1, a2, a4, b1, b2, b4, c1, c2, c4) * rDet;

	result[M_03] = - det3(b1, b2, b3, c1, c2, c3, d1, d2, d3) * rDet;
	result[M_13] =   det3(a1, a2, a3, c1, c2, c3, d1, d2, d3) * rDet;
	result[M_23] = - det3(a1, a2, a3, b1, b2, b3, d1, d2, d3) * rDet;
	result[M_33] =   det3(a1, a2, a3, b1, b2, b3, c1, c2, c3) * rDet;

	return true;
}


template <class T>
bool Matrix4<T>::sqrt()
{
	Matrix4 iX;
	Matrix4  Y;
	Matrix4 iY;
	T  g;
	T ig;
	Y.setIdentity();
	for (int i = 0; i < 6; ++i)
	{
		getInverse(iX);

		Y.getInverse(iY);

		g = fabsf(powf(det() * Y.det(), -0.125f));
		ig = 1.0f / g;

		scale    (g     );
		addScaled(iY, ig);
		scale    (0.5f   );

		Y.scale    (g     );
		Y.addScaled(iX, ig);
		Y.scale    (0.5f   );
	}
	// ToDo: return should depend on achieved accuracy
	return true;
}


template <class T>
void Matrix4<T>::setExpOf(const Matrix4& m)
{
	const int q = 6;

	Matrix4 A(m);
	Matrix4 D;
	Matrix4 N;

	int j = 1;
	int k;

	T c = 1.0;

	j += int(logf(A.normInfinity() / 0.693f));

	if (j < 0)
		j = 0;

	A.scale(1.0f / T(1 << j));

	setIdentity();

	for (k = 1; k <= q; ++k)
	{
		c *= T(q - k + 1) / T(k * (2 *q - k + 1));

		multLeft(A);

		N.addScaled(*this, c);

		if (k % 2)
			D.addScaled(*this, -c);
		else
			D.addScaled(*this,  c);
	}

	//    invertFrom(D);
	D.getInverse(*this);
	mult(N);
	for (k = 0; k < j; ++k)
		mult(*this);
}


template <class T>
bool Matrix4<T>::setLogOf(const Matrix4 &matrix)
{
	const int maxiter = 12;
	int k = 0;
	int i = 0;
	const T eps = 1e-12f;
	Matrix4 A(matrix),Z;

	// Take repeated square roots to reduce spectral radius
	//Z.setValue(A);
	Z=A;

	Z[M_00] -= 1.f;
	Z[M_11] -= 1.f;
	Z[M_22] -= 1.f;
	Z[M_33] -= 1.f;

	while (Z.normInfinity() > 0.5f)
	{
		A.sqrt();

		//Z.setValue(A);
		Z=A;
		Z[M_00] -= 1.f;
		Z[M_11] -= 1.f;
		Z[M_22] -= 1.f;
		Z[M_33] -= 1.f;
		++k;
	}

	A[M_00] -= 1.f;
	A[M_11] -= 1.f;
	A[M_22] -= 1.f;
	A[M_33] -= 1.f;
	A.negate();
	//setValue(A);
	*this=A;
	//Z.setValue(A);
	Z=A;
	i = 1;
	while (Z.normInfinity() > eps && i < maxiter)
	{
		Z.mult(A);
		++i;
		addScaled(Z, 1.f / T(i));
	}

	scale(-1.0f * T(1 << k));

	return (i < maxiter);
}


template <class T>
void Matrix4<T>::print(std::ostream &target)
{
	target << matrix_[M_00] << ' ' << matrix_[M_01] << ' ' << matrix_[M_02] << ' ' << matrix_[M_03] << '\n'
	       << matrix_[M_10] << ' ' << matrix_[M_11] << ' ' << matrix_[M_12] << ' ' << matrix_[M_13] << '\n'
	       << matrix_[M_20] << ' ' << matrix_[M_21] << ' ' << matrix_[M_22] << ' ' << matrix_[M_23] << '\n'
	       << matrix_[M_30] << ' ' << matrix_[M_31] << ' ' << matrix_[M_32] << ' ' << matrix_[M_33] << std::endl;
}


typedef Matrix4<double> Matrix4d;


typedef Matrix4<float> Matrix4f;


/**
 * Helper class that transforms the Matrix4d data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<Matrix4d>
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
 * Helper class that transforms the Matrix4f data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<Matrix4f>
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
std::ostream &operator <<(std::ostream &os, const Matrix4<T> &obj)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
			os << obj[j * 4 + i] << ' ';
		os << std::endl;
	}
	return os;
}


template <class T>
std::istream &operator >>(std::istream &is, Matrix4<T> &obj)
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if (is.good())
				is >> obj[j * 4 + i];
	return is;
}


} // namespace InstantIO


#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif // __INSTANTIO_MATRIX4_H
