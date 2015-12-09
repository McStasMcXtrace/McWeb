#ifndef __INSTANTIO_MFTYPES_H
#define __INSTANTIO_MFTYPES_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: MFTypes.h                                                       //
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
#include "SmartPtr.h"
#include "TypeName.h"
#include "Color.h"
#include "ColorRGBA.h"
#include "Image.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Rotation.h"
#include "Time.h"
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#include <vector>
#include <string>


namespace InstantIO
{


typedef std::vector<bool> MFBool;
typedef std::vector<Color> MFColor;
typedef std::vector<ColorRGBA> MFColorRGBA;
typedef std::vector<double> MFDouble;
typedef std::vector<float> MFFloat;
typedef std::vector<Image> MFImage;
typedef std::vector<long> MFInt32;
typedef std::vector<Matrix3d> MFMatrix3d;
typedef std::vector<Matrix3f> MFMatrix3f;
typedef std::vector<Matrix4d> MFMatrix4d;
typedef std::vector<Matrix4f> MFMatrix4f;
typedef std::vector<Rotation> MFRotation;
typedef std::vector<std::string> MFString;
typedef std::vector<Time> MFTime;
typedef std::vector<Vec2d> MFVec2d;
typedef std::vector<Vec2f> MFVec2f;
typedef std::vector<Vec3d> MFVec3d;
typedef std::vector<Vec3f> MFVec3f;
typedef std::vector<Vec4d> MFVec4d;
typedef std::vector<Vec4f> MFVec4f;


typedef SmartPtr<MFBool> MFBoolPtr;
typedef SmartPtr<MFColor> MFColorPtr;
typedef SmartPtr<MFColorRGBA> MFColorRGBAPtr;
typedef SmartPtr<MFDouble> MFDoublePtr;
typedef SmartPtr<MFFloat> MFFloatPtr;
typedef SmartPtr<MFImage> MFImagePtr;
typedef SmartPtr<MFInt32> MFInt32Ptr;
typedef SmartPtr<MFMatrix3d> MFMatrix3dPtr;
typedef SmartPtr<MFMatrix3f> MFMatrix3fPtr;
typedef SmartPtr<MFMatrix4d> MFMatrix4dPtr;
typedef SmartPtr<MFMatrix4f> MFMatrix4fPtr;
typedef SmartPtr<MFRotation> MFRotationPtr;
typedef SmartPtr<MFString> MFStringPtr;
typedef SmartPtr<MFTime> MFTimePtr;
typedef SmartPtr<MFVec2d> MFVec2dPtr;
typedef SmartPtr<MFVec2f> MFVec2fPtr;
typedef SmartPtr<MFVec3d> MFVec3dPtr;
typedef SmartPtr<MFVec3f> MFVec3fPtr;
typedef SmartPtr<MFVec4d> MFVec4dPtr;
typedef SmartPtr<MFVec4f> MFVec4fPtr;


/**
 * Helper class that transforms the MFBool data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFBool>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFBoolPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFColor data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFColor>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFColorPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFColorRGBA data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFColorRGBA>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFColorRGBAPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFDouble data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFDouble>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFDoublePtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFFloat data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFFloat>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFFloatPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


template <>
class INSTANTIO_DLLMAPPING TypeName<MFImagePtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFInt32 data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFInt32>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFInt32Ptr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFMatrix3d data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFMatrix3d>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFMatrix3dPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFMatrix3f data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFMatrix3f>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFMatrix3fPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFMatrix4d data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFMatrix4d>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFMatrix4dPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFMatrix4f data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFMatrix4f>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFMatrix4fPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFRotation data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFRotation>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFRotationPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFString data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFString>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFStringPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFTime data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFTime>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFTimePtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFVec2d data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec2d>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec2dPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFVec2f data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec2f>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec2fPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFVec3d data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec3d>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec3dPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFVec3f data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec3f>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec3fPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFVec4d data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec4d>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec4dPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


/**
 * Helper class that transforms the MFVec4f data type
 * to a human-readable string. This class is used
 * internally by the InstantIO library.
 * @author Helmut Seibert
 */
template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec4f>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};

template <>
class INSTANTIO_DLLMAPPING TypeName<MFVec4fPtr>
{
public:

	/**
	 * Returns the type as a human-readable string.
	 * @return The type as a human-readable string.
	 */
	static const char *getName();
};


} // namespace InstantIO


#endif // __INSTANTIO_MFTYPES_H
