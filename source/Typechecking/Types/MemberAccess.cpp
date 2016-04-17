// MemberAccess.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
#include "typechecking.h"

#include "ir/namespace.h"

using namespace Ast;


std::string print(DotOp* op)
{
	std::string l = "*";
	std::string r = "*";

	if(VarRef* vl = dynamic_cast<VarRef*>(op->leftExpr))
	{
		l = vl->name;
	}
	else if(FuncCall* fl = dynamic_cast<FuncCall*>(op->leftExpr))
	{
		l = fl->name + "(...)";
	}
	else if(DotOp* dl = dynamic_cast<DotOp*>(op->leftExpr))
	{
		l = print(dl);
	}

	if(VarRef* vr = dynamic_cast<VarRef*>(op->rightExpr))
	{
		r = vr->name;
	}
	else if(FuncCall* fr = dynamic_cast<FuncCall*>(op->rightExpr))
	{
		r = fr->name + "(...)";
	}
	else if(DotOp* dr = dynamic_cast<DotOp*>(op->rightExpr))
	{
		r = print(dr);
	}

	return "(" + l + "." + r + ")";
}





































enum class ExprType
{
	Invalid,
	NamespaceIdent,
	ClassName,
	FunctionCall,
	Tuple,
	Array,
	Identifier,
};

static void _linearise(TCInstance* ti, DotOp* op, std::deque<Expr*>* list)
{
	// add from the left.
	if(DotOp* dl = dynamic_cast<DotOp*>(op->leftExpr))
	{
		_linearise(ti, dl, list);
	}
	else
	{
		list->push_back(op->leftExpr);
	}

	list->push_back(op->rightExpr);
}

static std::deque<Expr*> linearise(TCInstance* ti, DotOp* op)
{
	std::deque<Expr*> list;
	_linearise(ti, op, &list);

	return list;
}

fir::Type* DotOp::doTypecheck(TCInstance* ti)
{
	// typecheck for dotop must only be called from the top-level one.
	// else we end up checking multiple times.
	auto list = linearise(ti, this);

	std::deque<std::string> namespaces;
	std::deque<std::string> classes;

	fir::Namespace* cur = ti->currentNamespace;
	for(auto expr : list)
	{

	}

	printf("[ %s ]\n", print(this).c_str());
	return 0;
}





































