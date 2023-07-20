#include "interval.h"
# include <QString>
Interval::Interval(int a,int b)
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

Interval::Interval(const Interval &other)
{
    _left=other.leftValue();
    _right=other.rightValue();
}

Interval operator*(int value,const Interval &other)
{
    return Interval(value * other.leftValue(),value * other.rightValue());
}

Interval operator*(const Interval &other,int value)
{
    return Interval(value * other.leftValue(),value * other.rightValue());
}

Interval operator+(const Interval& a, const Interval& b)
{
    return Interval(a.leftValue()+b.leftValue(),a.rightValue()+b.rightValue());
}

Interval operator-(const Interval& a, const Interval& b)
{
    return Interval(a.leftValue()-b.rightValue(),a.rightValue()-b.leftValue());
}
Interval Interval::left() const
{
    int mid=(_right-_left)/2;
    return Interval(_left,_left+mid);
}

Interval Interval::right() const
{
    int mid=(_right-_left)/2;
    return Interval(_left+mid,_right);
}

QString Interval::toString() const
{
    return "["+QString::number(_left)+","+QString::number(_right)+"]";
}

int  Interval::width() const
{
    return _right-_left;
}

std::ostream& operator<<(std::ostream& out, const Interval &f1)
{
    out << f1.toString().toStdString();
    return out;
}

int Interval::leftValue() const
{
    return _left;
}

int Interval::rightValue() const
{
    return _right;
}

Interval& Interval::operator=(const Interval &other)
{
    _left=other.leftValue();
    _right=other.rightValue();
    return *this;
}


void   Interval::divideLeft()
{
    _left/=2;
}

void   Interval::divideRight()
{
    _right/=2;
    if(_right<_left)
    {
        int t=_left;
        _left=_right;
        _right=t;
    }
}

Interval::~Interval()
{

}
