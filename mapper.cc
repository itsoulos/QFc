# include <stdio.h>
# include <stdlib.h>
# include <mapper.h>
# include <math.h>

Mapper::Mapper(int d)
{
	dimension = d;
	parser.resize(0);
	vars="x1";
	for(int i=1;i<dimension;i++)
	{
		vars=vars+",";
		char str[10];
		sprintf(str,"x%d",i+1);
		vars=vars+str;
	}	
}

void	Mapper::setExpr(vector<string> s)
{
	if(parser.size()!=s.size()) 
	{
        parser.resize(s.size());
		for(int i=0;i<parser.size();i++) 
		{
			parser[i] = new FunctionParser();
		}
	}
	for(int i=0;i<s.size();i++) 
	{
		int d=parser[i]->Parse(s[i],vars);
	}
}

extern int haveX;

int	Mapper::map(Matrix x,Matrix &x1)
{
    double *xx=new double[x.size()];

    for(int i=0;i<x.size();i++) xx[i]=x[i];
	int countX=0;

	for(int i=0;i<parser.size();i++) 
	{
		haveX=0;
		x1[i]=parser[i]->Eval(xx);
		countX+=haveX==0;
		//if(!haveX) {delete[] xx;return 0;}
		if(isnan(x1[i]) || isinf(x1[i])) {delete[] xx;return 0;}
		if(parser[i]->EvalError()) {delete[] xx;return 0;}
        //if(fabs(x1[i])>1e+2) {delete[] xx;return 0;}
	}
    delete[] xx;
	haveX=countX;
	return 1;
}

int Mapper::getOriginalDimension() const
{
    return dimension;
}

Mapper::~Mapper()
{
	for(int i=0;i<parser.size();i++) delete parser[i];
}
