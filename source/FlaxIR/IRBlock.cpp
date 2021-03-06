// IRBlock.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ir/function.h"
#include "ir/block.h"

namespace fir
{
	IRBlock::IRBlock() : Value(Type::getVoid())
	{
		this->parentFunction = 0;
	}

	IRBlock::IRBlock(Function* fn) : Value(Type::getVoid())
	{
		this->parentFunction = fn;
		this->addUser(fn);
	}

	void IRBlock::setFunction(Function* fn)
	{
		this->parentFunction = fn;
		this->addUser(fn);
	}

	Function* IRBlock::getParentFunction()
	{
		return this->parentFunction;
	}

	void IRBlock::eraseFromParentFunction()
	{
		iceAssert(this->parentFunction && "no function");
		std::deque<IRBlock*>& blist = this->parentFunction->getBlockList();

		for(auto it = blist.begin(); it != blist.end(); it++)
		{
			if(*it == this)
			{
				blist.erase(it);
				return;
			}
		}

		iceAssert(0 && "not in function");
	}

	std::deque<Instruction*>& IRBlock::getInstructions()
	{
		return this->instructions;
	}
}






