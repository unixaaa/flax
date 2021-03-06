// Type.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "ir/type.h"

namespace fir
{
	PrimitiveType::PrimitiveType(size_t bits, Kind kind)
	{
		this->bitWidth = bits;
		this->primKind = kind;
	}


	PrimitiveType* PrimitiveType::getBool(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		// bitwidth = 1
		std::vector<PrimitiveType*> bools = tc->primitiveTypes[1];

		// how do we have more than 1?
		iceAssert(bools.size() == 1 && "???? more than 1 bool??");
		iceAssert(bools.front()->bitWidth == 1 && "not bool purporting to be bool???");

		return bools.front();
	}








	PrimitiveType* PrimitiveType::getIntWithBitWidthAndSignage(FTContext* tc, size_t bits, bool issigned)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		std::vector<PrimitiveType*> types = tc->primitiveTypes[bits];

		iceAssert(types.size() > 0 && "no types of this kind??");

		for(auto t : types)
		{
			iceAssert(t->bitWidth == bits);
			if(t->isIntegerType() && !t->isFloatingPointType() && (t->isSigned() == issigned))
				return t;
		}

		iceAssert(false);
		return 0;
	}

	PrimitiveType* PrimitiveType::getFloatWithBitWidth(FTContext* tc, size_t bits)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");


		std::vector<PrimitiveType*> types = tc->primitiveTypes[bits];

		iceAssert(types.size() > 0 && "no types of this kind??");

		for(auto t : types)
		{
			iceAssert(t->bitWidth == bits);
			if(t->isFloatingPointType())
				return t;
		}

		iceAssert(false);
		return 0;
	}



	PrimitiveType* PrimitiveType::getIntN(size_t bits, FTContext* tc)
	{
		return PrimitiveType::getIntWithBitWidthAndSignage(tc, bits, true);
	}

	PrimitiveType* PrimitiveType::getUintN(size_t bits, FTContext* tc)
	{
		return PrimitiveType::getIntWithBitWidthAndSignage(tc, bits, false);
	}





	PrimitiveType* PrimitiveType::getInt8(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return PrimitiveType::getIntWithBitWidthAndSignage(tc, 8, true);
	}

	PrimitiveType* PrimitiveType::getInt16(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return PrimitiveType::getIntWithBitWidthAndSignage(tc, 16, true);
	}

	PrimitiveType* PrimitiveType::getInt32(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return PrimitiveType::getIntWithBitWidthAndSignage(tc, 32, true);
	}

	PrimitiveType* PrimitiveType::getInt64(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return PrimitiveType::getIntWithBitWidthAndSignage(tc, 64, true);
	}





	PrimitiveType* PrimitiveType::getUint8(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return PrimitiveType::getIntWithBitWidthAndSignage(tc, 8, false);
	}

	PrimitiveType* PrimitiveType::getUint16(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return PrimitiveType::getIntWithBitWidthAndSignage(tc, 16, false);
	}

	PrimitiveType* PrimitiveType::getUint32(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return PrimitiveType::getIntWithBitWidthAndSignage(tc, 32, false);
	}

	PrimitiveType* PrimitiveType::getUint64(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return PrimitiveType::getIntWithBitWidthAndSignage(tc, 64, false);
	}




	PrimitiveType* PrimitiveType::getFloat32(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return PrimitiveType::getFloatWithBitWidth(tc, 32);
	}

	PrimitiveType* PrimitiveType::getFloat64(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return PrimitiveType::getFloatWithBitWidth(tc, 64);
	}







	// various
	std::string PrimitiveType::str()
	{
		// is primitive.
		std::string ret;

		if(this->primKind == Kind::Integer)
		{
			if(this->isSigned())	ret = "i";
			else					ret = "u";

			ret += std::to_string(this->getIntegerBitWidth());
		}
		else if(this->primKind == Kind::Floating)
		{
			// todo: bitWidth is applicable to both floats and ints,
			// but getIntegerBitWidth (obviously) works only for ints.
			if(this->bitWidth == 32)
				ret = "f32";

			else if(this->bitWidth == 64)
				ret = "f64";

			else
				iceAssert(!"????");
		}
		else
		{
			ret = "??";
		}

		return ret;
	}

	std::string PrimitiveType::encodedStr()
	{
		return this->str();
	}


	bool PrimitiveType::isTypeEqual(Type* other)
	{
		PrimitiveType* po = dynamic_cast<PrimitiveType*>(other);
		if(!po) return false;
		if(this->primKind != po->primKind) return false;
		if(this->bitWidth != po->bitWidth) return false;
		if(this->isTypeSigned != po->isTypeSigned) return false;

		return true;
	}




	bool PrimitiveType::isSigned()
	{
		iceAssert(this->primKind == Kind::Integer && "not integer type");
		return this->isTypeSigned;
	}

	size_t PrimitiveType::getIntegerBitWidth()
	{
		iceAssert(this->primKind == Kind::Integer && "not integer type");
		return this->bitWidth;
	}


	// float stuff
	size_t PrimitiveType::getFloatingPointBitWidth()
	{
		iceAssert(this->primKind == Kind::Floating && "not floating point type");
		return this->bitWidth;
	}




	PrimitiveType* PrimitiveType::reify(std::map<std::string, Type*> names, FTContext* tc)
	{
		// do nothing
		return this;
	}
}


















