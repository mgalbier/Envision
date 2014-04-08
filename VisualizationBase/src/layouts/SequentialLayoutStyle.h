/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

#pragma once

#include "../visualizationbase_api.h"

#include "LayoutStyle.h"

namespace Visualization {

class VISUALIZATIONBASE_API SequentialLayoutStyle : public LayoutStyle
{
	public:
		virtual ~SequentialLayoutStyle() override;

		bool isHorizontal() const;
		bool isForward() const;

		Property<Direction> direction{this,"direction"};
		Property<Alignment> alignment{this,"alignment"};
		Property<int> spaceBetweenElements{this,"spaceBetweenElements"};
		Property<bool> notLocationEquivalentCursors{this,"notLocationEquivalentCursors"};
		Property<bool> noInnerCursors{this,"noInnerCursors"};
		Property<bool> noBoundaryCursorsInsideShape{this,"noBoundaryCursorsInsideShape"};
		Property<bool> extraCursorsOutsideShape{this,"extraCursorsOutsideShape"};
		Property<bool> hasCursorWhenEmpty{this,"hasCursorWhenEmpty"};
		Property<int> minWidth{this,"minWidth"};
		Property<int> minHeight{this,"minHeight"};
};

inline bool SequentialLayoutStyle::isHorizontal() const
	{ return direction() == Direction::LeftToRight || direction() == Direction::RightToLeft;}
inline bool SequentialLayoutStyle::isForward() const
	{ return direction() == Direction::LeftToRight || direction() == Direction::TopToBottom;}

}
