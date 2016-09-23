// Blocks.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.


#include "ast.h"
#include "codegen.h"

using namespace Ast;
using namespace Codegen;

Result_t BracedBlock::codegen(CodegenInstance* cgi, fir::Value* extra)
{
	Result_t lastval(0, 0);
	cgi->pushScope();

	bool broke = false;
	for(Expr* e : this->statements)
	{
		if(!broke)
		{
			lastval = e->codegen(cgi);
		}

		if(lastval.type == ResultType::BreakCodegen)
			broke = true;		// don't generate the rest of the code. cascade the BreakCodegen value into higher levels
	}

	// ok, now do the deferred expressions.
	for(auto e : this->deferredStatements)
		e->codegen(cgi);


	cgi->popScope();
	return lastval;
}

fir::Type* BracedBlock::getType(CodegenInstance* cgi, bool allowFail, fir::Value* extra)
{
	iceAssert(0);
}


Result_t Break::codegen(CodegenInstance* cgi, fir::Value* extra)
{
	BracedBlockScope* cs = cgi->getCurrentBracedBlockScope();
	if(!cs)
	{
		error(this, "Break can only be used inside loop bodies");
	}

	iceAssert(cs->first);
	iceAssert(cs->first->body);
	iceAssert(cs->second.first);
	iceAssert(cs->second.second);

	// evaluate all deferred statements
	for(auto e : cs->first->body->deferredStatements)
		e->codegen(cgi);

	// for break, we go to the ending block
	cgi->builder.CreateUnCondBranch(cs->second.second);
	return Result_t(0, 0, ResultType::BreakCodegen);
}

fir::Type* Break::getType(CodegenInstance* cgi, bool allowFail, fir::Value* extra)
{
	iceAssert(0);
}







Result_t Continue::codegen(CodegenInstance* cgi, fir::Value* extra)
{
	BracedBlockScope* cs = cgi->getCurrentBracedBlockScope();
	if(!cs)
	{
		error(this, "Continue can only be used inside loop bodies");
	}

	iceAssert(cs->first);
	iceAssert(cs->first->body);
	iceAssert(cs->second.first);
	iceAssert(cs->second.second);


	// evaluate all deferred statements
	for(auto e : cs->first->body->deferredStatements)
		e->codegen(cgi);


	// for continue, we go to the beginning (loop) block
	cgi->builder.CreateUnCondBranch(cs->second.first);
	return Result_t(0, 0, ResultType::BreakCodegen);
}

fir::Type* Continue::getType(CodegenInstance* cgi, bool allowFail, fir::Value* extra)
{
	iceAssert(0);
}




Result_t Return::codegen(CodegenInstance* cgi, fir::Value* extra)
{
	// first, evaluate the deferred expressions

	// 1. in the current bracedblockscope
	// 2. iterate upwards until we get to the top
	{
		for(size_t i = cgi->blockStack.size(); i-- > 0;)
		{
			BracedBlockScope bbs = cgi->blockStack[i];
			iceAssert(bbs.first);
			iceAssert(bbs.first->body);

			for(auto e : bbs.first->body->deferredStatements)
				e->codegen(cgi);
		}
	}

	// 3. then call the function ones.
	{
		Func* fn = cgi->getCurrentFunctionScope();
		iceAssert(fn);

		BracedBlock* bb = fn->block;
		iceAssert(bb);

		for(auto e : bb->deferredStatements)
			e->codegen(cgi);
	}




	if(this->val)
	{
		auto res = this->val->codegen(cgi).result;
		fir::Value* left = res.first;

		fir::Function* f = cgi->builder.getCurrentBlock()->getParentFunction();
		iceAssert(f);

		this->actualReturnValue = cgi->autoCastType(f->getReturnType(), left, res.second);

		return Result_t(cgi->builder.CreateReturn(this->actualReturnValue), res.second, ResultType::BreakCodegen);
	}
	else
	{
		return Result_t(cgi->builder.CreateReturnVoid(), 0, ResultType::BreakCodegen);
	}
}

fir::Type* Return::getType(CodegenInstance* cgi, bool allowFail, fir::Value* extra)
{
	if(this->val) return this->val->getType(cgi);
	else return fir::PrimitiveType::getVoid();
}



Result_t DeferredExpr::codegen(CodegenInstance* cgi, fir::Value* extra)
{
	return expr->codegen(cgi);
}

fir::Type* DeferredExpr::getType(CodegenInstance* cgi, bool allowFail, fir::Value* extra)
{
	return this->expr->getType(cgi);
}








