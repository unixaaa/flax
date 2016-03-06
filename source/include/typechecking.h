// typechecking.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "defs.h"
#include "dependency.h"

struct TCInstance
{
	TCInstance(ModuleInfo* m) : mi(m) { }

	ModuleInfo* mi = 0;

	fir::Namespace* currentNamespace = 0;
	Codegen::DependencyGraph graph;





	// helper functions
	std::deque<std::string> getCurrentNamespaceStack();

	fir::Namespace* pushNamespace(std::string name);
	void popNamespace();


	std::deque<std::deque<std::pair<std::string, fir::Type*>>> identifierStack;
	std::pair<std::string, fir::Type*> getIdentifier(std::string name);

	void addIdentifier(std::string name, fir::Type* type);

	void pushIdentifierStack();
	void popIdentifierStack();
};

std::deque<std::string> deconstructTupleString(std::string tup);
TCInstance* doTypechecking(ModuleInfo* mi);









































