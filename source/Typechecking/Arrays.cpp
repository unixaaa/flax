// Arrays.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ast.h"
using namespace Ast;

fir::Type* SubscriptOp::doTypecheck(TCInstance* ti)
{
	return 0;
}

void SubscriptOp::generateDependencies(TCInstance* ti)
{
}
