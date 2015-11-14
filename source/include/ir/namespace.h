// namespace.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include <deque>
#include <string>

namespace fir
{
	struct Type;
	struct Value;

	struct Module;
	struct Function;
	struct StructType;
	struct FunctionType;
	struct GlobalVariable;

	struct Namespace
	{
		// methods
		Namespace(Module* parent);
		Namespace(Namespace* parent);


		std::deque<Function*> getFunctionsWithName(std::string name);
		std::deque<Function*> getFunctionsWithNameAndType(std::string name, FunctionType* ft);

		// fully qualified names, a list of namepaces
		std::deque<Function*> getFunctionsWithFQName(std::deque<std::string> qualifiedName);
		std::deque<Function*> getFunctionsWithFQNameAndType(std::deque<std::string> qualifiedName, FunctionType* ft);


		// types
		StructType* getNamedType(std::string name);

		// fully qualified names, a list of namepaces
		StructType* getFQNamedType(std::deque<std::string> qualifiedName);








		// fields
		std::string name;

		Module* mparent = 0;
		Namespace* parent = 0;

		std::deque<Namespace*> subNamespaces;
		std::deque<GlobalVariable*> globals;
		std::deque<StructType*> namedTypes;
		std::deque<Function*> functions;
	};
}










