// TopLevel.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
using namespace Ast;

fir::Type* NamespaceDef::doTypecheck(TCInstance *ti)
{
	return 0;
}

RootAst::RootAst(Parser::Pin pos) : Expr(pos)
{
}

fir::Type* RootAst::doTypecheck(TCInstance *ti)
{
	printf("root codegen\n");
	return 0;
}
