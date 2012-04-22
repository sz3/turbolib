#pragma once

#include <iostream>
// fixnum
//#define fixfloat
//#define fixdouble

template <class Primitive, unsigned ShiftBits, bool FastMath> class FixedPointMath;

template <class Primitive=long, unsigned ShiftBits=10, bool FastMath=false>
class FixedPointNumber
{
protected:
	static_assert(ShiftBits < sizeof(Primitive)*8, "ShiftBits is too big for specified Primitive type");
	const static Primitive _factor = 1 << ShiftBits;

public:
	FixedPointNumber()
		: _value(0)
	{
	}
	virtual ~FixedPointNumber() {}

	FixedPointNumber(Primitive num)
	{
		_value = num << ShiftBits;
	}

	FixedPointNumber& fromFloat(double num)
	{
		_value = static_cast<Primitive>(num*_factor);
		return *this;
	}

	FixedPointNumber& operator +=(const FixedPointNumber& rhs)
	{
		_value += rhs._value;
		return *this;
	}

	FixedPointNumber& operator -=(const FixedPointNumber& rhs)
	{
		_value -= rhs._value;
		return *this;
	}

	FixedPointNumber& operator *=(const FixedPointNumber& rhs)
	{
		_value = FixedPointMath<Primitive,ShiftBits,FastMath>::multiply(*this, rhs);
		return *this;
	}

	FixedPointNumber& operator /=(const FixedPointNumber& rhs)
	{
		_value = FixedPointMath<Primitive,ShiftBits,FastMath>::divide(*this, rhs);
		return *this;
	}

	bool operator==(const FixedPointNumber& rhs)
	{
		return _value == rhs._value;
	}

	double asDouble() const
	{
		return static_cast<double>(remainder())/_factor + asInt();
	}

	Primitive asInt() const
	{
		return _value >> ShiftBits;
	}

	Primitive remainder() const
	{
		static Primitive keep = _factor-1;
		return _value & keep;
	}

	Primitive rawvalue() const
	{
		return _value;
	}

protected:
	Primitive _value;

	friend class FixedPointMath<Primitive,ShiftBits,FastMath>;
};


// Slow math, as it were.
template <class Primitive, unsigned ShiftBits, bool FastMath>
class FixedPointMath
{
public:
	// need to make effort to keep extra digits -- probably need to do two multiplies and combine the results
	static Primitive multiply(const FixedPointNumber<Primitive,ShiftBits,FastMath>& me, const FixedPointNumber<Primitive,ShiftBits,FastMath>& rhs)
	{
		return (me._value * rhs.asInt()) + ((me._value * rhs.remainder()) >> ShiftBits);
	}

	// like multiply, probably need to do two division ops and combine the results
	static Primitive divide(const FixedPointNumber<Primitive,ShiftBits,FastMath>& me, const FixedPointNumber<Primitive,ShiftBits,FastMath>& rhs)
	{
		Primitive newval = (me._value << ShiftBits) / rhs._value;
		if (newval == 0)
		{
			Primitive temp = (rhs._value >> ShiftBits);
			if (temp != 0)
				newval = me._value / temp;
		}
		return newval;
	}
};

// Fast math
template <class Primitive, unsigned ShiftBits>
class FixedPointMath<Primitive, ShiftBits, true>
{
public:
	static Primitive multiply(const FixedPointNumber<Primitive,ShiftBits,true>& me, const FixedPointNumber<Primitive,ShiftBits,true>& rhs)
	{
		return (me._value * rhs._value) >> ShiftBits;
	}

	static Primitive divide(const FixedPointNumber<Primitive,ShiftBits,true>& me, const FixedPointNumber<Primitive,ShiftBits,true>& rhs)
	{
		return (me._value << ShiftBits) / rhs._value;
	}
};

typedef FixedPointNumber<long,      10> fixfloat;
typedef FixedPointNumber<long long, 20> fixdouble;
typedef FixedPointNumber<long,      10, true> fastfloat;
typedef FixedPointNumber<long long, 20, true> fastdouble;

template <class Primitive, unsigned ShiftBits>
std::ostream& operator<<(std::ostream& os, const FixedPointNumber<Primitive,ShiftBits>& val)
{
	os << val.asDouble();
	return os;
}
