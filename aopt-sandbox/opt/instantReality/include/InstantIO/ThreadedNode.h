#ifndef __INSTANTIO_THREADEDNODE_H
#define __INSTANTIO_THREADEDNODE_H

#ifdef _MSC_VER
# pragma once
#endif

// ========================================================================= //
//                                                                           //
// Filename: ThreadedNode.h                                                     //
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
#include "OutSlot.h"
#include "Time.h"
#include "Node.h"

namespace Medusa
{
	class Thread;
}
namespace Hermes
{
	class Wait;
}


namespace InstantIO
{


/**
 * @class ThreadedNode ThreadedNode.h InstantIO/ThreadedNode.h
 *
 * %ThreadedNode class
 */
class INSTANTIO_DLLMAPPING ThreadedNode : public Node
{
public:

	/// Constructor
	/**
	 * Creates a new %ThreadedNode object.
	 */
	ThreadedNode();

	/// Destructor
	/**
	 * Destroys the %ThreadedNode object.
	 */
	virtual ~ThreadedNode();

protected:

	/// Gets called when the first InSlot is connected to the Node
	virtual void start();

	/// Gets called when the last InSlot is disconnected from the Node
	virtual void stop();

	/// Ends operation of the %ThreadedNode
	/**
	 * wait the thread
	 */
	bool waitThread (int timeout = -1);

	/// Starts operation of the %ThreadedNode
	/**
	 * This method gets called when any component is interested
	 * to get values from this node.
	 */
	virtual int processData() = 0;

private:

	/** Copy constructor (not implemented!) */
	ThreadedNode(const ThreadedNode &);

	/** Copy operator (not implemented!) */
	const ThreadedNode &operator=(const ThreadedNode &);

	/// The thread object
	Medusa::Thread *thread_;

	/// The wait object
	Hermes::Wait *wait_;

};


} // namespace InstantIO


#endif // __INSTANTIO_CLOCKNODE_H
