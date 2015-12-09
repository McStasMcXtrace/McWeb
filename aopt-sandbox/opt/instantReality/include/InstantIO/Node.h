#ifndef __INSTANTIO_NODE_H
#define __INSTANTIO_NODE_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Node.h                                                          //
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
#include "Namespace.h"
#include "BasicOutSlot.h"
#include "BasicInSlot.h"
#include <string>
#include <vector>
#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4251)
#endif


namespace InstantIO
{


class NodeType;


/**
 * @class Node Node.h InstantIO/Node.h
 *
 * %Node class
 */
class INSTANTIO_DLLMAPPING Node: public Namespace, public BasicOutSlot::Listener, public BasicInSlot::Listener
{
public:

	/// Enumeration of the different states of the Node
	enum State { NODE_RUNNING, NODE_SLEEPING, NODE_ERROR, NODE_DISABLED };

	/// Constructor
	/**
	 * Creates a new Node object.
	 */
	Node();

	/// Destructor
	/**
	 * Destroys a Node object.
	 */
	virtual ~Node();

	virtual std::string typeName() const;

	/// Returns the State of the Node object
	/**
	 * Returns the State of the Node object.
	 * \return The State
	 */
	State getState() const;

	/// Returns a short information string
	/**
	 * Returns a short information string.
	 * \return The short information string
	 */
	std::string getShortInfo() const;

	/**
	 * Returns a detailed information string.
	 * @return A string containing detailed information.
	 */
	virtual std::string getDetailedInfo() const;

	/** Defines the log vector */
	typedef std::vector<std::string> LogVector;

	/**
	 * Returns the entries of the log.
	 * @param entries The vector that gets filled with
	 * the entries of the log.
	 */
	void getLog(LogVector &entries) const;

	/**
	 * Sets the value of a field.
	 * @param name The name of the field.
	 * @param value The new value of the field.
	 */
	virtual void setFieldValue(const std::string &name, const std::string &value);

	/**
	 * Returns the value of a field.
	 * @param name The name of the field.
	 * @return The value of the field.
	 */
	virtual std::string getFieldValue(const std::string &name) const
	/* throw (InvalidFieldException)*/;

	/**
	 * Returns the names of all fields.
	 * @param names A vector that gets filled with the names.
	 */
	virtual void getFieldNames(std::vector<std::string> &names) const;

	/// Returns the type of the Node
	/**
	 * Returns the type of the Node.
	 * \return The type of the Node
	 */
	virtual NodeType *type() const;

protected:

	/// Sets the State of the Node
	void setState(State state);

	/// Sets the State of the Node
	void setState(State state, const std::string &shortInfo);

	/// Sets the short information string
	void setShortInfo(const std::string &shortInfo);

	/**
	 * Adds an entry to the log.
	 * @param entry The new entry.
	 */
	void addLog(const std::string &entry);

	/**
	 * Gets called when the %Node is enabled.
	 */
	virtual void initialize();

	/**
	 * Gets called when the %Node is disabled.
	 */
	virtual void shutdown();

	/// Gets called when the first InSlot is connected to the Node
	virtual void start();

	/// Gets called when the last InSlot is disconnected from the Node
	virtual void stop();

	/// Implementation of the BasicOutSlot::Listener::startOutSlot method
	virtual void startOutSlot(BasicOutSlot &outSlot);

	/// Implementation of the BasicOutSlot::Listener::stopOutSlot method
	virtual void stopOutSlot(BasicOutSlot &outSlot);

	/// Implementation of the BasicInSlot::Listener::startInSlot method
	virtual void startInSlot(BasicInSlot &inSlot);

	/// Implementation of the BasicInSlot::Listener::stopInSlot method
	virtual void stopInSlot(BasicInSlot &inSlot);

private:

	/// Copy constructor (not implemented!)
	Node(const Node &);

	/// Copy operator (not implemented!)
	const Node &operator=(const Node &);

	/// Counts how many OutSlots of the Node are connected to InSlots
	volatile unsigned int useCounter_;

	/// The State of the Node
	State state_;

	/// The short information string
	std::string shortInfo_;

	/** The number of entries in the log */
	unsigned int logSize_;

	/** Index of the oldest entry in the log */
	unsigned int logTop_;

	/** Index of the next free entry in the log */
	unsigned int logBottom_;

	/** Keeps the log entries */
	LogVector log_;
};


} // namespace InstantIO


#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif // __INSTANTIO_NODE_H
