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

#include "DeclarationVisitor.h"
#include "ExpressionVisitor.h"
#include "StatementVisitor.h"
#include "ElementVisitor.h"

#include "../CppExportException.h"
#include "../SpecialCases.h"

#include "OOModel/src/expressions/Expression.h"
#include "OOModel/src/expressions/types/ArrayTypeExpression.h"
#include "OOModel/src/expressions/types/ReferenceTypeExpression.h"
#include "OOModel/src/expressions/types/PointerTypeExpression.h"
#include "OOModel/src/expressions/types/FunctionTypeExpression.h"
#include "OOModel/src/expressions/types/ClassTypeExpression.h"
#include "OOModel/src/expressions/types/PrimitiveTypeExpression.h"
#include "OOModel/src/expressions/types/TypeQualifierExpression.h"
#include "OOModel/src/expressions/types/AutoTypeExpression.h"
#include "OOModel/src/expressions/AssignmentExpression.h"
#include "OOModel/src/expressions/BinaryOperation.h"
#include "OOModel/src/expressions/UnaryOperation.h"
#include "OOModel/src/expressions/TypeTraitExpression.h"
#include "OOModel/src/expressions/BooleanLiteral.h"
#include "OOModel/src/expressions/IntegerLiteral.h"
#include "OOModel/src/expressions/FloatLiteral.h"
#include "OOModel/src/expressions/NullLiteral.h"
#include "OOModel/src/expressions/StringLiteral.h"
#include "OOModel/src/expressions/CharacterLiteral.h"
#include "OOModel/src/expressions/CastExpression.h"
#include "OOModel/src/expressions/InstanceOfExpression.h"
#include "OOModel/src/expressions/CommaExpression.h"
#include "OOModel/src/expressions/ConditionalExpression.h"
#include "OOModel/src/expressions/SuperExpression.h"
#include "OOModel/src/expressions/ThisExpression.h"
#include "OOModel/src/expressions/GlobalScopeExpression.h"
#include "OOModel/src/expressions/ThrowExpression.h"
#include "OOModel/src/expressions/TypeNameOperator.h"
#include "OOModel/src/expressions/DeleteExpression.h"
#include "OOModel/src/expressions/VariableDeclarationExpression.h"
#include "OOModel/src/expressions/LambdaExpression.h"
#include "OOModel/src/expressions/ArrayInitializer.h"
#include "OOModel/src/expressions/MethodCallExpression.h"
#include "OOModel/src/expressions/MetaCallExpression.h"
#include "OOModel/src/statements/Statement.h"
#include "OOModel/src/expressions/NewExpression.h"
#include "OOModel/src/types/PointerType.h"
#include "OOModel/src/types/SymbolProviderType.h"
#include "OOModel/src/expressions/EmptyExpression.h"
#include "OOModel/src/expressions/ErrorExpression.h"
#include "OOModel/src/expressions/UnfinishedOperator.h"
#include "OOModel/src/declarations/Class.h"

using namespace Export;
using namespace OOModel;

namespace CppExport {

template <typename T> Export::SourceFragment* ExpressionVisitor::optional(T* node)
{
	return node ? visit(node) : nullptr;
}

SourceFragment* ExpressionVisitor::visit(Expression* expression)
{
	auto fragment = new CompositeFragment{expression};

	// Types ============================================================================================================
	if (auto e = DCast<ArrayTypeExpression>(expression))
		*fragment << visit(e->typeExpression()) << "[" << optional(e->fixedSize()) << "]";
	else if (auto e = DCast<ReferenceTypeExpression>(expression))
		*fragment << visit(e->typeExpression()) << (e->isRValueReference() ? "&&" : "&");
	else if (auto e = DCast<PointerTypeExpression>(expression))
	{
		if (DCast<FunctionTypeExpression>(e->typeExpression()))
			*fragment << visitFunctionPointer(e);
		else
			*fragment << visit(e->typeExpression()) << "*";
	}
	else if (auto e = DCast<ClassTypeExpression>(expression)) *fragment << visit(e->typeExpression());
	else if (auto e = DCast<PrimitiveTypeExpression>(expression))
	{
		switch (e->typeValue())
		{
			case PrimitiveTypeExpression::PrimitiveTypes::INT: *fragment << "int"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::LONG: *fragment << "long"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_INT: notAllowed(e); break;
			case PrimitiveTypeExpression::PrimitiveTypes::UNSIGNED_LONG: notAllowed(e); break;
			case PrimitiveTypeExpression::PrimitiveTypes::FLOAT: *fragment << "float"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::DOUBLE: *fragment << "double"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::BOOLEAN: *fragment << "bool"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::CHAR: *fragment << "char"; break;
			case PrimitiveTypeExpression::PrimitiveTypes::VOID: *fragment << "void"; break;
			default: error(e, "Unknown primitive type");
		}
	}
	else if (auto e = DCast<TypeQualifierExpression>(expression))
	{
		// for pointer type expressions the const is printed after the expression
		if (DCast<PointerTypeExpression>(e->typeExpression()))
			*fragment << " " << visit(e->typeExpression());

		auto parentVariableDeclaration = e->firstAncestorOfType<VariableDeclaration>();
		if (!parentVariableDeclaration || !parentVariableDeclaration->modifiers()->isSet(Modifier::ConstExpr))
			switch (e->qualifier())
			{
				case TypeQualifierExpression::Qualifier::CONST: *fragment << "const"; break;
				case TypeQualifierExpression::Qualifier::VOLATILE: *fragment << "volatile"; break;
				default: error(e, "Unknown qualifier");
			}

		// for non-pointer type expressions the const is printed before the expression
		if (!DCast<PointerTypeExpression>(e->typeExpression()))
			*fragment << " " << visit(e->typeExpression());
	}
	else if (auto e = DCast<AutoTypeExpression>(expression)) *fragment << new TextFragment{e, "auto"};
	else if (auto e = DCast<FunctionTypeExpression>(expression))
		*fragment << list(e->results(), this) << " " << list(e->arguments(), this, "argsList");

	// Operators ========================================================================================================

	else if (auto e = DCast<AssignmentExpression>(expression))
	{
		*fragment << visit(e->left()) << " ";
		switch (e->op())
		{
			case AssignmentExpression::ASSIGN: *fragment << "="; break;
			case AssignmentExpression::PLUS_ASSIGN: *fragment << "+="; break;
			case AssignmentExpression::MINUS_ASSIGN: *fragment << "-="; break;
			case AssignmentExpression::TIMES_ASSIGN: *fragment << "*="; break;
			case AssignmentExpression::DIVIDE_ASSIGN: *fragment << "/="; break;
			case AssignmentExpression::BIT_AND_ASSIGN: *fragment << "&="; break;
			case AssignmentExpression::BIT_OR_ASSIGN: *fragment << "|="; break;
			case AssignmentExpression::BIT_XOR_ASSIGN: *fragment << "^="; break;
			case AssignmentExpression::REMAINDER_ASSIGN: *fragment << "%="; break;
			case AssignmentExpression::LEFT_SHIFT_ASSIGN: *fragment << "<<="; break;
			case AssignmentExpression::RIGHT_SHIFT_SIGNED_ASSIGN: *fragment << ">>="; break;
			case AssignmentExpression::RIGHT_SHIFT_UNSIGNED_ASSIGN: *fragment << ">>>="; break;
			default: error(e, "Unknown assignment type");
		}
		*fragment << " " << visit(e->right());
	}
	else if (auto e = DCast<BinaryOperation>(expression))
	{
		bool stringLiteralConcatenation = false;
		if (e->op() == BinaryOperation::PLUS)
		{
			auto left = e->left();
			while (auto binaryOperation = DCast<BinaryOperation>(left))
				left = binaryOperation->right();

			auto right = e->right();
			while (auto binaryOperation = DCast<BinaryOperation>(right))
				right = binaryOperation->left();

			if (DCast<OOModel::StringLiteral>(left) && DCast<OOModel::StringLiteral>(right))
				stringLiteralConcatenation = true;
		}

		auto binaryOperationFragment = stringLiteralConcatenation ? fragment->append(new CompositeFragment{e, "sections"})
																					 :	fragment;
		*binaryOperationFragment << visit(e->left());
		if (!stringLiteralConcatenation)
			switch (e->op())
			{
				case BinaryOperation::TIMES: *binaryOperationFragment << "*"; break;
				case BinaryOperation::DIVIDE: *binaryOperationFragment << "/"; break;
				case BinaryOperation::REMAINDER: *binaryOperationFragment << "%"; break;
				case BinaryOperation::PLUS: *binaryOperationFragment << "+"; break;
				case BinaryOperation::MINUS: *binaryOperationFragment << "-"; break;
				case BinaryOperation::LEFT_SHIFT: *binaryOperationFragment << " << "; break;
				case BinaryOperation::RIGHT_SHIFT_SIGNED: *binaryOperationFragment << " >> "; break;
				case BinaryOperation::RIGHT_SHIFT_UNSIGNED: *binaryOperationFragment << " >>> "; break;
				case BinaryOperation::LESS: *binaryOperationFragment << " < "; break;
				case BinaryOperation::GREATER: *binaryOperationFragment << " > "; break;
				case BinaryOperation::LESS_EQUALS: *binaryOperationFragment << " <= "; break;
				case BinaryOperation::GREATER_EQUALS: *binaryOperationFragment << " >= "; break;
				case BinaryOperation::EQUALS: *binaryOperationFragment << " == "; break;
				case BinaryOperation::NOT_EQUALS: *binaryOperationFragment << " != "; break;
				case BinaryOperation::XOR: *binaryOperationFragment << " ^ "; break;
				case BinaryOperation::AND: *binaryOperationFragment << " & "; break;
				case BinaryOperation::OR: *binaryOperationFragment << " | "; break;
				case BinaryOperation::CONDITIONAL_AND: *binaryOperationFragment << " && "; break;
				case BinaryOperation::CONDITIONAL_OR: *binaryOperationFragment << " || "; break;
				case BinaryOperation::ARRAY_INDEX: *binaryOperationFragment << "["; break;
				default: error(e, "Unknown binary operator type");
			}
		*binaryOperationFragment << visit(e->right());
		if (e->op() == BinaryOperation::ARRAY_INDEX) *binaryOperationFragment << "]";
	}
	else if (auto e = DCast<UnaryOperation>(expression))
	{
		switch (e->op())
		{
			case UnaryOperation::PREINCREMENT: *fragment << "++" << visit(e->operand()); break;
			case UnaryOperation::PREDECREMENT: *fragment << "--" << visit(e->operand()); break;
			case UnaryOperation::POSTINCREMENT: *fragment << visit(e->operand()) << "++"; break;
			case UnaryOperation::POSTDECREMENT: *fragment << visit(e->operand()) << "--"; break;
			case UnaryOperation::PLUS: *fragment << "+" << visit(e->operand()); break;
			case UnaryOperation::MINUS: *fragment << "-" << visit(e->operand()); break;
			case UnaryOperation::NOT: *fragment << "!" << visit(e->operand()); break;
			case UnaryOperation::COMPLEMENT: *fragment << "~" << visit(e->operand()); break;
			case UnaryOperation::PARENTHESIS: *fragment << "(" << visit(e->operand()) << ")"; break;
			case UnaryOperation::DEREFERENCE: *fragment << "*" << visit(e->operand()); break;
			case UnaryOperation::ADDRESSOF: *fragment << "&" << visit(e->operand()); break;
			default: error(e, "Unknown unary operator type");
		}
	}
	else if (auto e = DCast<TypeTraitExpression>(expression))
	{
		if (e->typeTraitKind() == TypeTraitExpression::TypeTraitKind::SizeOf) *fragment << "sizeof";
		else if (e->typeTraitKind() == TypeTraitExpression::TypeTraitKind::AlignOf) *fragment << "alignof";
		else if (e->typeTraitKind() == TypeTraitExpression::TypeTraitKind::TypeId) *fragment << "typeid";
		else Q_ASSERT(false);

		*fragment << "(" << visit(e->operand()) << ")";
	}

	// Literals =========================================================================================================

	else if (auto e = DCast<BooleanLiteral>(expression)) *fragment << (e->value() ? "true" : "false");
	else if (auto e = DCast<IntegerLiteral>(expression)) *fragment << e->value();
	else if (auto e = DCast<FloatLiteral>(expression)) *fragment << e->value();
	else if (DCast<NullLiteral>(expression)) *fragment << "nullptr";
	else if (auto e = DCast<StringLiteral>(expression))
		*fragment << "\"" << QString(e->value()).replace(QRegExp("\\n"), "\\n") << "\""; // TODO: also consider \r
	else if (auto e = DCast<CharacterLiteral>(expression)) *fragment << "'" << e->value() << "'";

	// Misc =============================================================================================================

	else if (auto e = DCast<CastExpression>(expression))
		switch (e->castKind())
		{
			case CastExpression::CastKind::Default:
				*fragment << "(" << visit(e->castType()) << ") " << visit(e->expr());
				break;
			case CastExpression::CastKind::ConstCast:
				*fragment << "const_cast<" << visit(e->castType()) << ">(" << visit(e->expr()) << ")";
				break;
			case CastExpression::CastKind::DynamicCast:
				*fragment << "dynamic_cast<" << visit(e->castType()) << ">(" << visit(e->expr()) << ")";
				break;
			case CastExpression::CastKind::ReinterpretCast:
				*fragment << "reinterpret_cast<" << visit(e->castType()) << ">(" << visit(e->expr()) << ")";
				break;
			case CastExpression::CastKind::StaticCast:
				*fragment << "static_cast<" << visit(e->castType()) << ">(" << visit(e->expr()) << ")";
				break;
			case CastExpression::CastKind::FunctionalCast:
				*fragment << visit(e->castType()) << "(" << visit(e->expr()) << ")";
				break;
			default: error(e, "Unknown cast type");
		}
	else if (auto e = DCast<InstanceOfExpression>(expression))
		*fragment << visit(e->expr()) << " instanceof " << visit(e->typeExpression());
	else if (auto e = DCast<CommaExpression>(expression)) *fragment << visit(e->left()) << ", " << visit(e->right());
	else if (auto e = DCast<ConditionalExpression>(expression))
		*fragment << visit(e->condition()) << " ? " << visit(e->trueExpression()) << " : " << visit(e->falseExpression());
	else if (DCast<SuperExpression>(expression)) *fragment << "super";
	else if (DCast<ThisExpression>(expression)) *fragment << "this";
	else if (auto e = DCast<GlobalScopeExpression>(expression)) notAllowed(e);
	else if (auto e = DCast<ThrowExpression>(expression)) *fragment << "throw " << visit(e->expr());
	else if (auto e = DCast<TypeNameOperator>(expression))  *fragment << "typename " << visit(e->typeExpression());
	else if (auto e = DCast<DeleteExpression>(expression))
		*fragment << "delete" << (e->isArray() ? "[] " : " ") << visit(e->expr());
	else if (auto e = DCast<VariableDeclarationExpression>(expression)) *fragment << declaration(e->decl());
	else if (auto e = DCast<LambdaExpression>(expression))
	{
		*fragment << "[";
		if (e->defaultCaptureType() == LambdaExpression::DefaultCaptureType::Value)
			*fragment << "=";
		else if (e->defaultCaptureType() == LambdaExpression::DefaultCaptureType::Reference)
			*fragment << "&";

		if (!e->captures()->isEmpty())
		{
			if (e->defaultCaptureType() != LambdaExpression::DefaultCaptureType::None) *fragment << ", ";
			*fragment << list(e->captures(), ExpressionVisitor(data()), "comma");
		}
		*fragment << "]" << list(e->arguments(), ElementVisitor(data()), "argsList");
		if (e->results()->size() > 1) error(e->results(), "Cannot have more than one return value in Cpp");
		if (e->results()->size() == 1) *fragment << " -> " << visit(e->results()->first()->typeExpression());
		*fragment << list(e->body(), StatementVisitor(data()), "body");
	}
	else if (auto e = DCast<ArrayInitializer>(expression)) *fragment << list(e->values(), this, "initializerList");
	else if (auto e = DCast<MethodCallExpression>(expression))
	{
		if (e->methodCallKind() == MethodCallExpression::MethodCallKind::Call)
			*fragment << visit(e->callee()) << list(e->arguments(), this, "argsList");
		else
			*fragment << visit(e->callee()) << list(e->arguments(), this, "initializerList");
	}
	else if (auto e = DCast<MetaCallExpression>(expression))
	{
		auto calleeReference = DCast<OOModel::ReferenceExpression>(e->callee());
		if (calleeReference && calleeReference->name() == "Q_EMIT")
		{
			*fragment << visit(e->callee());
			auto signalsMethodCall = DCast<OOModel::MethodCallExpression>(e->arguments()->first());
			Q_ASSERT(signalsMethodCall);
			*fragment << " " << visit(signalsMethodCall);
		}
		else
		{
			*fragment << visit(e->callee());

			if (auto arguments = SpecialCases::overrideFlagArgumentTransformation(e))
				*fragment << arguments;
			else if (!e->arguments()->isEmpty())
			{
				auto arguments = new Export::CompositeFragment{e->arguments(), "argsList"};
				for (auto node : *e->arguments())
					if (auto n = DCast<Expression>(node)) *arguments << visit(n);
					else if (auto n = DCast<Statement>(node)) *arguments << statement(n);
					else if (auto n = DCast<Declaration>(node)) *arguments << declaration(n);
					else if (auto n = DCast<FormalResult>(node)) *arguments << element(n);
					else Q_ASSERT(false);
				*fragment << arguments;
			}
		}
	}
	else if (auto e = DCast<NewExpression>(expression))
	{
		*fragment << "new " << visit(e->newType());
		for (auto dim : *e->dimensions())
			*fragment << "[" << visit(dim) << "]";
		if (e->initializer()) *fragment << visit(e->initializer());
	}
	else if (auto e = DCast<ReferenceExpression>(expression))
	{
		if (e->prefix() && !DCast<MetaCallExpression>(expression->parent()))
		{
			if (e->memberKind() == ReferenceExpression::MemberKind::Dot)
				*fragment << visit(e->prefix()) << ".";
			else if (e->memberKind() == ReferenceExpression::MemberKind::Pointer)
				*fragment << visit(e->prefix()) << "->";
			else if (e->memberKind() == ReferenceExpression::MemberKind::Static)
				*fragment << visit(e->prefix()) << "::";
			else if (e->memberKind() == ReferenceExpression::MemberKind::Template)
				*fragment << visit(e->prefix()) << "::template ";
			else
				Q_ASSERT(false);
		}

		if (!e->prefix() && printContext().isClass() && e->target())
		{
			/* export problem
			 * ==============
			 * in case we have a resolved reference without prefix and we are printing the source part we might have
			 * to qualify the reference depending on the target context.
			 *
			 * for example the "AdapterManager::AdapterKey" in AdapterManager.cpp:
			 * QHash<AdapterManager::AdapterKey, AdapterManager::AdapterCreationFunction>& AdapterManager::adapters()
			 *
			 * the imported code would be printed like this:
			 * QHash<AdapterKey, AdapterCreationFunction>& AdapterManager::adapters()
			 * because AdapterKey is defined inside AdapterManager and therefore we do not need the qualification in
			 * the header part or Envision's tree model. However when it appears in the public interface and the is used
			 * in a method definition outside the declaration of AdapterManager we have to compute this prefix.
			 */

			/*
			 * addPrefix is true if the reference occurs inside the results of the parent method or
			 * in the type of the parent field.
			 *
			 * inBodyOfMethod is true if the reference occurs inside the parent method body
			 */
			bool addPrefix = false;
			bool inBodyOfMethod = false;
			if (auto parentMethod = e->firstAncestorOfType<Method>())
			{
				if (parentMethod->results()->isAncestorOf(e))
					addPrefix = true;
				if (parentMethod->items()->isAncestorOf(e))
					inBodyOfMethod = true;
			}
			else if (auto parentField = e->firstAncestorOfType<Field>())
				if (parentField->typeExpression()->isAncestorOf(e))
					addPrefix = true;

			if (addPrefix)
				if (auto parentClass = e->firstAncestorOfType<Class>())
				{
					/*
					 * if the target of the reference is inside the parent class and it does not target a friend declaration
					 * or a formal type argument then add the parent class name as prefix.
					 */
					if (parentClass->isAncestorOf(e->target()) &&
						 !parentClass->friends()->isAncestorOf(e->target()) &&
						 !DCast<FormalTypeArgument>(e->target()))
					{
						// in case the parent class is a template class specifiy it as a typename
						if (!parentClass->typeArguments()->isEmpty())
							*fragment << "typename ";

						// prefix with parent class name and type arguments
						*fragment << parentClass->name();
						if (!parentClass->typeArguments()->isEmpty())
						{
							auto typeArgumentComposite = new CompositeFragment{parentClass->typeArguments(), "typeArgsList"};
							for (auto typeArgument : *parentClass->typeArguments())
								*typeArgumentComposite << typeArgument->nameNode();
							*fragment << typeArgumentComposite;
						}
						*fragment << "::";
					}
				}

			/*
			 * in case where the target is the parent class and the reference is not inside the body of a method
			 * qualify the reference with all ancestor classes.
			 */
			if (auto parentClass = e->firstAncestorOfType<Class>())
				if (e->target() == parentClass && !inBodyOfMethod)
				{
					auto currentParent = parentClass->firstAncestorOfType<Class>();
					while (currentParent)
					{
						/*
						 * if the current ancestor is a template class and the reference is in the results of a method or the
						 * type of a field add "typename"
						 */
						if (!currentParent->typeArguments()->isEmpty() && addPrefix)
							*fragment << "typename ";

						// prefix with ancestor class name and type arguments
						*fragment << currentParent->name();
						if (!currentParent->typeArguments()->isEmpty())
						{
							auto typeArgumentComposite =
									new CompositeFragment{currentParent->typeArguments(), "typeArgsList"};
							for (auto typeArgument : *currentParent->typeArguments())
								*typeArgumentComposite << typeArgument->nameNode();
							*fragment << typeArgumentComposite;
						}
						*fragment << "::";

						currentParent = currentParent->firstAncestorOfType<Class>();
					}
				}
		}

		// reference name and type arguments
		*fragment << e->name();
		if (!e->typeArguments()->isEmpty()) *fragment << list(e->typeArguments(), this, "typeArgsList");
	}

	// Flexible input support ===========================================================================================

	else if (DCast<EmptyExpression>(expression)); // do nothing
	else if (auto e = DCast<ErrorExpression>(expression))
	{
		if (!e->prefix().isEmpty()) *fragment << e->prefix();
		*fragment << visit(e->arg());
		if (!e->postfix().isEmpty()) *fragment << e->postfix();
	}
	else if (auto e = DCast<UnfinishedOperator>(expression))
	{
		QStringList result;

		for (int i=0; i< e->operands()->size(); ++i)
			*fragment << e->delimiters()->at(i) << visit(e->operands()->at(i));

		if (e->delimiters()->size() > e->operands()->size())
			*fragment << e->delimiters()->last();
	}
	else
	{
		throw CppExportException("Unhandled expression of type " + expression->typeName());
	}

	return fragment;
}

SourceFragment* ExpressionVisitor::visitFunctionPointer(PointerTypeExpression* functionPointer,
																				  const QString& name)
{
	auto functionTypeExpression = DCast<FunctionTypeExpression>(functionPointer->typeExpression());
	Q_ASSERT(functionTypeExpression);

	auto fragment = new CompositeFragment{functionPointer};
	*fragment << list(functionTypeExpression->results(), this)
				 << " (*" << name << ") "
				 << list(functionTypeExpression->arguments(), this, "argsList");
	return fragment;
}

PrintContext& ExpressionVisitor::printContext() { return data().get()->printContextStack_.last(); }

}
