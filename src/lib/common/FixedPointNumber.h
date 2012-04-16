#pragma once

#include <iostream>
// fixnum
//#define fixfloat
//#define fixdouble

template <class BaseType=long, unsigned ShiftDigits=10>
class FixedPointNumber
{
protected:
	const static BaseType _factor = 1 << ShiftDigits;

public:
	FixedPointNumber()
		: _value(0)
	{
	}

	FixedPointNumber(double num)
	{
		operator=(num);
	}

	FixedPointNumber& operator=(double num)
	{
		_value = static_cast<BaseType>(num*_factor);
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

	// need to make effort to keep extra digits -- probably need to do two multiplies and combine the results
	FixedPointNumber& operator *=(const FixedPointNumber& rhs)
	{
		BaseType newval = _value * rhs.asInt();
		newval += (_value * rhs.remainder()) >> ShiftDigits;
		_value = newval;
		return *this;
	}

	// like multiply, probably need to do two division ops and combine the results
	FixedPointNumber& operator /=(const FixedPointNumber& rhs)
	{
		BaseType newval = (_value << ShiftDigits) / rhs._value;
		if (newval == 0)
		{
			BaseType temp = (rhs._value >> ShiftDigits);
			if (temp != 0)
				newval = _value / temp;
		}
		_value = newval;
		return *this;
	}

	bool operator==(const FixedPointNumber& rhs)
	{
		return _value == rhs._value;
	}

	BaseType asInt() const
	{
		return _value >> ShiftDigits;
	}

	double asDouble() const
	{
		return static_cast<double>(remainder())/_factor + asInt();
	}

	BaseType remainder() const
	{
		static BaseType keep = _factor-1;
		return _value & keep;
	}

	BaseType rawvalue() const
	{
		return _value;
	}

protected:
	BaseType _value;
};

typedef FixedPointNumber<long,      10> fixfloat;
typedef FixedPointNumber<long long, 20> fixdouble;

template <class BaseType, unsigned ShiftDigits>
std::ostream& operator<<(std::ostream& os, const FixedPointNumber<BaseType,ShiftDigits>& val)
{
	os << val.asDouble();
	return os;
}
