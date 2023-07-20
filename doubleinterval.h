#ifndef DOUBLEINTERVAL_H
#define DOUBLEINTERVAL_H
# include <QString>
# include <math.h>

class DoubleInterval
{
private:
    double _left,_right;
public:
    DoubleInterval(double a=0.0,double b=0.0);
    DoubleInterval(const DoubleInterval &other);
    DoubleInterval left() const;
    DoubleInterval right() const;
    QString toString() const;
    DoubleInterval&operator=(const DoubleInterval &other);
    double leftValue() const;
    double rightValue() const;
    double width() const;
    void    divideLeft();
    void    divideRight();
    friend std::ostream& operator<<(std::ostream& out, const DoubleInterval &f1);
    friend DoubleInterval operator+(const DoubleInterval& a, const DoubleInterval& b);
    friend DoubleInterval operator-(const DoubleInterval& a, const DoubleInterval& b);
    friend DoubleInterval operator*(double value,const DoubleInterval &other);
    friend DoubleInterval operator*(const DoubleInterval &other,double value);
    ~DoubleInterval();
};

#endif // DOUBLEINTERVAL_H
