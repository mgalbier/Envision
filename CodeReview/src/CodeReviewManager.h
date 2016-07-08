/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2016 ETH Zurich
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

#pragma once

#include "codereview_api.h"

#include "VersionControlUI/src/nodes/DiffComparisonPair.h"

#include "nodes/CommentedNode.h"

namespace CodeReview {

using DiffFrames = QList<VersionControlUI::DiffComparisonPair*>;

using OrderedDiffFrames = QList<VersionControlUI::DiffComparisonPair*>;

using GroupedDiffFrames = QList<QList<VersionControlUI::DiffComparisonPair*>>;

using GroupingFunction =
		std::function<GroupedDiffFrames (DiffFrames diffComparisonPairs)>;

using OrderingFunction =
		std::function<OrderedDiffFrames (DiffFrames diffComparisonPairs)>;

class CODEREVIEW_API CodeReviewManager
{
	public:
		CommentedNode* commentedNode(QString nodeId);
		static CodeReviewManager& instance();

		static QList<QList<VersionControlUI::DiffComparisonPair*>> orderDiffComparisonPairs(
				GroupingFunction dcpgf, OrderingFunction dcpsf,
				QList<VersionControlUI::DiffComparisonPair*> diffComparisonPairs);


	private:
		QHash<QString, CommentedNode*> commentedNodes_;
		CodeReviewManager(QString oldVersion, QString newVersion);

		Model::TreeManager* manager_{};

};

}
