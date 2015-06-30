/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
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

#include "CScript.h"

#include "ModelBase/src/nodes/Node.h"
#include "OOModel/src/declarations/Class.h"
#include "OOModel/src/declarations/Method.h"

#include "../wrappers/AstApi.h"
#include "../helpers/BoostPythonHelpers.h"

namespace InformationScripting {

CScript::CScript() : Command{"script"} {}

bool CScript::canInterpret(Visualization::Item*, Visualization::Item*, const QStringList& commandTokens,
									const std::unique_ptr<Visualization::Cursor>&)
{
	return commandTokens.size() > 1 && commandTokens.first() == "script";
}

Interaction::CommandResult* CScript::execute(Visualization::Item*, Visualization::Item* target,
															const QStringList& commandTokens, const std::unique_ptr<Visualization::Cursor>&)
{
	using namespace boost::python;

	QStringList args = commandTokens.mid(1);
	if (args[0] == "methods")
	{
		auto node = target->node();
		Q_ASSERT(node);

		auto parentClass = node->firstAncestorOfType<OOModel::Class>();

		try {
			PyImport_AppendInittab((char*)"AstApi", PyInit_AstApi);
			Py_Initialize();

			BoostPythonHelpers::initializeQStringConverters();

			object main_module = import("__main__");
			dict main_namespace = extract<dict>(main_module.attr("__dict__"));

			object nodeApi = import("AstApi");
			object sys = import("sys");

			list methods;
			for (auto method : *parentClass->methods())
				methods.append(ptr(method));

			main_namespace["methods"] = methods;

			exec_file("scripts/methods.py", main_namespace, main_namespace);
			// Workaround to get output
			sys.attr("stdout").attr("flush")();
		} catch (error_already_set ) {
			qDebug() << "Error in Python: " << BoostPythonHelpers::parsePythonException();
		}
	}
	return new Interaction::CommandResult();
}

} /* namespace InformationScripting */
