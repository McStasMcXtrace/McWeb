#ifndef __INSTANTIO_FIELD_H
#define __INSTANTIO_FIELD_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: Field.h                                                         //
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
#include "BasicFieldAccessor.h"
#include <string>


namespace InstantIO
{


class Node;


/**
 * @class Field Field.h InstantIO/Field.h
 *
 * Keeps information about Node fields.
 * <p>
 * Most {@link Node Nodes} need some kind of parameters when they are started,
 * e.g. which serial port they should use to communicate with a device. The Field
 * class contains meta information that describes one parameter of a Node. The
 * meta information consists of:
 * <ul>
 * <li> The name of the field. The name is used to identify the field.
 * <li> A description of the field. This description is not used by the system in any
 * way. It solely serves documentation purposes. It contains information about the
 * field in a human-readable way. Some user interfaces for managing the InstantIO system
 * display the description to the user.
 * <li> The default value of the field. The default value is a string representation
 * of the value the field contains when the user does not explicitly specify a value.
 * <li> A FieldAccessor object used to access the contents of the field.
 * </ul>
 * For example, let's say you want to create a Node that operates a device
 * that is connected to a serial port of the computer. So, you need to specify the
 * number of the serial port and the baud rate. First, you have to write methods for
 * setting and getting the port number and the baud rate:
 * <code><pre>
 * class MyNode: public Node
 * {
 * public:
 *   ...
 *   void setPort(int port);
 *   int getPort();
 *   void setBaudRate(int port);
 *   int getBaudRate();
 *   ...
 * };
 * </pre></code>
 * Then, you have to create instances of the Field class that describe the parameters.
 * Usually, you do this by creating a static array in the Node class:
 * <code><pre>
 * class MyNode: public Node
 * {
 * public:
 *   ...
 *    static Field fields[2] =
 *    {
 *      Field(
 *        "port",
 *        "The serial port the device is connected to",
 *        "0",
 *        FieldAccessor<MyNode, int>(&MyNode::setPort, &MyNode::getPort)
 *      ),
 *      Field(
 *        "BaudRate",
 *        "The baud rate the device is operating at",
 *        "115200",
 *        FieldAccessor<MyNode, int>(&MyNode::setBaudRate, &MyNode::getBaudRate)
 *      )
 *    };
 *   ...
 * };
 * </pre></code>
 * Finally, you have to specify the fields when you create the Instance of the
 * NodeType object that contains meta information about the Node:
 * <code><pre>
 * static NodeType myType(
 *   "MyNode", &MyNode::create,
 *   "Example Node", 0, "Patrick D&amp;auml;hne",
 *   MyNode::fields
 *   sizeof(MyNode::fields) / sizeof(Field));
 * </pre></code>
 * @author Patrick D&auml;hne
 * @see Node Node
 * @see NodeType NodeType
 */
class INSTANTIO_DLLMAPPING Field
{
public:

	/**
	 * Creates a new %Field object.
	 * @param name The name of the field. The name is used to identify the field.
	 * @param description A description of the field. This description is not
	 * used by the system in any way. It solely serves documentation purposes.
	 * It contains information about the field in a human-readable way. Some
	 * user interfaces for managing the InstantIO system display the description to
	 * the user.
	 * @param defaultValue The default value of the field. The default value is
	 * a string representation of the value the field contains when the user
	 * does not explicitly specify a value.
	 * @param fieldAccessor A FieldAccessor object used to access the contents
	 * of the field.
	 */
	inline Field(const std::string &name, const std::string &description, const std::string &defaultValue, const BasicFieldAccessor &fieldAccessor)
	: name_(name), description_(description), defaultValue_(defaultValue), fieldAccessor_(fieldAccessor.clone())
	{}

	/**
	 * Creates a new %Field object that is an exact copy
	 * of another %Field object.
	 * @param other The other %Field object.
	 */
	inline Field(const Field &other)
	: name_(other.name_), description_(other.description_), defaultValue_(other.defaultValue_), fieldAccessor_(other.fieldAccessor_->clone())
	{}

	/** Destroys the %Field object */
	inline ~Field()
	{ delete fieldAccessor_; }

	/**
	 * Returns the name of the field. The name is used to identify the
	 * field.
	 * @return The name of the field.
	 */
	inline const std::string &getName() const
	{ return name_; }

	/**
	 * Returns the description of the field. The description of the field
	 * is not used by the system in any way. It solely serves documentation
	 * purposes. It contains information about the field in a human-readable
	 * way. Some user interfaces for managing the InstantIO system display the
	 * description to the user.
	 * @return The description of the field.
	 */
	inline const std::string &getDescription() const
	{ return description_; }

	/**
	 * Returns the default value of the field. The default value is a
	 * string representation of the value the field contains when the
	 * user does not explicitly specify a value.
	 * @return The default value.
	 */
	inline const std::string &getDefaultValue() const
	{ return defaultValue_; }

	/**
	 * Sets the value of a field. Use this method to set fields of a
	 * Node.
	 * @param node The Node object whose field you want to set.
	 * @param value A string representation of the new value of the field.
	 */
	inline void set(Node &node, const std::string &value) const
	{ fieldAccessor_->set(node, value); }

	/**
	 * Returns the value of the field. Use this method to read fields of
	 * a Node.
	 * @param node The Node object whose field you want to get.
	 * @return A string representation of the field's value.
	 */
	inline std::string get(const Node &node) const
	{ return fieldAccessor_->get(node); }

	/**
	 * Checks if a field is set to given value.
	 * @param node The Node that contains the field.
	 * @param value The value we compare the field with.
	 * @return true when the field has the given value,
	 * false otherwise.
	 */
	inline bool sameValue(const Node &node, const std::string &value) const
	{ return fieldAccessor_->sameValue(node, value); }

private:

	/** Copy operator (not implemented!) */
	const Field &operator=(const Field &);

	/** The name of the field */
	std::string name_;

	/** The description of the field */
	std::string description_;

	/** The default value of the field */
	std::string defaultValue_;

	/** Accessor object used to access the contents of the field */
	const BasicFieldAccessor *fieldAccessor_;
};


} // namespace InstantIO


#endif // __INSTANTIO_FIELD_H
