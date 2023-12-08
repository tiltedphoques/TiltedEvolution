#pragma once

namespace BSReflection
{

struct IObject
{
	virtual ~IObject();
	virtual void sub_1();
};

static_assert(sizeof(IObject) == 0x8);

}
