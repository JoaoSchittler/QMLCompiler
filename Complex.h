#ifndef COMPLEXQUBIT_H
#define COMPLEXQUBIT_H
#include <math.h>
class Complex
{
	public:	double r, i;
	Complex(double x, double y): r(x), i(y) {}
	Complex(const Complex& c):r(c.r),i(c.i) {}
	Complex(): r(0), i(0) {}
	
	Complex operator +(const Complex& c)
    {
        return Complex(r+c.r,i+c.i);
    }
    Complex operator -(const Complex& c)
    {
        return Complex(r-c.r,i-c.i);
    }
    Complex operator *(const Complex& c)
    {
        return Complex(r*c.r - i*c.i,r*c.i + i*c.r); // (a+bi)(c+di)=(ac-bd)+(ad+bc)i
    }
    Complex operator /(const Complex& c)
    {
    	double div = c.r*c.r + c.i*c.i;
    	return Complex((r*c.r + i*c.i)/div,(i*c.r-r*c.i)/div);// (a+bi)/(c+di) = (ac+bd)/(c*c + d*d) + i*(bc-ad)/(c*c + d*d)
	}
	bool operator == (const Complex& c)
	{
		if(r == c.r && i == c.i) return true;
		return false;
	}
    double Magnitude()
    {
    	return sqrt(r*r + i*i);
	}
};
class QBit
{
	public : Complex left, right;
	QBit():left(0,0), right(0,0) {	}
	QBit(Complex a,Complex b) : left(a), right(b) {	}
	void Normalize()
	{
		double leftM = left.Magnitude();
		double rightM = right.Magnitude();
		double div = sqrt(leftM*leftM + rightM*rightM);
		
		left.r /= div; left.i /= div; 
		right.r /= div; right.i /= div;
		
		//printf("Normalized Qbit to (%f %f) (%f %f)\n",left.r,left.i,right.r,right.i);
	}
        
};
#endif
