// Type.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "errors.h"
#include "ir/type.h"

namespace Codegen
{
	std::string unwrapPointerType(std::string, int*);
}

namespace fir
{
	// NOTE: some global state.
	// structs

	Type* FTContext::normaliseType(Type* type)
	{
		if(Type::areTypesEqual(type, this->voidType))
			return this->voidType;

		size_t ind = 0;
		if(type->isPointerType())
			ind = type->toPointerType()->getIndirections();

		std::vector<Type*>& list = this->typeCache[ind];

		// find in the list.
		// todo: make this more efficient
		for(auto t : list)
		{
			if(Type::areTypesEqual(t, type))
				return t;
		}

		list.push_back(type);
		return type;
	}


	static FTContext* defaultFTContext = 0;
	void setDefaultFTContext(FTContext* tc)
	{
		iceAssert(tc && "Tried to set null type context as default");
		defaultFTContext = tc;
	}

	FTContext* getDefaultFTContext()
	{
		if(defaultFTContext == 0)
		{
			// iceAssert(0);
			defaultFTContext = createFTContext();
		}

		return defaultFTContext;
	}

	std::string VoidType::str()
	{
		return "void";
	}

	std::string VoidType::encodedStr()
	{
		return "void";
	}

	bool VoidType::isTypeEqual(Type* other)
	{
		return other && other->isVoidType();
	}

	Type* VoidType::reify(std::map<std::string, Type*> names, FTContext* tc)
	{
		// do nothing
		return this;
	}

	VoidType::VoidType()
	{
		// nothing
	}

	VoidType* VoidType::get(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		return tc->voidType;
	}

	FTContext* createFTContext()
	{
		FTContext* tc = new FTContext();

		// fill in primitives

		// void.
		tc->voidType = new VoidType();

		// bool
		{
			PrimitiveType* t = new PrimitiveType(1, PrimitiveType::Kind::Integer);
			t->isTypeSigned = false;

			tc->primitiveTypes[1].push_back(t);
			tc->typeCache[0].push_back(t);
		}




		// int8
		{
			PrimitiveType* t = new PrimitiveType(8, PrimitiveType::Kind::Integer);
			t->isTypeSigned = true;

			tc->primitiveTypes[8].push_back(t);
			tc->typeCache[0].push_back(t);
		}
		// int16
		{
			PrimitiveType* t = new PrimitiveType(16, PrimitiveType::Kind::Integer);
			t->isTypeSigned = true;

			tc->primitiveTypes[16].push_back(t);
			tc->typeCache[0].push_back(t);
		}
		// int32
		{
			PrimitiveType* t = new PrimitiveType(32, PrimitiveType::Kind::Integer);
			t->isTypeSigned = true;

			tc->primitiveTypes[32].push_back(t);
			tc->typeCache[0].push_back(t);
		}
		// int64
		{
			PrimitiveType* t = new PrimitiveType(64, PrimitiveType::Kind::Integer);
			t->isTypeSigned = true;

			tc->primitiveTypes[64].push_back(t);
			tc->typeCache[0].push_back(t);
		}




		// uint8
		{
			PrimitiveType* t = new PrimitiveType(8, PrimitiveType::Kind::Integer);
			t->isTypeSigned = false;

			tc->primitiveTypes[8].push_back(t);
			tc->typeCache[0].push_back(t);
		}
		// uint16
		{
			PrimitiveType* t = new PrimitiveType(16, PrimitiveType::Kind::Integer);
			t->isTypeSigned = false;

			tc->primitiveTypes[16].push_back(t);
			tc->typeCache[0].push_back(t);
		}
		// uint32
		{
			PrimitiveType* t = new PrimitiveType(32, PrimitiveType::Kind::Integer);
			t->isTypeSigned = false;

			tc->primitiveTypes[32].push_back(t);
			tc->typeCache[0].push_back(t);
		}
		// uint64
		{
			PrimitiveType* t = new PrimitiveType(64, PrimitiveType::Kind::Integer);
			t->isTypeSigned = false;

			tc->primitiveTypes[64].push_back(t);
			tc->typeCache[0].push_back(t);
		}


		// float32
		{
			PrimitiveType* t = new PrimitiveType(32, PrimitiveType::Kind::Floating);
			t->isTypeSigned = false;

			tc->primitiveTypes[32].push_back(t);
			tc->typeCache[0].push_back(t);
		}
		// float64
		{
			PrimitiveType* t = new PrimitiveType(64, PrimitiveType::Kind::Floating);
			t->isTypeSigned = false;

			tc->primitiveTypes[64].push_back(t);
			tc->typeCache[0].push_back(t);
		}

		return tc;
	}






	std::string Type::typeListToString(std::initializer_list<Type*> types)
	{
		return typeListToString(std::vector<Type*>(types.begin(), types.end()));
	}

	std::string Type::typeListToString(std::deque<Type*> types)
	{
		return typeListToString(std::vector<Type*>(types.begin(), types.end()));
	}

	std::string Type::typeListToString(std::vector<Type*> types)
	{
		// print types
		std::string str = "{ ";
		for(auto t : types)
			str += t->str() + ", ";

		if(str.length() > 2)
			str = str.substr(0, str.length() - 2);

		return str + " }";
	}


	bool Type::areTypeListsEqual(std::vector<Type*> a, std::vector<Type*> b)
	{
		if(a.size() != b.size()) return false;
		if(a.size() == 0 && b.size() == 0) return true;

		for(size_t i = 0; i < a.size(); i++)
		{
			if(!Type::areTypesEqual(a[i], b[i]))
				return false;
		}

		return true;
	}

	bool Type::areTypeListsEqual(std::deque<Type*> a, std::deque<Type*> b)
	{
		return areTypeListsEqual(std::vector<Type*>(a.begin(), a.end()), std::vector<Type*>(b.begin(), b.end()));
	}

	bool Type::areTypeListsEqual(std::initializer_list<Type*> a, std::initializer_list<Type*> b)
	{
		return areTypeListsEqual(std::vector<Type*>(a.begin(), a.end()), std::vector<Type*>(b.begin(), b.end()));
	}



	Type* Type::getPointerTo(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		size_t inds = 0;
		fir::Type* base = this;

		while(base->isPointerType())
		{
			base = base->getPointerElementType();
			inds++;
		}

		PointerType* newType = new PointerType(inds + 1, this);

		// get or create.
		newType = dynamic_cast<PointerType*>(tc->normaliseType(newType));
		iceAssert(newType);
		return newType;
	}

	Type* Type::getPointerElementType(FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		if(!this->isPointerType())
			error("type is not a pointer (%s)", this->str().c_str());


		PointerType* ptrthis = dynamic_cast<PointerType*>(this);
		iceAssert(ptrthis);

		Type* newType = ptrthis->baseType;
		newType = tc->normaliseType(newType);

		return newType;
	}

	bool Type::areTypesEqual(Type* a, Type* b)
	{
		return a->isTypeEqual(b);
	}



	Type* Type::getIndirectedType(ssize_t times, FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");

		Type* ret = this;
		if(times > 0)
		{
			for(ssize_t i = 0; i < times; i++)
			{
				// auto old = ret;
				ret = ret->getPointerTo();
			}
		}
		else if(times < 0)
		{
			for(ssize_t i = 0; i < -times; i++)
				ret = ret->getPointerElementType();
		}

		ret = tc->normaliseType(ret);
		return ret;
	}


	Type* Type::fromBuiltin(std::string builtin, FTContext* tc)
	{
		if(!tc) tc = getDefaultFTContext();
		iceAssert(tc && "null type context");


		int indirections = 0;
		builtin = Codegen::unwrapPointerType(builtin, &indirections);

		Type* real = 0;

		if(builtin == INT8_TYPE_STRING)				real = Type::getInt8(tc);
		else if(builtin == INT16_TYPE_STRING)		real = Type::getInt16(tc);
		else if(builtin == INT32_TYPE_STRING)		real = Type::getInt32(tc);
		else if(builtin == INT64_TYPE_STRING)		real = Type::getInt64(tc);
		else if(builtin == INTUNSPEC_TYPE_STRING)	real = Type::getInt64(tc);

		else if(builtin == UINT8_TYPE_STRING)		real = Type::getUint8(tc);
		else if(builtin == UINT16_TYPE_STRING)		real = Type::getUint16(tc);
		else if(builtin == UINT32_TYPE_STRING)		real = Type::getUint32(tc);
		else if(builtin == UINT64_TYPE_STRING)		real = Type::getUint64(tc);
		else if(builtin == UINTUNSPEC_TYPE_STRING)	real = Type::getUint64(tc);

		else if(builtin == FLOAT32_TYPE_STRING)		real = Type::getFloat32(tc);

		// float is implicit double.
		else if(builtin == FLOAT64_TYPE_STRING)		real = Type::getFloat64(tc);
		else if(builtin == FLOAT_TYPE_STRING)		real = Type::getFloat64(tc);

		else if(builtin == STRING_TYPE_STRING)		real = Type::getStringType();
		else if(builtin == CHARACTER_TYPE_STRING)	real = Type::getCharType();

		else if(builtin == BOOL_TYPE_STRING)		real = Type::getBool(tc);
		else if(builtin == VOID_TYPE_STRING)		real = Type::getVoid(tc);

		else return 0;

		iceAssert(real);

		real = real->getIndirectedType(indirections);
		return real;
	}


	Type* Type::fromLlvmType(fir::Type* ltype, std::deque<bool> signage)
	{
		iceAssert(0);
		return Type::getVoid();
	}







	bool Type::isPointerTo(Type* other)
	{
		return other->getPointerTo() == this;
	}

	bool Type::isPointerElementOf(Type* other)
	{
		return this->getPointerTo() == other;
	}

	PrimitiveType* Type::toPrimitiveType()
	{
		auto t = dynamic_cast<PrimitiveType*>(this);
		if(t == 0) error("not primitive type");
		return t;
	}

	FunctionType* Type::toFunctionType()
	{
		auto t = dynamic_cast<FunctionType*>(this);
		if(t == 0) error("not function type");
		return t;
	}

	PointerType* Type::toPointerType()
	{
		auto t = dynamic_cast<PointerType*>(this);
		if(t == 0) error("not pointer type");
		return t;
	}

	StructType* Type::toStructType()
	{
		auto t = dynamic_cast<StructType*>(this);
		if(t == 0) error("not struct type");
		return t;
	}

	ClassType* Type::toClassType()
	{
		auto t = dynamic_cast<ClassType*>(this);
		if(t == 0) error("not class type");
		return t;
	}

	TupleType* Type::toTupleType()
	{
		auto t = dynamic_cast<TupleType*>(this);
		if(t == 0) error("not tuple type");
		return t;
	}

	ArrayType* Type::toArrayType()
	{
		auto t = dynamic_cast<ArrayType*>(this);
		if(t == 0) error("not array type");
		return t;
	}

	LLVariableArrayType* Type::toLLVariableArrayType()
	{
		auto t = dynamic_cast<LLVariableArrayType*>(this);
		if(t == 0) error("not llva type");
		return t;
	}

	ParametricType* Type::toParametricType()
	{
		auto t = dynamic_cast<ParametricType*>(this);
		if(t == 0) error("not type parameter");
		return t;
	}

	StringType* Type::toStringType()
	{
		auto t = dynamic_cast<StringType*>(this);
		if(t == 0) error("not string type");
		return t;
	}

	CharType* Type::toCharType()
	{
		auto t = dynamic_cast<CharType*>(this);
		if(t == 0) error("not char type");
		return t;
	}
















	bool Type::isStructType()
	{
		return dynamic_cast<StructType*>(this) != 0;
	}

	bool Type::isTupleType()
	{
		return dynamic_cast<TupleType*>(this) != 0;
	}

	bool Type::isClassType()
	{
		return dynamic_cast<ClassType*>(this) != 0;
	}

	bool Type::isPackedStruct()
	{
		return this->isStructType()
			&& (this->toStructType()->isTypePacked);
	}

	bool Type::isArrayType()
	{
		return dynamic_cast<ArrayType*>(this) != 0;
	}

	bool Type::isFloatingPointType()
	{
		auto t = dynamic_cast<PrimitiveType*>(this);
		return t != 0 && t->primKind == PrimitiveType::Kind::Floating;
	}

	bool Type::isIntegerType()
	{
		auto t = dynamic_cast<PrimitiveType*>(this);
		return t != 0 && t->primKind == PrimitiveType::Kind::Integer;
	}

	bool Type::isSignedIntType()
	{
		auto t = dynamic_cast<PrimitiveType*>(this);
		return t != 0 && t->primKind == PrimitiveType::Kind::Integer && t->isSigned();
	}

	bool Type::isFunctionType()
	{
		return dynamic_cast<FunctionType*>(this) != 0;
	}

	bool Type::isPrimitiveType()
	{
		return dynamic_cast<PrimitiveType*>(this) != 0;
	}

	bool Type::isPointerType()
	{
		return dynamic_cast<PointerType*>(this) != 0;
	}

	bool Type::isVoidType()
	{
		return dynamic_cast<VoidType*>(this) != 0;
	}

	bool Type::isLLVariableArrayType()
	{
		return dynamic_cast<LLVariableArrayType*>(this) != 0;
	}

	bool Type::isNullPointer()
	{
		return this == PrimitiveType::getVoid()->getPointerTo();
	}

	bool Type::isParametricType()
	{
		return dynamic_cast<ParametricType*>(this) != 0;
	}

	bool Type::isStringType()
	{
		return dynamic_cast<StringType*>(this) != 0;
	}

	bool Type::isCharType()
	{
		return dynamic_cast<CharType*>(this) != 0;
	}











	// static conv. functions

	VoidType* Type::getVoid(FTContext* tc)
	{
		return VoidType::get(tc);
	}

	PrimitiveType* Type::getBool(FTContext* tc)
	{
		return PrimitiveType::getBool(tc);
	}

	PrimitiveType* Type::getInt8(FTContext* tc)
	{
		return PrimitiveType::getInt8(tc);
	}

	PrimitiveType* Type::getInt16(FTContext* tc)
	{
		return PrimitiveType::getInt16(tc);
	}

	PrimitiveType* Type::getInt32(FTContext* tc)
	{
		return PrimitiveType::getInt32(tc);
	}

	PrimitiveType* Type::getInt64(FTContext* tc)
	{
		return PrimitiveType::getInt64(tc);
	}

	PrimitiveType* Type::getUint8(FTContext* tc)
	{
		return PrimitiveType::getUint8(tc);
	}

	PrimitiveType* Type::getUint16(FTContext* tc)
	{
		return PrimitiveType::getUint16(tc);
	}

	PrimitiveType* Type::getUint32(FTContext* tc)
	{
		return PrimitiveType::getUint32(tc);
	}

	PrimitiveType* Type::getUint64(FTContext* tc)
	{
		return PrimitiveType::getUint64(tc);
	}

	PrimitiveType* Type::getFloat32(FTContext* tc)
	{
		return PrimitiveType::getFloat32(tc);
	}

	PrimitiveType* Type::getFloat64(FTContext* tc)
	{
		return PrimitiveType::getFloat64(tc);
	}

	PointerType* Type::getInt8Ptr(FTContext* tc)
	{
		return PointerType::getInt8Ptr(tc);
	}

	PointerType* Type::getInt16Ptr(FTContext* tc)
	{
		return PointerType::getInt16Ptr(tc);
	}

	PointerType* Type::getInt32Ptr(FTContext* tc)
	{
		return PointerType::getInt32Ptr(tc);
	}

	PointerType* Type::getInt64Ptr(FTContext* tc)
	{
		return PointerType::getInt64Ptr(tc);
	}

	PointerType* Type::getUint8Ptr(FTContext* tc)
	{
		return PointerType::getUint8Ptr(tc);
	}

	PointerType* Type::getUint16Ptr(FTContext* tc)
	{
		return PointerType::getUint16Ptr(tc);
	}

	PointerType* Type::getUint32Ptr(FTContext* tc)
	{
		return PointerType::getUint32Ptr(tc);
	}

	PointerType* Type::getUint64Ptr(FTContext* tc)
	{
		return PointerType::getUint64Ptr(tc);
	}

	PointerType* Type::getFloat32Ptr(FTContext* tc)
	{
		return PointerType::getFloat32Ptr(tc);
	}

	PointerType* Type::getFloat64Ptr(FTContext* tc)
	{
		return PointerType::getFloat64Ptr(tc);
	}

	CharType* Type::getCharType(FTContext* tc)
	{
		return CharType::get(tc);
	}

	StringType* Type::getStringType(FTContext* tc)
	{
		return StringType::get(tc);
	}


}


















