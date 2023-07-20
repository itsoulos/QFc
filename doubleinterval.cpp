#include "doubleinterval.h"
DoubleInterval::DoubleInterval(double a,double b)
{
    if(b<a)
    {
        double t=b;
        b=a;
        a=t;
    }
    _left=a;
    _right=b;
}

DoubleInterval::DoubleInterval(const DoubleInterval &other)
{
    _left=other.leftValue();
    _right=other.rightValue();
}

DoubleInterval operator*(double value,const DoubleInterval &other)
{
    return DoubleInterval(value * other.leftValue(),value * other.rightValue());
}

DoubleInterval operator*(const DoubleInterval &other,double value)
{
    return DoubleInterval(value * other.leftValue(),value * other.rightValue());
}

DoubleInterval operator+(const DoubleInterval& a, const DoubleInterval& b)
{
    return DoubleInterval(a.leftValue()+b.leftValue(),a.rightValue()+b.rightValue());
}

DoubleInterval operator-(const DoubleInterval& a, const DoubleInterval& b)
{
    return DoubleInterval(a.leftValue()-b.rightValue(),a.rightValue()-b.leftValue());
}
DoubleInterval DoubleInterval::left() const
{
    double mid=(_right-_left)/2.0;
    return DoubleInterval(_left,_left+mid);
}

DoubleInterval DoubleInterval::right() const
{
    double mid=(_right-_left)/2.0;
    return DoubleInterval(_left+mid,_right);
}

QString DoubleInterval::toString() const
{
    return "["+QString::number(_left)+","+QString::number(_right)+"]";
}

double  DoubleInterval::width() const
{
    return _right-_left;
}

std::ostream& operator<<(std::ostream& out, const DoubleInterval &f1)
{
    out << f1.toString().toStdString();
    return out;
}

double DoubleInterval::leftValue() const
{
    return _left;
}

double DoubleInterval::rightValue() const
{
    return _right;
}

DoubleInterval& DoubleInterval::operator=(const DoubleInterval &other)
{
    _left=other.leftValue();
    _right=other.rightValue();
    return *this;
}


void   DoubleInterval::divideLeft()
{
    _left/=2.0;
}

void   DoubleInterval::divideRight()
{
    _right/=2.0;
    if(_right<_left)
    {
        double t=_left;
        _left=_right;
        _right=t;
    }
}

DoubleInterval::~DoubleInterval()
{

}
