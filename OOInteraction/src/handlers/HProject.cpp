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

#include "HProject.h"

#include "../commands/CCreateProject.h"
#include "../commands/CCreateModule.h"
#include "../commands/CCreateClass.h"
#include "../commands/CCreateMethod.h"
#include "../commands/CCreateField.h"
#include "../commands/CDumpMethodRenderings.h"
#include "../actions/OOActions.h"

#include "VersionControlUI/src/commands/CHistory.h"
#include "InteractionBase/src/commands/CMerge.h"
#include "InteractionBase/src/prompt/Prompt.h"
#include "InteractionBase/src/input_actions/ActionRegistry.h"

#include "OOVisualization/src/declarations/VProject.h"
#include "OOModel/src/declarations/Project.h"

#include "VersionControlUI/src/commands/CDiff.h"
#include "VersionControlUI/src/commands/CClear.h"

namespace OOInteraction {

HProject::HProject()
{
	// TODO: is it appropriate to add commands in the constructor or should they be registered somewhere else?
	addCommand(new CCreateProject{});
	addCommand(new CCreateModule{});
	addCommand(new CCreateClass{});
	addCommand(new CCreateMethod{});
	addCommand(new CCreateField{});

	addCommand(new VersionControlUI::CDiff{});
	addCommand(new VersionControlUI::CClear{});
	addCommand(new VersionControlUI::CHistory{});
	addCommand(new Interaction::CMerge{});

	addCommand(new CDumpMethodRenderings{});
}

HProject* HProject::instance()
{
	static HProject h;

	static bool initialized = false;
	if (!initialized)
	{
		initialized = true;
		Interaction::ActionRegistry::instance()->registerInputHandler("HProject.Paste",
																						  OOActions::pasteIntoTopLevelContainer);
	}

	return &h;
}

void HProject::keyPressEvent(Visualization::Item *target, QKeyEvent *event)
{
	if (event->modifiers() == Qt::NoModifier && (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter))
	{
		Interaction::Prompt::show(target);
	}
	else if (Interaction::ActionRegistry::instance()
				->handleKeyInput(target, QKeySequence(event->modifiers()|event->key()), "HProject")){}
	else GenericHandler::keyPressEvent(target, event);
}

}
