#pragma once

#include "ClangAstConsumer.h"

//ENVISION
#include "OOModel/src/allOOModelNodes.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTContext.h"

class ClangAstFrontendActions : public clang::ASTFrontendAction
{
public:
    ClangAstFrontendActions(Model::Model* model, OOModel::Project* project);
    virtual clang::ASTConsumer *CreateASTConsumer(
      clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        InFile.str();
        Compiler.getASTConsumer();
      return new ClangAstConsumer(model_,project_);
    }

private:
    Model::Model* model_;
    OOModel::Project* project_;
};
