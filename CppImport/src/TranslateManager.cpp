#include "TranslateManager.h"

TranslateManager::TranslateManager(Model::Model* model, OOModel::Project* project) :
    model_(model), project_(project)
{
}

void TranslateManager::insertClass(clang::CXXRecordDecl* rDecl, OOModel::Class *ooClass)
{
    //if rdecl is not managed yet add it:
    if(!classMap_.contains(rDecl))
    {
        std::cout << "-------------------------->CLASS " << rDecl->getName().str() << " NOT IN MAP " << std::endl;
        classMap_.insert(rDecl,ooClass);
    }
}

OOModel::Method* TranslateManager::insertMethodDecl(clang::CXXMethodDecl* mDecl)
{
    OOModel::Method* method = nullptr;

    if(!methodMap_.contains(mDecl))
    {
        if(methodMap_.contains(mDecl->getCorrespondingMethodInClass(mDecl->getParent())))
            std::cout << "IS DECLARED BEFORE" << std::endl;
        //Look if there is a function with same name in map
        QMap<clang::CXXMethodDecl*,OOModel::Method*>::iterator it = methodMap_.begin();
        for(;it!=methodMap_.end();++it)
        {
            clang::CXXMethodDecl* inMapDecl = it.key();
            if(mDecl->getName().equals(inMapDecl->getName()) &&
                    !inMapDecl->isThisDeclarationADefinition() && mDecl->isThisDeclarationADefinition())
            {
                //found a pair with same name and only one is defined
                if((mDecl->getResultType() == inMapDecl->getResultType()) &&
                        (mDecl->param_size() == inMapDecl->param_size()))
                {
                    bool matching = true;
                    for(unsigned i = 0; i < mDecl->param_size(); i++)
                    {
                        if(mDecl->getParamDecl(i)->getType() != inMapDecl->getParamDecl(i)->getType())
                            matching = false;
                    }
                    if(matching)
                    {
                        method = it.value();
                        break;
                    }
                }
            }
        }
        // check if method node exists or else create one
        method = method ? method : addNewMethod(mDecl);
    }
    return method;
}

OOModel::Field *TranslateManager::insertField(clang::FieldDecl* fDecl)
{
    clang::CXXRecordDecl* parentClass = llvm::dyn_cast<clang::CXXRecordDecl>(fDecl->getParent());
    if(parentClass && classMap_.contains(parentClass))
    {
        OOModel::Field* field = new OOModel::Field();
        OOModel::Expression* type = CppImportUtilities::convertClangType(fDecl->getType());
        if(type) field->setTypeExpression(type);
        field->setName(QString::fromStdString(fDecl->getName().str()));
        classMap_.value(parentClass)->fields()->append(field);
        return field;
    }
    return nullptr;
}

void TranslateManager::insertVar(clang::VarDecl* vDecl, OOModel::VariableDeclaration* ooVarDecl)
{
    if(!varMap_.contains(vDecl))
    {
        varMap_.insert(vDecl,ooVarDecl);
    }
    else
    {
        std::cout << "---------->VAR : " << vDecl->getName().str() << " IS ALREADY IN THE MAP" << std::endl;
    }



    if(vDecl->getParentFunctionOrMethod())
    {
        std::cout << "HAS PARENT FUNCTION OR METHOD ________" << vDecl->getName().str() << std::endl;
        //add to method
        clang::FunctionDecl* parentFunc = llvm::dyn_cast<clang::FunctionDecl>(vDecl->getParentFunctionOrMethod());
        if(methodMap_.contains(llvm::dyn_cast<clang::CXXMethodDecl>(parentFunc)))
        {
            std::cout << "FOUND A SUITABLE FUNCTION FOR VAR ------------> " << vDecl->getName().str() << std::endl;
        }
        ooVarDecl->getAllAttributes();
    }
}

OOModel::IfStatement *TranslateManager::insertIfStmt(clang::IfStmt* ifStmt)
{
    OOModel::IfStatement* ooIfStmt = new OOModel::IfStatement();
    ifStmtMap_.insert(ifStmt,ooIfStmt);
    return ooIfStmt;
}

OOModel::VariableDeclaration *TranslateManager::getVar(clang::VarDecl* vDecl)
{
    if(varMap_.contains(vDecl))
        return varMap_.value(vDecl);
    return nullptr;
}

OOModel::Method* TranslateManager::addNewMethod(clang::CXXMethodDecl* mDecl)
{
    // add a new method
    OOModel::Method* method = new OOModel::Method();
    method->setName(QString::fromStdString(mDecl->getName().str()));
    // process result type
    OOModel::Expression* restype = CppImportUtilities::convertClangType(mDecl->getResultType());
    if(restype)
    {
        OOModel::FormalResult* methodResult = new OOModel::FormalResult();
        methodResult->setTypeExpression(restype);
        method->results()->append(methodResult);
    }
    // process arguments
    clang::FunctionDecl::param_const_iterator it = mDecl->param_begin();
    for(;it != mDecl->param_end();++it)
    {
        OOModel::FormalArgument* arg = new OOModel::FormalArgument();
        arg->setName(QString::fromStdString((*it)->getName().str()));
        OOModel::Expression* type = CppImportUtilities::convertClangType((*it)->getType());
        if(type) arg->setTypeExpression(type);
        method->arguments()->append(arg);
    }
    // find the correct class to add the method
    if(classMap_.contains(mDecl->getParent()))
    {
        OOModel::Class* parent = classMap_.value(mDecl->getParent());
        parent->methods()->append(method);
    }
    else
    {
        std::cout << "METHOD DECL NO PARENT FOUND" << std::endl;
    }

    methodMap_.insert(mDecl,method);

    return method;
}
