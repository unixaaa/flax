// ControlFlow.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
using namespace Ast;

fir::Type* DeferredStmt::doTypecheck(TCInstance *ti)
{
	return 0;
}

fir::Type* BracedBlock::doTypecheck(TCInstance *ti)
{
	return 0;
}

fir::Type* BreakStmt::doTypecheck(TCInstance *ti)
{
	return 0;
}

fir::Type* ContinueStmt::doTypecheck(TCInstance *ti)
{
	return 0;
}

fir::Type* ReturnStmt::doTypecheck(TCInstance *ti)
{
	return 0;
}



