// Codegen.cpp
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "codegen.h"

CGInstance* doCodegen(ModuleInfo* mi, TCInstance* ti)
{
	CGInstance* ci = new CGInstance(mi);
	return ci;
}
