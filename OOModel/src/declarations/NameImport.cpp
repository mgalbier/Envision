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

#include "NameImport.h"
#include "../types/SymbolProviderType.h"
#include "../expressions/ReferenceExpression.h"

#include "ModelBase/src/nodes/TypedList.hpp"
#include "OOModel/src/typesystem/OOResolutionRequest.h"
template class Model::TypedList<OOModel::NameImport>;

namespace OOModel {

DEFINE_COMPOSITE_EMPTY_CONSTRUCTORS(NameImport)
DEFINE_COMPOSITE_TYPE_REGISTRATION_METHODS(NameImport)

DEFINE_ATTRIBUTE(NameImport, importedName, Expression, false, false, true)
DEFINE_ATTRIBUTE(NameImport, importAll, Boolean, false, false, true)

NameImport::NameImport(Expression *importedName, bool importAllChildrenInScope)
: Super{nullptr, NameImport::getMetaData()}
{
	if (importedName) setImportedName(importedName);
	if (importAllChildrenInScope) setImportAll(true);
}

bool NameImport::definesSymbol() const
{
	return false;
}

Model::Node* NameImport::target() const
{
	Model::Node* ret{};

	auto type = this->importedName()->type();
	if (auto sp = dynamic_cast<SymbolProviderType*>(type.get()))
		ret = sp->symbolProvider();

	return ret;
}

bool NameImport::findSymbols(std::unique_ptr<Model::ResolutionRequest> request) const
{
	Q_ASSERT(request->direction() != SEARCH_DOWN);

	// Name imports only provide shortcuts for objects that are within the entity declaring the import
	auto p = parent(); // This should be a list of Declarations
	if (p) p = p->parent(); // This should be the parent entity
	if (!p || !p->isAncestorOf(request->source())) return false;
	// Note above that it is important that we only consider descendants of p and not p itself. This is because when
	// a NameImport (or the initial part of one) within p, resolves to p itself (e.g. import java.something inside the
	// java package) we will do a symbol start with p as a source. In that case import should not be further used as
	// shortcuts.


	if (request->direction() == SEARCH_HERE)
	{

		// If this node is part of a list and the source is a name import from the same list, impose an order
		// The code below wa commented when SEARCH_UP_ORDERED was introduced as it should have made it redundant.
//		auto listParent = DCast<Model::List>(parent());
//		if (listParent)
//		{
//			int sourceIndex = listParent->indexToSubnode(request->source());
//			if (sourceIndex >=0)
//				if (DCast<NameImport> (listParent->at<Model::Node>(sourceIndex)))
//				{
//					int thisIndex = listParent->indexOf(this);
//					if (thisIndex > sourceIndex)
//						return false;
//				}
//		}

		// If this node defines a shortcut to a single name which is not the one being looked for, then do not resolve
		// the target
		if (!importAll())
			if (auto ref = DCast<ReferenceExpression>(importedName()))
				if (!request->matcher().matches(ref->name())) return false;

		if (auto t = target())
			return t->findSymbols(request->clone((importAll() ? t : request->source()),
					(importAll() ? SEARCH_DOWN : SEARCH_HERE), false));
	}
	else if (request->direction() == SEARCH_UP || request->direction() == SEARCH_UP_ORDERED)
	{
		if (parent())
			return parent()->findSymbols(request->clone(SEARCH_UP_ORDERED));
	}

	return false;
}

}
