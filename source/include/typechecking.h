// typechecking.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <stdint.h>
#include <stddef.h>

#include "defs.h"

struct TCInstance
{
	TCInstance(ModuleInfo* m) : mi(m) { }

	ModuleInfo* mi = 0;
};




TCInstance* doTypechecking(ModuleInfo* mi);












