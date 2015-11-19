// module.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "errors.h"

#include <map>
#include <deque>
#include <string>
#include <vector>
#include <unordered_map>

#include "value.h"
#include "function.h"
#include "namespace.h"
#include "executiontarget.h"

namespace llvm
{
	class Module;
}

namespace fir
{
	struct Module
	{
		Module(std::string nm);

		std::string getModuleName();
		void setModuleName(std::string name);

		llvm::Module* translateToLlvm();

		std::string print();

		private:
		Namespace* topLevelNamespace = 0;
		std::string moduleName;
	};
}


































