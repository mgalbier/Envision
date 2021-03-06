/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2014 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

#include "NodeOwningCommand.h"

#include "../model/AllTreeManagers.h"
#include "../model/TreeManager.h"
#include "../nodes/Node.h"

namespace Model {

NodeOwningCommand::NodeOwningCommand(Node* target, const QString & text, Node* ownedIfDone, Node* ownedIfUndone)
: UndoCommand{target, text}, ownedIfDone_{ownedIfDone}, ownedIfUndone_{ownedIfUndone}
{
	// If the target node is not yet owned, do not assume ownership over its subnodes.
	if (target->manager() == nullptr)
	{
		// A tree that has once become owned by an undo stack or a manager, should never become owned by another
		// tree that has no manager. If this were allowed, then the latter tree can be freely deleted, thereby
		// invalidating the undo command. In general, unless a tree which has once become owned by the undo stack,
		// always stays within the undo stack, there is no way to guarantee that we can indeed perform an undo.
		Q_ASSERT(!ownedIfDone_ || !AllTreeManagers::instance().managerOfOwningUndoStack(ownedIfDone_));
		Q_ASSERT(!ownedIfUndone_ || !AllTreeManagers::instance().managerOfOwningUndoStack(ownedIfUndone_));

		ownedIfDone_ = nullptr;
		ownedIfUndone_ = nullptr;
	}
}

NodeOwningCommand::~NodeOwningCommand()
{
	auto n = owned();
	// Only delete a node if:
	// - It is not part of a manager
	// - It is not currently owned by any other command in any undo stack
	if (n && !n->manager())
		if (AllTreeManagers::instance().managerOfOwningUndoStack(n, this) == nullptr)
			SAFE_DELETE(n);
}

Node* NodeOwningCommand::owned() const
{
	return isUndone() ? ownedIfUndone_ : ownedIfDone_;
}

Node* NodeOwningCommand::insertedNode() const
{
	return ownedIfUndone_;
}

Node* NodeOwningCommand::removedNode() const
{
	return ownedIfDone_;
}

void NodeOwningCommand::redo()
{
	if (auto manager = target()->manager())
	{
		moveNodes(ownedIfDone_, manager->nodeIdMap(), manager->nodeIdMapForUndoStack()); // handle deletions
		moveNodes(ownedIfUndone_, manager->nodeIdMapForUndoStack(), manager->nodeIdMap()); // handle insertions
	}
	UndoCommand::redo();
}

void NodeOwningCommand::undo()
{
	if (auto manager = target()->manager())
	{
		moveNodes(ownedIfUndone_, manager->nodeIdMap(), manager->nodeIdMapForUndoStack());  // handle insertions
		moveNodes(ownedIfDone_, manager->nodeIdMapForUndoStack(), manager->nodeIdMap()); // handle deletions
	}
	UndoCommand::undo();
}

void NodeOwningCommand::moveNodes(Node* root, NodeIdMap& source, NodeIdMap& target)
{
	if (root)
	{
		QList<Node*> stack{root};
		while (!stack.isEmpty())
		{
			auto node = stack.takeFirst();

			auto id = source.idIfExists(node);
			if (!id.isNull())
			{
				target.setId(node, id);
				source.remove(node);
			}

			stack.append( node->children() );
		}
	}
}

}
