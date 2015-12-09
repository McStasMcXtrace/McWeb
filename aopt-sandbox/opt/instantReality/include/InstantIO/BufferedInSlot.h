#ifndef __INSTANTIO_BUFFEREDINSLOT_H
#define __INSTANTIO_BUFFEREDINSLOT_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: BufferedInSlot.h                                                //
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
#include "InSlot.h"
#include "Data.h"
#include "InterruptedException.h"
#include <string>
#include <vector>


namespace InstantIO
{


/**
 * @class BufferedInSlot BufferedInSlot.h InstantIO/BufferedInSlot.h
 *
 * InSlot that buffers values. Usually, InSlots do not buffer
 * values, i.e. when a new data value is written into the InSlot
 * before the old data value has been received by the application,
 * the old data value gets lost. This is ok under many circumstances,
 * but sometimes it is important to get all data values (e.g. when
 * receiving button press and release event). For this reason, the
 * %BufferedInSlot class exists.
 * <p>
 * When you create a %BufferedInSlot object, you have to specify
 * the maximum number of data values that get buffered by the
 * InSlot. When more values are written into the InSlot than the
 * maximum number, the newer data values overwrite the oldest data
 * values stored in the InSlot.
 *
 * @author Patrick D&auml;hne
 * @see InSlot InSlot
 */
template <class T> class BufferedInSlot: public InSlot<T>
{
public:

	/**
	 * Creates a new %BufferedInSlot object.
	 * @param numValues The maximum number of values that are buffered
	 * by this InSlot.
	 */
	inline explicit BufferedInSlot(unsigned int numValues = 1)
	: InSlot<T>(), buffer_(numValues), readPos_(buffer_.begin()), writePos_(buffer_.begin()), numValues_(0)
	{}

	/**
	 * Creates a new %BufferedInSlot object.
	 * @param description The description of this %BufferedInSlot. This
	 * information is not used by the InstantIO system directly, instead it is
	 * displayed for information purposes in user interfaces used to configure
	 * the InstantIO system.
	 * @param numValues The maximum number of values that are buffered
	 * by this InSlot.
	 */
	inline explicit BufferedInSlot(const std::string &description, unsigned int numValues = 1)
	: InSlot<T>(description), buffer_(numValues),
	  readPos_(buffer_.begin()), writePos_(buffer_.begin()), numValues_(0)
	{}

	/**
	 * Destroys the %BufferedInSlot object.
	 */
	virtual ~BufferedInSlot() {};

	/**
	 * Returns if data is available on the %BufferedInSlot.
	 * All method that receive data values ({@link #top top}, {@link
	 * #pop pop}) block when no data is available until data is received
	 * from an OutSlot. By using this method, you can prevent this blocking
	 * by checking if data is actually available.
	 * @return false when data is available, true otherwise.
	 */
	virtual bool empty() const
	{
		this->lock();
		bool result = numValues_ == 0;
		this->unlock();
		return result;
	};

	/**
	 * Returns the number of unread data values available
	 * on the %BufferedInSlot. This method returns the number
	 * of times the application may call the #pop method
	 * without blocking.
	 * @return The number of unread data values.
	 * @see #pop
	 */
	inline unsigned int getNumValues() const
	{
		this->lock();
		unsigned int result = numValues_;
		this->unlock();
		return result;
	}

	/**
	 * Returns the current data value without removing it from
	 * the %BufferedInSlot.
	 * <p>
	 * To remove the data value from the %BufferedInSlot, you can
	 * call the {@link #pop pop} method.
	 * @return The current data value.
	 * @throw InterruptedException when the interrupt method has
	 * been called
	 * @see #pop
	 */
	virtual const Data<T> top()
	/*throw (InterruptedException)*/
	{
		this->lock();
		if (this->interrupted_ == true)
		{
			this->interrupted_ = false;
			this->unlock();
			throw InterruptedException();
		}
		Data<T> value = *readPos_;
		this->unlock();
		return value;
	};

	/**
	 * Removes the current data value from the %BufferedInSlot
	 * and returns it.
	 * <p>
	 * This method blocks when no data is available until data
	 * is received from an OutSlot. By using the
	 * {@link #empty empty} or #getNumValues methods you can check
	 * if data is available before calling this method.
	 * <p>
	 * To get the data value without removing it from the
	 * %BufferedInSlot, use the {@link #top top} method.
	 *
	 * @return The current data value.
	 * @throw InterruptedException when the interrupt method has
	 * been called
	 * @see #empty
	 * @see #getNumValues
	 * @see #top
	 */
	virtual const Data<T> pop()
	/*throw (InterruptedException)*/
	{
		this->down();
		this->lock();
		if (this->interrupted_ == true)
		{
			this->interrupted_ = false;
			this->unlock();
			throw InterruptedException();
		}
		Data<T> value = *readPos_;
		incrementPos(readPos_);
		--numValues_;
		this->unlock();
		return value;
	};

	/**
	 * Writes a new data value into the %BufferedInSlot. Usually, you
	 * do not write data values directly into a %BufferedInSlot.
	 * InSlots receive data values from {@link OutSlot OutSlots} they are
	 * connected to. This method is available for rare special
	 * circumstances where it might be helpful to write data values
	 * manually into the %BufferedInSlot.
	 * @param data A Data object that contains a new data value and
	 * the time when it has been created.
	 */
	virtual void push(const Data<T> &data)
	{
		this->lock();
		if ((writePos_ == readPos_) && (numValues_ > 0))
			incrementPos(readPos_);
		else
		{
			++numValues_;
			this->up();
		}
		*writePos_ = data;
		incrementPos(writePos_);
		this->unlock();
		this->newData();
	};

private:

	/** Defines the vector of data values */
	typedef std::vector<Data<T> > DataVector;

	/** Vector of buffered data values */
	// protected by mutex_
	DataVector buffer_;

	/** Pointer to the read position in the value buffer */
	// protected by mutex_
	typename DataVector::iterator readPos_;

	/** Pointer to the write position in the value buffer */
	// protected by mutex_
	typename DataVector::iterator writePos_;

	/** The number of data values available on this %BufferedInSlot */
	// protected by mutex_
	unsigned int numValues_;

	/**
	 * Helper function used to increment a pointer into the buffer.
	 * When the pointer points to the end of the buffer, it
	 * gets set to the start of the buffer.
	 * @param pos The pointer
	 */
	inline void incrementPos(typename DataVector::iterator &pos)
	{
		// mutex_ must be locked before calling this method!
		if (++pos == buffer_.end())
			pos = buffer_.begin();
	}

	/** Copy constructor (not implemented!) */
	BufferedInSlot(const BufferedInSlot<T> &);

	/** Copy operator (not implemented!) */
	const BufferedInSlot<T> &operator=(const BufferedInSlot<T> &);
};


} // namespace InstantIO


#endif //__INSTANTIO_BUFFEREDINSLOT_H
