# include <knn.h>
# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <gensolver.h>
# include <model.h>

static double getDistance(double *x1,double *x2,int d)
{
	double s=0.0;
	for(int i=0;i<d;i++) s+=pow(x1[i]-x2[i],2.0);
	return sqrt(s);
}

static double getDistance(vector<double> x1,vector<double> x2)
{
	double s=0.0;
	int d=x1.size();
	for(int i=0;i<d;i++) s+=pow(x1[i]-x2[i],2.0);
	return sqrt(s);
}

KNN::KNN(Mapper *m):Model(m)
{
    model_type = MODEL_KNN;
}

double KNN::train1()
{

	vector<double> classes;
	double v;
	for(int i=0;i<xpoint.size();i++) 
	{
		int d=mapper->map(origx[i],xpoint[i]);
		if(!d) return 1e+100;
		int found=0;
		for(int j=0;j<classes.size();j++)
		{
			if(fabs(classes[j]-ypoint[i])<1e-7)
			{
				found=1;
				break;
			}
		}
		if(found) continue;
		classes.push_back(ypoint[i]);
	}
	vector<double> distance;
	distance.resize(num_weights);
	vector<int> index;
	index.resize(num_weights);
	vector<int> winner;
	winner.resize(classes.size());

	double sum=0.0;
	for(int i=0;i<xpoint.size();i++)
	{
		for(int j=0;j<num_weights;j++)
		{
			distance[j]=1e+100;
			index[j]=-1;
		}
		for(int j=0;j<num_weights;j++)
		{
//			for(int m=0;m<num_weights * 5;m++)
			for(int k=0;k<xpoint.size();k++)
			{
				//int k=(int)(drand48() * xpoint.size());
				if(i==k) continue;
				double d=getDistance(xpoint[i],xpoint[k]);
				if(d<distance[j])
				{
					int found=0;
					for(int l=0;l<j;l++)
					{
						if(index[l]==k) {found=1;break;}
					}
					if(found) continue;
					distance[j]=d;
					index[j]=k;
				}
			}
		}
		for(int j=0;j<winner.size();j++) winner[j]=0;
		for(int j=0;j<num_weights;j++)
		{
			double d=ypoint[index[j]];
			for(int k=0;k<classes.size();k++)
			{
				if(fabs(d-classes[k])<1e-5) 
				{
					winner[k]++;
				}
			}
		}
		int maxv=0;
		int maxclass=0;
		for(int j=0;j<winner.size();j++) 
		{
			if(winner[j]>maxv)
			{
				maxv=winner[j];
				maxclass=j;
			}
		}
		sum+=fabs(classes[maxclass]-ypoint[i])>1e-5;

		//if(fabs(classes[maxclass]-ypoint[i])>1e-5) 
		//sum+=pow(classes[maxclass]-ypoint[i],2.0);
	}
	return sum*100.0/ypoint.size();
}

double KNN::train2()
{
	return train1();
}

double KNN::output(Matrix &x)
{
    int k = num_weights;
    vector<double> dist;
    vector<int> index;
    dist.resize(xpoint.size());
    index.resize(dist.size());
    for(int i=0;i<xpoint.size();i++)
    {
        double d=getDistance(x,xpoint[i]);
        index[i]=i;
    }
    for(int i=0;i<dist.size();i++)
    {
     for(int j=0;j<dist.size()-1;j++)
     {
         if(dist[j+1]<dist[j])
         {
             double d=dist[j];
             dist[j]=dist[j+1];
             dist[j+1]=d;
             int k=index[j];
             index[j]=index[j+1];
             index[j+1]=k;
         }
     }
    }
    double average = 0.0;
    for(int i=1;i<=k;i++)
    {
        average+=ypoint[index[i]];
    }
    average/=k;
    return average;
}

void   KNN::getDeriv(Matrix &xx,Matrix &g)
{
	/*	NOTHING TO DO
	 * */
}

void	KNN::sortArray(Matrix &x,vector<int> &index)
{
	for(int i=0;i<x.size();i++)
	{
		for(int j=0;j<x.size()-1;j++)
			if(x[j+1]<x[j])
			{
				double t=x[j];
				x[j]=x[j+1];
				x[j+1]=t;
				int k=index[j];
				index[j]=index[j+1];
				index[j+1]=k;
			}
	}
}


void	KNN::makeDistance(vector<Matrix> &testx,vector<Matrix> &distance)
{
	distance.resize(testx.size());
	
	for(int i=0;i<testx.size();i++)
	{
		distance[i].resize(testx.size());
		for(int j=0;j<i;j++)
		{
			distance[i][j]=distance[j][i]=(i==j)?1e+100:getDistance(testx[i],testx[j]);
		}
		distance[i][i]=1e+100;

	}
}

void	KNN::loadTest(QString filename,vector<Matrix> &testx,Matrix &testy)
{
	FILE *fp;
    fp=fopen(filename.toStdString().c_str(),"r");
	if(!fp) return ;
	int dim;
	fscanf(fp,"%d",&dim);
	if(dim<=0) 
	{
		fclose(fp);
		return ;
	}	
	int count;
	fscanf(fp,"%d",&count);
	if(count<=0)
	{
		fclose(fp);
		return ;
	}
	testx.resize(count);
	for(int i=0;i<count;i++) testx[i].resize(pattern_dimension);
	testy.resize(count);
	Matrix xx;
	xx.resize(dim);
	double sum = 0.0;
	for(int i=0;i<count;i++)
	{
		for(int j=0;j<dim;j++) fscanf(fp,"%lf",&xx[j]);
		mapper->map(xx,testx[i]);
		fscanf(fp,"%lf",&testy[i]);
	}
	fclose(fp);
}

static void findKNN(int k,Matrix &distance,vector<int> &index)
{
	index.resize(k);
	for(int i=0;i<k;i++) index[i]=-1;
	for(int i=0;i<k;i++)
	{
		double mindist=1e+100;
		int imindist=0;
		for(int j=0;j<distance.size();j++)
		{
			if(distance[j]<mindist)
			{
				int found=0;
				for(int l=0;l<i;l++) 
				{
					if(index[l]==j) {found=1;break;}
				}
				if(found) continue;
				mindist=distance[j];
				imindist=j;
			}
		}
		index[i]=imindist;
	}	
}

double	KNN::KNNtestError(vector<Matrix> &testx,Matrix &testy,vector<Matrix> &distance)
{
	int count=testx.size();
	int dim;
	vector<double> classes;
	double v;
	for(int i=0;i<xpoint.size();i++) 
	{
		int found=0;
		for(int j=0;j<classes.size();j++)
		{
			if(fabs(classes[j]-ypoint[i])<1e-7)
			{
				found=1;
				break;
			}
		}
		if(found) continue;
		classes.push_back(ypoint[i]);
	}

	vector<int> winner;
	winner.resize(classes.size());
	vector<int> index;

	double sum=0.0;
	for(int i=0;i<count;i++)
	{
		findKNN(num_weights,distance[i],index);
		for(int j=0;j<winner.size();j++) winner[j]=0;	
		for(int j=0;j<num_weights;j++)
		{
			double d=testy[index[j]];
			int pos=0;
			for(int k=0;k<classes.size();k++)
			{
				if(fabs(classes[k]-d)<1e-5) 
				{
					pos=k;
					break;
				}
			}
			winner[pos]++;
		}
		int maxw=winner[0];
		int imaxw=0;
		for(int j=0;j<winner.size();j++) if(winner[j]>maxw) {maxw=winner[j];imaxw=j;}
		if(fabs(testy[i]-classes[imaxw])>1e-5) sum=sum+1.0;
	}

    return (sum)*1.0/count;
}
KNN::~KNN()
{
}
