# include <model.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <math.h>

static int isone(double x)
{
	return fabs(x-1.0)<1e-5;
}

static int iszero(double x)
{
	return fabs(x)<1e-5;
}

double scale_factor = 1.0;

Model::Model(Mapper *m)
{
	num_weights = 1;
	pattern_dimension = 0;
	mapper = m;
	isvalidation=0;
    seed_for_random = 1;
}

Matrix	Model::getWeights()
{
	return weight;
}


int	Model::getOriginalDimension() const
{
	return original_dimension;
}


int	Model::getNumPatterns() const
{
	return ypoint.size();
}

void	Model::randomizeWeights()
{
	weight.resize((pattern_dimension+2)*num_weights);
	setDimension(weight.size());
    for(int i=0;i<weight.size();i++) weight[i]=0.1*(2.0*randomDouble()-1.0);
}

void	Model::setPatternDimension(int d)
{
	if(pattern_dimension!=d)
	{
		pattern_dimension = d;
		for(int i=0;i<xpoint.size();i++) 
			xpoint[i].resize(pattern_dimension);
	}

}

void 	Model::readPatterns(QString filename)
{
	FILE *fp;
    fp = fopen(filename.toStdString().c_str(),"r");
    if(!fp)
    {
        printf("Can not open %s \n",filename.toStdString().c_str());
        return;
    }
	int count,d;
	fscanf(fp,"%d",&d);
	if(d<=0) 
	{
		d=0;
        printf("Negative or zero dimension \n");

		fclose(fp);
		return;
	}
	original_dimension =d ;
	fscanf(fp,"%d",&count);
	if(count<=0)
    { printf("Negative or zero count \n");
		d=0;
		fclose(fp);
		return;
	}
	origx.resize(count);
	xpoint.resize(count);
	origy.resize(count);
	ypoint.resize(count);

	int count1=0,count2=0,two_classes_flag=1;
	for(int i=0;i<count;i++)
	{
		origx[i].resize(d);
		for(int j=0;j<d;j++)
		{
			fscanf(fp,"%lf",&origx[i][j]);

		}
		fscanf(fp,"%lf",&ypoint[i]);
		if(iszero(ypoint[i])) count1++;
		else 
		if(isone(ypoint[i])) count2++;
		else	two_classes_flag=0;
		origy[i]=ypoint[i];
	}
	fclose(fp);

    xpoint.resize(origx.size());
    for(int i=0;i<xpoint.size();i++) xpoint[i].resize(pattern_dimension);

	
}

int     Model::getModelType() const
{
    return model_type;
}

static double sig(double x) {return 1.0/(1.0+exp(-x));}
void	Model::transform(Matrix x,Matrix &x1)
{
	for(int  i=0;i<x.size();i++) 
	//x1[i]=sig(x[i]);
		//x1[i]=(x[i]-xmean[i])/(xstd[i]);
	x1[i]=x[i];
	//x1[i]=(x[i]-xmin[i])/(xmax[i]-xmin[i]);
}

void	Model::setNumOfWeights(int w)
{
	num_weights = w;
}

Matrix	Model::getXpoint(int pos)
{
	return xpoint[pos];
}

double  Model::getYPoint(int pos)
{
	return ypoint[pos];
}

double  Model::getModelAtPoint(int pos)
{
	double v = output(xpoint[pos]);
	return v;
}

int	Model::getPatternDimension() const
{
	return pattern_dimension;
}

double	Model::valError()
{
	double s=0.0;
	for(int i=4*xpoint.size()/5;i<xpoint.size();i++)
	{
		double v = output(xpoint[i]);
		s+=(v-ypoint[i])*(v-ypoint[i]);
	}
	return s;
}

double	Model::funmin(Matrix &x)
{
	if(weight.size()!=x.size()) weight.resize(x.size());
	for(int i=0;i<x.size();i++) weight[i] = x[i];
	double s=0.0;
	int end=xpoint.size();
	if(isvalidation) end=4*xpoint.size()/5;
	int correct1=0;
	int correct2=0;
	int count1=0,count2=0;
	for(int i=0;i<end;i++)
	{
		double v = output(xpoint[i]);
		double e=v-ypoint[i];
		e=e*e;
#ifdef SCALEFACTOR
		if(!isone(scale_factor))
		{
			if(scale_factor>1) 
			{
				if(isone(ypoint[i])) e=e*scale_factor;
			}
			else
			{
				if(iszero(ypoint[i])) e=e*1.0/scale_factor;
			}
		}
#endif
		if(isnan(v) || isinf(v)) return 1e+100;
		s+=e;
		if(isnan(s) || isinf(s)) return 1e+100;
	}
	return s;
}

void  Model::granal(Matrix &x,Matrix &g)
{
	if(weight.size()!=x.size())
	weight.resize(x.size());
	for(int i=0;i<x.size();i++) 
	{
		weight[i] = x[i];
		g[i]=0.0;
	}
	double s=0.0;
	Matrix gtemp;
	gtemp.resize(g.size());
	int end=xpoint.size();
	if(isvalidation) end=4*xpoint.size()/5;
	for(int i=0;i<end;i++)
	{
		double	e=output(xpoint[i])-ypoint[i];
#ifdef SCALEFACTOR
		if(!isone(scale_factor))
		{
			if(scale_factor>1) 
			{
				if(isone(ypoint[i])) e=e*scale_factor;
			}
			else
			{
				if(iszero(ypoint[i])) e=e*1.0/scale_factor;
			}
		}
#endif
		getDeriv(xpoint[i],gtemp);
		for(int j=0;j<g.size();j++)
		{
			g[j]+=2.0*e*gtemp[j];
		}
	}
}

/*	Grafei to apotelemsa tou mapping 
 *	sto arxeio train. Diabazei apo to arxeio itest
 *	ta test patterns kai ta grafei sto arxeio otest.
 *	O skopos einai na xrisimopoiithoyn ta patterns 
 *	gia epexergasia apo kapoio montelo poy den 
 *	kalyptetai sto paketo.
 * */
void	Model::print(char *train,char *itest, char *otest)
{
	FILE *fp=fopen(train,"w");
	if(!fp) return;
	fprintf(fp,"%d\n%d\n",pattern_dimension,origx.size());
	for(int i=0;i<origx.size();i++)
	{
		mapper->map(origx[i],xpoint[i]);
		for(int j=0;j<pattern_dimension;j++)
			fprintf(fp,"%lf ",xpoint[i][j]);
		fprintf(fp,"%lf\n",ypoint[i]);
	}
	fclose(fp);
	FILE *fin=fopen(itest,"r");
	if(!fin) return;
	FILE *fout=fopen(otest,"w");
	int d,count;
	fscanf(fin,"%d",&d);
	fscanf(fin,"%d",&count);
	printf("read %d %d \n",d,count);
	Matrix testx;
	testx.resize(d);
	double testy;
	Matrix xx;
	xx.resize(pattern_dimension);
	fprintf(fout,"%d\n%d\n",pattern_dimension,count);
	for(int i=0;i<count;i++)
	{
		for(int j=0;j<d;j++)
			fscanf(fin,"%lf",&testx[j]);
		fscanf(fin,"%lf",&testy);
		mapper->map(testx,xx);
		for(int j=0;j<pattern_dimension;j++)
			fprintf(fout,"%lf ",xx[j]);
		fprintf(fout,"%lf\n",testy);
	}
	fclose(fin);
	fclose(fout);
}

double	Model::testError(QString filename)
{
	double testy;
	Matrix testx;
	int count;
	int dim;
	FILE *fp;
    fp=fopen(filename.toStdString().c_str(),"r");
	if(!fp) return -1.0;
	fscanf(fp,"%d",&dim);
	if(dim<=0) 
	{
		fclose(fp);
		return -1.0;
	}	
	fscanf(fp,"%d",&count);
	if(count<=0)
	{
		fclose(fp);
		return -1.0;
	}
	testx.resize(pattern_dimension);
	Matrix xx;
	xx.resize(dim);
	double sum = 0.0;
	Matrix xx2;
	xx2.resize(dim);
	for(int i=0;i<count;i++)
	{
		for(int j=0;j<dim;j++) fscanf(fp,"%lf",&xx[j]);
		transform(xx,xx2);
		mapper->map(xx2,testx);
		fscanf(fp,"%lf",&testy);
		double d=output(testx);
		sum+=pow(d-testy,2.0);
	}
	fclose(fp);
	return (sum);
}

double	Model::classTestError(QString filename,double &precision,double &recall)
{
	vector<double> classes;
	Matrix testx;
	double testy;
	int count;
	int dim;
	FILE *Fp;
	int   tp=0,fp=0,tn=0,fn=0;
	precision = 0.0;
	recall    = 0.0;
    Fp=fopen(filename.toStdString().c_str(),"r");
	if(!Fp) return -1.0;
	fscanf(Fp,"%d",&dim);
	if(dim<=0) 
	{
		fclose(Fp);
		return -1.0;
	}	
	fscanf(Fp,"%d",&count);
	if(count<=0)
	{
		fclose(Fp);
		return -1.0;
	}

	double x1,y1;
	for(int i=0;i<count;i++)
	{
		for(int j=0;j<dim;j++)
		{
			fscanf(Fp,"%lf",&x1);
		}
		fscanf(Fp,"%lf",&y1);
		int found=-1;
		for(int j=0;j<classes.size();j++)
		{
			if(fabs(classes[j]-y1)<1e-8)
			{
				found = j;
				break;
			}
		}
		if(found==-1)
		{
			int s=classes.size();
			classes.resize(s+1);
			classes[s]=y1;
		}
	}
	fclose(Fp);
	
    Fp=fopen(filename.toStdString().c_str(),"r");
	fscanf(Fp,"%d",&dim);
	fscanf(Fp,"%d",&count);
	testx.resize(pattern_dimension);
	Matrix xx;
	xx.resize(dim);
	double sum = 0.0;
	Matrix xx2;
	xx2.resize(dim);
	int count1=0,count2=0,est1=0,est2=0;
	vector<double> T,O;
	T.resize(count);
	O.resize(count);
	for(int i=0;i<count;i++)
	{
		for(int j=0;j<dim;j++) 
		{
			fscanf(Fp,"%lf",&xx[j]);
			/**/
			/**/
		}
		mapper->map(xx,testx);
		fscanf(Fp,"%lf",&testy);
		double c=output(testx);
		
		int found =-1;
        int yfound = -1;
		double dmin=1e+10;
        double ydmin = 1e+100;
		for(int j=0;j<classes.size();j++)
        {
			if(fabs(classes[j]-c)<dmin)
			{
				found=j;
				dmin=fabs(classes[j]-c);
			}
            if(fabs(classes[j]-testy)<ydmin)
            {
                yfound=j;
                ydmin=fabs(classes[j]-testy);
            }
        }
        /** ayta prepei na ginoun indexes **/


        T[i]=found;
        O[i]=yfound ;
		if(classes.size()==2)
		{
			if(isone(classes[found]) && isone(testy)) tp++;
			else
			if(isone(classes[found]) && iszero(testy)) fp++;
			else
			if(iszero(classes[found]) && iszero(testy)) tn++;
			else
			if(iszero(classes[found]) && isone(testy)) fn++;
			else ;
	//			printf("unspecified %lf %lf \n",classes[found],testy);
		}
		double myclass=classes[found];
		if(fabs(testy)<1e-5) count1++; else count2++;
		if(fabs(testy-classes[found])<1e-5)
		{
			if(fabs(classes[found])<1e-5) est1++; else est2++;
		}
		sum+=(fabs(testy-myclass)>1e-5);
	}
	fclose(Fp);
//	printf("CLASS1 = %2.lf%% CLASS2=%.2lf%%\n",est1*100.0/count1,est2*100.0/count2);
	recall=tp*1.0/(tp*1.0+fn*1.0);
	precision=tp*1.0/(tp*1.0+fp*1.0);
	vector<double> aprecision;
	vector<double> arecall;
	aprecision.resize(classes.size());
	arecall.resize(classes.size());
	printConfusionMatrix(classes.size(),
				T,O,aprecision,arecall);
	precision = 0.0;
	recall  = 0.0;
	for(int i=0;i<aprecision.size();i++)
	{
		if(isnan(aprecision[i]) || isinf(aprecision[i])) aprecision[i]=arecall[i];
		if(isnan(arecall[i]) || isinf(arecall[i])) arecall[i]=aprecision[i];
		precision+=aprecision[i];
		recall+=arecall[i];
	}
	precision/=classes.size();
	recall/=classes.size();
	printf("PRECISION  %20.10lf RECALL: %20.10lf\n",precision,recall);
	return (sum)/count;
}

void	Model::enableValidation()
{
	isvalidation=1;
}


int Model::nearestClassIndex(vector<double> &dclass,double y)
{
    int ifound=-1;
    double dmin=1e+100;
    for(unsigned int i=0;i<dclass.size();i++)
    {
        if(fabs(dclass[i]-y)<dmin)
        {
            dmin=fabs(dclass[i]-y);
            ifound=i;
        }
    }
    return ifound;
}

void	Model::printConfusionMatrix(int nclass,
				vector<double> &T,vector<double> &O,
                             vector<double> &precision,
                             vector<double> &recall)
{
    int i,j;

    int N=T.size();
    precision.resize(nclass);
    recall.resize(nclass);
    int **CM;
    printf("** CONFUSION MATRIX ** Number of classes: %d\n",nclass);
    CM=new int*[nclass];
    for(i=0;i<nclass;i++) CM[i]=new int[nclass];
    for(i=0;i<nclass;i++)
        for(j=0;j<nclass;j++) CM[i][j] = 0;

    for(i=0;i<N;i++) CM[(int)T[i]][(int)O[i]]++;

    for(i=0;i<nclass;i++)
    {
        bool fail_sum = false;
        double sum = 0.0;
        for(j=0;j<nclass;j++)
            sum+=CM[j][i];

        if(fabs(sum)<1e-5)
        {
            fail_sum  = true;
            sum = 1.0;
        }
        if(fail_sum) precision[i]=1.0;
        else
        precision[i]=CM[i][i]/sum;
        sum = 0.0;
        fail_sum = false;
        for(j=0;j<nclass;j++)
            sum+=CM[i][j];
        if(fabs(sum)<1e-5)
        {
            fail_sum  = true;
            sum = 1.0;
        }
        if(fail_sum) recall[i]=1.0;
        else
        recall[i]=CM[i][i]/sum;
    }
    for(i=0;i<nclass;i++)
    {
        for(j=0;j<nclass;j++)
        {
            printf("%4d ",CM[i][j]);
        }
        printf("\n");
        delete[] CM[i];
    }
    delete[] CM;
}
void    Model::dumpFile(QString input,QString output)
{
    FILE *Fp=fopen(input.toStdString().c_str(),"r");
    FILE *fout=fopen(output.toStdString().c_str(),"w");
    if(!fout)
    {
        fclose(Fp);
        return;
    }
    int dim,count;
    fscanf(Fp,"%d",&dim);
    fscanf(Fp,"%d",&count);
    vector<double> testx;
    double testy;
    testx.resize(pattern_dimension);
    vector<double> xx;
    xx.resize(dim);
    fprintf(fout,"%d\n%d\n",pattern_dimension,count);
    for(int i=0;i<count;i++)
    {
        for(int j=0;j<dim;j++)
        {
            fscanf(Fp,"%lf",&xx[j]);

            /**/
            /**/
        }
        mapper->map(xx,testx);
        for(int j=0;j<pattern_dimension;j++)
            fprintf(fout,"%lf ",testx[j]);
        fscanf(Fp,"%lf",&testy);
        fprintf(fout,"%lf\n",testy);
    }
    fclose(Fp);
    fclose(fout);
}

void    Model::randomSeed(int s)
{
    seed_for_random = s;
    random.seed(s);
}

int     Model::getSeed() const
{
    return seed_for_random;
}

double  Model::randomDouble()
{
    return random.generateDouble();
}

int     Model::randomInt(int low,int upper)
{
    return random.bounded(low,upper);
}

Model::~Model()
{
}
