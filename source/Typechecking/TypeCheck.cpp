// TypeCheck.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "compiler.h"
#include "typechecking.h"

TCInstance* doTypechecking(ModuleInfo* mi)
{
	TCInstance* ti = new TCInstance(mi);
	mi->tci = ti;

	iceAssert(mi->rootAst != 0);
	mi->rootAst->doTypecheck(ti);

	return ti;
}
