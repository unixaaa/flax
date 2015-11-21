// semantic.h
// Copyright (c) 2014 - 2015, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <string>

#include "defs.h"
#include "dependency.h"

namespace Ast
{
	struct Root;
	struct Expr;
	struct VarDecl;
}

enum class VarState
{
	Invalid = 0,

	NoValue,
	ValidAlloc,
	ValidStack,
	Deallocated,
	ModifiedAlloc,
};

struct VarDef
{
	std::string name;
	VarState state;

	Ast::Expr* expr = 0;
	Ast::VarDecl* decl = 0;
	bool visited = false;
};


namespace SemAnalysis
{
	void rewriteDotOperators(TCInstance* ti);
	void analyseVarUsage(TCInstance* ti);
}


























