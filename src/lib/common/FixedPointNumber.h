#pragma once

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

	FixedPointNumber(BaseType num)
	{
		_value = num << ShiftDigits;
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

	FixedPointNumber& operator *=(const FixedPointNumber& rhs)
	{
		_value *= rhs._value;
		return *this;
	}

	FixedPointNumber& operator /=(const FixedPointNumber& rhs)
	{
		_value /= rhs._value;
		return *this;
	}

	bool operator==(const FixedPointNumber& rhs)
	{
		return _value == rhs._value;
	}

	BaseType whole() const
	{
		return _value >> ShiftDigits;
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
