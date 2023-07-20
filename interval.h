#ifndef INTERVAL_H
#define INTERVAL_H

# include <QString>
# include <math.h>
class Interval
{
private:
    int _left,_right;
public:
    Interval(int a=0,int b=0);
    Interval(const Interval &other);
    Interval left() const;
    Interval right() const;
    QString toString() const;
    Interval&operator=(const Interval &other);
    int leftValue() const;
    int rightValue() const;
    int width() const;
    void    divideLeft();
    void    divideRight();
    friend std::ostream& operator<<(std::ostream& out, const Interval &f1);
    friend Interval operator+(const Interval& a, const Interval& b);
    friend Interval operator-(const Interval& a, const Interval& b);
    friend Interval operator*(int value,const Interval &other);
    friend Interval operator*(const Interval &other,int value);
    ~Interval();
};

#endif // INTERVAL_H
