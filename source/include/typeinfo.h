// typeinfo.h
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once
#include <stdint.h>
#include <string>
#include <vector>

namespace fir
{
	struct Type;
}

namespace Ast
{
	struct StructBase;
}

namespace Codegen
{
	enum class TypeKind;
}

struct TCInstance;
namespace TypeInfo
{
	void addNewType(TCInstance* ti, fir::Type* stype, Ast::StructBase* str, Codegen::TypeKind etype);
	void initialiseTypeInfo(TCInstance* ti);
	void generateTypeInfo(TCInstance* ti);
	size_t getIndexForType(TCInstance* ti, fir::Type* type);
}























