// executiontarget.h
// Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#pragma once

#include <stdint.h>
#include <stddef.h>


namespace fir
{
	struct Type;

	struct ExecutionTarget
	{
		size_t getBitsPerByte();
		size_t getPointerWidthInBits();
		size_t getTypeSizeInBits(Type* type);

		// 64-bits
		static ExecutionTarget* getLP64();
		static ExecutionTarget* getLLP64();
		static ExecutionTarget* getILP64();

		// 32-bits
		static ExecutionTarget* getILP32();


		static ExecutionTarget* getAppropriate();

		private:
		ExecutionTarget(size_t ptrSize, size_t byteSize, size_t shortSize, size_t intSize, size_t longSize);

		size_t psize;
		size_t bsize;
		size_t ssize;
		size_t isize;
		size_t lsize;
	};
}

