#ifndef __INSTANTIO_ROTATION_H
#define __INSTANTIO_ROTATION_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Rotation.h                                                      //
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


template <class T> class Vec3;
typedef Vec3<float> Vec3f;


/**
 * @class Rotation Rotation.h InstantIO/Rotation.h
 *
 * Helper class for exchanging Rotations/Orientations.
 * %Rotation is a basic helper class for exchanging rotations resp.
 * orientations between different software components. It does
 * not contain any means for rotation arithmetics, only methods for
 * setting and getting the rotation. It is not meant to be used
 * directly by software components for rotation representation. Instead,
 * software components should use their own, appropriate classes for
 * handling rotations. Only when sending rotations to an OutSlot, or when
 * receiving rotations from an InSlot, the internal representation of
 * rotations should be converted to Rotation's. This ensures the
 * interoperability between different software components that use
 * different internal representations for rotations.
 * <p>
 * Rotations are internally stored as quaternions. You can access
 * these quaternions directly, or you can use methods that convert
 * a rotation axis and a rotation angle to and from the quaternions.
 *
 * @author Patrick D&auml;hne
 */
class INSTANTIO_DLLMAPPING Rotation
{
public:

	/** Creates a new %Rotation object */
	inline Rotation(): quat_()
	{ quat_[0] = quat_[1] = quat_[2] = quat_[3] = 0.f; }

	/**
	 * Constructor that initializes the %Rotation object with the
	 * given quaternions.
	 * @param q1 The first quaternion
	 * @param q2 The second quaternion
	 * @param q3 The third quaternion
	 * @param q4 The fourth quaternion
	 */
	inline Rotation(float q1, float q2, float q3, float q4): quat_()
	{ quat_[0] = q1; quat_[1] = q2; quat_[2] = q3; quat_[3] = q4; }

	/**
	 * Constructor that initializes the %Rotation object with the values
	 * of a given float array.
	 * @param quat An array of at least four float values. The
	 * first four values must contain the quaternions that
	 * specify the rotation.
	 */
	inline Rotation(const float *quat): quat_()
	{
		quat_[0] = quat[0]; quat_[1] = quat[1];
		quat_[2] = quat[2]; quat_[3] = quat[3];
	}

	/**
	 * Constructor that initializes the %Rotation object with another
	 * %Rotation object.
	 * @param rot The other %Rotation object used to initialize
	 * this %Rotation object.
	 */
	inline Rotation(const Rotation &rot): quat_()
	{
		quat_[0] = rot.quat_[0]; quat_[1] = rot.quat_[1];
		quat_[2] = rot.quat_[2]; quat_[3] = rot.quat_[3];
	}

	/**
	 * Constructor that initializes the %Rotation object with
	 * a rotation axis and a rotation angle.
	 * @param axis The rotation axis
	 * @param angle The rotation angle in radians
	 */
	inline Rotation(const Vec3f &axis, float angle): quat_()
	{ set(axis, angle); }

	/**
	 * Assigns the components of another %Rotation object to
	 * this object's components.
	 * @param rot The other %Rotation object
	 * @return The other %Rotation object
	 */
	inline const Rotation &operator=(const Rotation &rot)
	{
		quat_[0] = rot.quat_[0]; quat_[1] = rot.quat_[1];
		quat_[2] = rot.quat_[2]; quat_[3] = rot.quat_[3];
		return rot;
	}

	/**
	 * Assigns an array of floats to this object's components.
	 * @param quat An array of at least four float values. The
	 * first four values must contain the quaternions that
	 * specify the rotation.
	 * @return The array of floats
	 */
	inline const float *operator=(const float *quat)
	{
		quat_[0] = quat[0]; quat_[1] = quat[1];
		quat_[2] = quat[2]; quat_[3] = quat[3];
		return quat;
	}

	/**
	 * Sets the first quaternion of the rotation.
	 * @param q1 The new first quaternion.
	 */
	inline void setQ1(float q1) { quat_[0] = q1; }

	/**
	 * Returns the first quaternion of the rotation.
	 * @return The first quaternion.
	 */
	inline float getQ1() const { return quat_[0]; }

	/**
	 * Sets the second quaternion of the rotation.
	 * @param q2 The new second quaternion.
	 */
	inline void setQ2(float q2) { quat_[1] = q2; }

	/**
	 * Returns the second quaternion of the rotation.
	 * @return The second quaternion.
	 */
	inline float getQ2() const { return quat_[1]; }

	/**
	 * Sets the third quaternion of the rotation.
	 * @param q3 The new third quaternion.
	 */
	inline void setQ3(float q3) { quat_[2] = q3; }

	/**
	 * Returns the third quaternion of the rotation.
	 * @return The third quaternion.
	 */
	inline float getQ3() const { return quat_[2]; }

	/**
	 * Sets the fourth quaternion of the rotation.
	 * @param q4 The new fourth quaternion.
	 */
	inline void setQ4(float q4) { quat_[3] = q4; }

	/**
	 * Returns the fourth quaternion of the rotation.
	 * @return The fourth quaternion.
	 */
	inline float getQ4() const { return quat_[3]; }

	/**
	 * Sets the rotation by setting the four quaternions.
	 * @param q1 The first quaternion
	 * @param q2 The second quaternion
	 * @param q3 The third quaternion
	 * @param q4 The fourth quaternion
	 */
	inline void set(float q1, float q2, float q3, float q4)
	{ quat_[0] = q1; quat_[1] = q2; quat_[2] = q3; quat_[3] = q4; }

	/**
	 * Sets the rotation with the values of a given float array.
	 * @param quat An array of at least four float values. The
	 * first four values must contain the quaternions that
	 * specify the rotation.
	 */
	inline void set(const float *quat)
	{
			quat_[0] = quat[0];
			quat_[1] = quat[1];
			quat_[2] = quat[2];
			quat_[3] = quat[3];
	}

	/**
	 * Sets the rotation to the rotation of another
	 * %Rotation object.
	 * @param rot The other %Rotation object.
	 */
	inline void set(const Rotation &rot)
	{
		quat_[0] = rot.quat_[0];
		quat_[1] = rot.quat_[1];
		quat_[2] = rot.quat_[2];
		quat_[3] = rot.quat_[3];
	}

	/**
	 * Sets the rotation by a rotation axis and a
	 * rotation angle.
	 * @param axis The rotation axis
	 * @param angle The rotation angle in radians
	 */
	void set(const Vec3f &axis, float angle);

	/**
	 * Returns the four components of the quaternion
	 * representation of the rotation.
	 * @param q1 Returns the first component
	 * @param q2 Returns the second component
	 * @param q3 Returns the third component
	 * @param q4 Returns the fourth component
	 */
	inline void get(float &q1, float &q2, float &q3, float &q4) const
	{ q1 = quat_[0]; q2 = quat_[1]; q3 = quat_[2]; q4 = quat_[3]; }

	/**
	 * Returns the quaternions of the rotation.
	 * @param quat An array of at least four float values
	 * that gets filled with the quaternions of the
	 * rotation.
	 */
	inline void get(float *quat) const
	{
		quat[0] = quat_[0];
		quat[1] = quat_[1];
		quat[2] = quat_[2];
		quat[3] = quat_[3];
	}

	/**
	 * Returns the rotation.
	 * @param rot The %Rotation object that is set to
	 * the rotation of this %Rotation object.
	 */
	inline void get(Rotation &rot) const
	{
		rot.quat_[0] = quat_[0];
		rot.quat_[1] = quat_[1];
		rot.quat_[2] = quat_[2];
		rot.quat_[3] = quat_[3];
	}
	
	/**
	 * Returns the rotation axis and the rotation angle.
	 * @param axis The vector that gets filled with the
	 * rotation axis.
	 * @return The rotation angle in radians.
	 */
	float get(Vec3f &axis) const;

	/**
	 * Returns the quaternions of the rotation.
	 * @return An array of four float elements that contain
	 * the quaternions of the rotation.
	 */
	inline const float *get() const { return quat_; }
	
	/**
	 * Returns the quaternions of the rotation.
	 * @return An array of four float elements that contain
	 * the quaternions of the rotation.
	 */
	inline float *get() { return quat_; }

	/**
	 * Get a component of the quaternion representation
	 * of the rotation by an index.
	 * @param i The index
	 * @return The component
	 */
	inline float operator[](int i) const { return quat_[i]; }

	/**
	 * Get a component of the quaternion representation
	 * of the rotation by an index.
	 * @param i The index
	 * @return The component
	 */
	inline float &operator[](int i) { return quat_[i]; }

	/**
	 * Transforms the rotation to an array of four
	 * float values that contain the four components
	 * of the quaternion representation of the rotation.
	 * @return The four components
	 */
	inline operator float *() { return &quat_[0]; }

	/**
	 * Transforms the rotation to an array of four
	 * float values that contain the four components
	 * of the quaternion representation of the rotation.
	 * @return The four components
	 */
	inline operator const float *() const { return &quat_[0]; }

	/**
	 * Multiplies the rotation by another rotation
	 * to get the product of both rotations.
	 * @param rot The other rotation
	 * @return Itself
	 */
	Rotation &operator*=(const Rotation &rot);

private:

	/** The four quaternions of the rotation. */
	float quat_[4];

};


/**
 * Helper class that transforms the Rotation data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Patrick D&auml;hne
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<Rotation>
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

#endif // __INSTANTIO_ROTATION_H
