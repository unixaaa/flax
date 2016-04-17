// Functions.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
#include "typechecking.h"

#include "ir/namespace.h"

using namespace Ast;






fir::Type* FuncDecl::doTypecheck(TCInstance* ti)
{
	// do return type first
	// get it.

	if(this->genericTypeParameters.size() > 0)
	{
		printf("skipping generic");
		return 0;
	}

	fir::Type* returnType = fir::Type::fromBuiltin(this->returnType);

	if(returnType == 0)
	{
		returnType = ti->currentNamespace->getNamedType(this->returnType);
		if(returnType == 0)
			error(this, "Unknown type '%s'", this->returnType.c_str());
	}

	std::deque<fir::Type*> argTypes;
	for(auto arg : this->arguments)
	{
		fir::Type* ty = fir::Type::fromBuiltin(arg->type);
		if(ty == 0)
			ty = ti->currentNamespace->getNamedType(arg->type);

		if(ty == 0)
			error(this, "Unknown type '%s'", arg->type.c_str());

		argTypes.push_back(ty);
	}

	return fir::FunctionType::get(argTypes, returnType, this->isVarArg);
}
















static fir::Type* checkReturnsInIfStmt(TCInstance* ti, IfStmt* ifst, bool* allPathsPassed);

static fir::Type* checkReturnTypeOfBlock(TCInstance* ti, BracedBlock* bb, Expr** retExpr, bool* allPathsPassed)
{
	*allPathsPassed = true;

	fir::Type* final = 0;
	for(auto st : bb->statements)
	{
		if(ReturnStmt* rt = dynamic_cast<ReturnStmt*>(st))
		{
			if(std::find(bb->statements.begin(), bb->statements.end(), st) != bb->statements.end() - 1)
				warn(st, "Statements after this will not be executed");

			if(retExpr) *retExpr = rt;
			return rt->doTypecheck(ti);
		}
		else if(IfStmt* is = dynamic_cast<IfStmt*>(st))
		{
			bool passed = 0;
			final = checkReturnsInIfStmt(ti, is, &passed);

			*allPathsPassed = passed;
		}
		else
		{
			if(retExpr) *retExpr = st;
			final = st->doTypecheck(ti);
		}
	}

	if(final == 0) *allPathsPassed = 0;
	return final;
}

static fir::Type* checkReturnsInIfStmt(TCInstance* ti, IfStmt* ifst, bool* allPathsPassed)
{
	// default is true.
	*allPathsPassed = true;

	// check if, check else ifs, check else.
	fir::Type* ret = 0;
	for(auto acase : ifst->cases)
	{
		// since this is only checked once, check the conditions as well.
		acase.first->doTypecheck(ti);

		Expr* retExpr = 0;
		fir::Type* cur = checkReturnTypeOfBlock(ti, acase.second, &retExpr, allPathsPassed);

		if(ret != 0 && cur != 0 && ret != cur)
		{
			error(retExpr, "Mismatched return type: earlier expression had type %s, current expression has type %s.",
				ret->str().c_str(), cur->str().c_str());
		}
		else if(ret != 0 && cur == 0)
		{
			*allPathsPassed = false;
		}
		else
		{
			ret = cur;
		}
	}

	{
		Expr* retExpr = 0;
		fir::Type* cur = checkReturnTypeOfBlock(ti, ifst->elseCase, &retExpr, allPathsPassed);

		if(ret != 0 && cur != 0 && ret != cur)
		{
			error(retExpr, "Mismatched return type: earlier expression had type %s, current expression has type %s.",
				ret->str().c_str(), cur->str().c_str());
		}
		else if(ret != 0 && cur == 0)
		{
			*allPathsPassed = false;
		}
		else
		{
			ret = cur;
		}
	}

	if(ret == 0) *allPathsPassed = 0;
	return ret;
}

static fir::Type* checkFunctionReturnsInAllPaths(TCInstance* ti, FunctionDef* func, bool* allPassed)
{
	Expr* lol;
	fir::Type* ret = checkReturnTypeOfBlock(ti, func->funcBody, &lol, allPassed);
	if(ret == 0) ret = fir::PrimitiveType::getVoid();

	printf("return type of function %s was %s.\n", func->funcDecl->name.c_str(), ret ? ret->str().c_str() : "unknown");
	return ret;
}












fir::Type* FunctionDef::doTypecheck(TCInstance* ti)
{
	// first thing, decl needs to be type checked.
	// this involves figuring out if the types of parameters exist, etc.

	// however, if we are generic, we have to defer until we are used
	// aka instantiated
	if(this->funcDecl->genericTypeParameters.size() > 0) return 0;

	fir::FunctionType* ftype = this->funcDecl->doTypecheck(ti)->toFunctionType();
	iceAssert(ftype);


	// check the declarations
	ti->pushIdentifierStack();

	for(size_t i = 0; i < ftype->getArgumentTypes().size(); i++)
		ti->addIdentifier(this->funcDecl->arguments[i]->name, ftype->getArgumentN(i));

	// check the statements inside.
	// this already does the typecheck on each expression.

	bool allPassed = 0;
	fir::Type* givenType = checkFunctionReturnsInAllPaths(ti, this, &allPassed);
	if(!allPassed && !ftype->getReturnType()->isVoidType())
	{
		error(this, "Not all code paths returned a value (return type %s required)", ftype->getReturnType()->str().c_str());
	}
	if(!givenType->isTypeEqual(ftype->getReturnType()))
	{
		error(this, "Provided expressions (type %s) do not corroborate with declared return type %s", givenType->str().c_str(),
			ftype->getReturnType()->str().c_str());
	}

	ti->popIdentifierStack();
	return ftype;
}



































