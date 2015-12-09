#ifndef __INSTANTIO_NODETYPE_H
#define __INSTANTIO_NODETYPE_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: NodeType.h                                                      //
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
#include "Field.h"
#include "Node.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#ifdef __sgi
# include <assert.h>
#else
# include <cassert>
#endif
#ifdef _MSC_VER
# pragma warning (push)
# pragma warning (disable: 4251)
#endif


namespace InstantIO
{


/**
 * @class NodeType NodeType.h InstantIO/NodeType.h
 *
 * Keeps meta information about Nodes. When you implement
 * a new Node, you also have to create an instance of the
 * %NodeType object that contains meta information about
 * the Node. This information consists of:
 * <ul>
 * <li>A name that is used to identify the Node.
 * <li>A pointer to a method used to create new Nodes of
 * this type.
 * <li>A short description of the Node. This information
 * is not used by the InstantIO system in any way, it is simply a
 * human-readable string that serves documentation purposes.
 * <li>A detailed description of the Node. This
 * information is not used by the InstantIO system in any way, it
 * is simply a human-readable string that serves documentation
 * purposes.
 * <li>The name of the author who wrote the Node. This
 * information is not used by the InstantIO system in any way, it
 * simply serves documentation purposes.
 * <li>Meta information about the fields (parameters) of the
 * Node.
 * </ul>
 * The following piece of code demonstrates how to create
 * a NodeType object that keeps meta information about
 * a Node "MyNode":
 * <code><pre>
 * static NodeType myNodeType(
 *   "MyNode",
 *   &MyNode::create,
 *   "Short description",
 *   "Detailed description",
 *   "Patrick D&amp;auml;hne",
 *   fields,
 *   sizeof(fields) / sizeof(Field));
 * </pre></code>
 *
 * @author Patrick D&auml;hne
 * @see Node Node
 */
class INSTANTIO_DLLMAPPING NodeType
{
public:

	/** Defines the method used to create new Nodes */
	typedef Node *(*CreateNodeMethod)();

	/**
	 * Creates a new %NodeType object.
	 * @param name The name of the %NodeType. This name
	 * is used to identify the %NodeType.
	 * @param createMethod Method used to create new nodes of this type
	 * @param shortDescription A short description of
	 * the Node. This information if not used by the InstantIO
	 * system in any way, it is simply a human-readable string
	 * that serves documentation purposes.
	 * @param longDescription A detailed description of the
	 * Node. This information is not used by the InstantIO system
	 * in any way, it is simply a human-readable string that
	 * serves documentation purposes.
	 * @param author The name of the person that wrote
	 * the node. This information is not used by the InstantIO
	 * system in any way, it simply serves documentation
	 * purposes.
	 * @param fields An array that contains meta-information
	 * about the fields of the Node.
	 * @param numFields The number of fields
	 */ 
	NodeType(const std::string &name,
	         CreateNodeMethod createMethod,
	         const char *shortDescription = 0,
	         const char *longDescription = 0,
	         const char *author = 0,
	         Field *fields = 0,
	         unsigned int numFields = 0);

	/** Destroys the %NodeType object */
	~NodeType();

	/** Defines a vector of NodeTypes */
	typedef std::vector<NodeType*> NodeTypeVector;

	/**
	 * Fills a vector with all types of nodes currently available.
	 * @param types The vector that gets filled with the NodeTypes
	 */
	static void getTypes(NodeTypeVector &types);

	/**
	 * Tries to find a %NodeType for the given name.
	 * @param name The name of the %NodeType. Case does not matter.
	 * @return The %NodeType that matches the label, or 0
	 * when no such %NodeType exists.
	 */
	static const NodeType *findType(std::string name);

	/**
	 * Returns the name of the %NodeType. The name is used to
	 * identify the %NodeType. Case does not matter.
	 * @return The name of the %NodeType.
	 */
	inline const std::string &getName() const { return name_; }

	/**
	 * Returns a short description of the Node. This information
	 * is not used by the InstantIO system in any way, it is simply a
	 * human-readable string that serves documentation purposes.
	 * @return The short description.
	 */
	inline const std::string &getShortDescription() const { return shortDescription_; }

	/**
	 * Returns a detailed description of the Node. This information
	 * is not used by the InstantIO system in any way, it is simply a
	 * human-readable string that serves documentation purposes.
	 * @return The detailed description.
	 */
	inline const std::string &getLongDescription() const { return longDescription_; }

	/**
	 * Returns the name of the person that wrote the Node.
	 * This information is not used by the InstantIO system, it simply
	 * serves documentation purposes.
	 * @return The name of the author
	 */
	inline const std::string &getAuthor() const { return author_; }

	/**
	 * Returns all meta information about the fields of the Node.
	 * Fields contain parameters of Nodes.
	 * @return An array containing all field information
	 */
	inline const Field *getFields() const { return fields_; }

	/**
	 * Returns meta information about a specific field of the
	 * Node.
	 * @param name The name of the field. Case does not matter.
	 * @return The field that has the given name or 0 when such
	 * a field does not exist.
	 */
	const Field *getField(const std::string &name) const;

	/**
	 * Returns the number of fields for the %NodeType.
	 * @return The number of fields
	 */
	inline unsigned int numFields() const { return numFields_; }

	/**
	 * Creates a new Node.
	 * @return The new Node.
	 */
	static std::auto_ptr<Node> newInstance(const std::string &type)
	/* throw (InvalidNodeException)*/;

	/**
	 * Creates a new Node.
	 * @return The new Node.
	 */
	inline std::auto_ptr<Node> newInstance() const
	{
		assert(createMethod_ != 0);
		return std::auto_ptr<Node>(createMethod_());
	}

private:

	/** Copy Constructor (not implemented!) */
	NodeType(const NodeType &);

	/** Copy Operator (not implemented!) */
	const NodeType &operator=(const NodeType &);

	/**
	 * Tries to find a %NodeType for the given name.
	 * @param name The name of the %NodeType. Case does not matter.
	 * @return The %NodeType that matches the label, or 0
	 * when no such %NodeType exists.
	 */
	static const NodeType *internalFindType(std::string name);

	/** The name of the %NodeType */
	std::string name_;

	/** The short description of the Node */
	std::string shortDescription_;

	/** The detailed description of the Node */
	std::string longDescription_;

	/** The author that wrote the Node */
	std::string author_;

	/** Meta information about the fields of the Node */
	Field *fields_;

	/** The number of fields */
	unsigned int numFields_;

	/** Defines a map that maps field names to fields */
	typedef std::map<std::string, Field*> FieldMap;

	/** Maps field names to fields */
	FieldMap fieldMap_;

	/** The method that creates new Nodes of this type */
	CreateNodeMethod createMethod_;

	/** Defines a map that maps %NodeTypes names to NodeTypes */
	typedef std::map<std::string, NodeType*> TypeMap;

	/** Maps %NodeType names to NodeTypes */
	static TypeMap *types_;

	/** Helper Method to load Plugins; returns true on success */
	static bool loadLib ( const std::string &pluginName );

};


} // namespace InstantIO


#ifdef _MSC_VER
# pragma warning (pop)
#endif

#endif // __INSTANTIO_NODETYPE_H
