/* K-Means Clustering Algorithm */

#include "kmeans.h"
# include <vector>
using namespace std;

/* ---------------------------------------------- */
/* K-Means Clusteriing Algorithm                  */
/* Reads data from data_vectors matrix,           */
/* implements kmeans clustering and returns the   */ 
/* results to centers matrix along with the       */ 
/* variances into the variances matrix.           */ 
/* Input Parameters:                              */
/*      data_vectors : pointer to input data      */
/*                     matrix (m rows, n columns) */ 
/*      centers      : pointer to centers matrix  */
/*                     (K rows, n columns)        */
/*      variances    : pointer to the variances   */
/*                     matrix (K rows, n columns) */
/*      m            : number of data vectors     */
/*      n            : dimension of data vector   */
/*      K            : number of centers          */ 
/* ---------------------------------------------- */
# include <QfcRandom.h>

void myKmeans(double * data_vectors, double * centers, 
            double * variances, int m, int n, int K)
{
	for(int i=0;i<K;i++)
		for(int j=0;j<n;j++)
			centers[i*n+j]=0.0;

	typedef vector<int> D;
	vector<D> member;
	member.resize(K);


	double *pattern =new double[n];
	double *old=new double[n];

	double sumdiff=1e+100;
	int iters=0;
	for(int i=0;i<m;i++)
	{
        int k=randDouble()*K;
		if(k==K) k=K-1;
		member[k].push_back(i);
	}
	for(int i=0;i<K;i++)
	{
		for(int j=0;j<member[i].size();j++)
		{
			for(int k=0;k<n;k++) 
			{
				pattern[k]=data_vectors[member[i][j]*n+k];
				centers[i*n+k]+=pattern[k];
			}
		}
		for(int j=0;j<n;j++) {
			centers[i*n+j]/=member[i].size();
		}
	}		

	while(iters<20 && sumdiff>1000)
	{
		iters++;
		for(int i=0;i<K;i++) 
			member[i].resize(0);

		for(int i=0;i<m;i++)
		{
			for(int j=0;j<n;j++) pattern[j]=data_vectors[i*n+j];
			double mindist=1e+100;
			int    imin=-1;
			for(int j=0;j<K;j++)
			{
				double s=0.0;
				for(int k=0;k<n;k++) s+=pow(centers[j*n+k]-pattern[k],2.0);
				if(s<mindist || j==0) 
				{
					mindist = s;
					imin = j;
				}
			}
			member[imin].push_back(i);
		}
		/*
		for(int i=0;i<K;i++)
		{
			if(member[i].size()==0) 
			{
				for(int j=0;j<K;j++)
				{
					if(member[j].size()>=2)
					{
						member[i].push_back(member[j][member[j].size()-1]);
						member[j].resize(member[j].size()-1);
						break;
					}
				}
			}
		}
		*/
		sumdiff=0.0;
		for(int i=0;i<K;i++)
		{
			int count=0;
			for(int j=0;j<n;j++) {old[j]=centers[i*n+j];centers[i*n+j]=0.0;}
			double ilastclass=0.0;
			for(int j=0;j<member[i].size();j++)
			{
				count++;
				for(int k=0;k<n;k++) 
				{
					extern double *Output;
					if(j==0) ilastclass=Output[member[i][j]];
					else
					if(Output[member[i][j]]!=ilastclass)
					{
						member[i][j]=-1;
						count--;
						break;
					}
					pattern[k]=data_vectors[member[i][j]*n+k];
					centers[i*n+k]+=pattern[k];
				}
			}
			if(count)
			{
				for(int j=0;j<n;j++) 
				{	
					centers[i*n+j]/=count;
					sumdiff+=pow(old[j]-centers[i*n+j],2.0);
				}
			}
		}		
	}
	
	for(int i=0;i<K;i++)
	{
		for(int j=0;j<n;j++) variances[i*n+j]=0.0;
		int count=0;
		for(int j=0;j<member[i].size();j++)
		{
			if(member[i][j]<0) continue;
			count++;
			for(int k=0;k<n;k++) 
			{
				pattern[k]=data_vectors[member[i][j]*n+k];
				variances[i*n+k]+=pow(pattern[k]-centers[i*n+k],2.0);
			}
		}
		if(count)
		for(int k=0;k<n;k++) variances[i*n+k]/=count;
	}
	delete[] pattern;
	delete[] old;
}

# include <random>
using namespace std;


void Kmeans(double * data_vectors, double * centers, 
            double * variances, int m, int n, int K)
{
	int i=0;
	int j=0;
	int l=0;
	int k=0;


	double * new_centers = (double*)malloc(sizeof(double)*K*n);
	int **cluster_members=new int*[K];
	for(int i=0;i<K;i++)
		cluster_members[i]=new int[m];
	int *num_of_cluster_members=new int[K];
	double distance=0;
	double total_distance=0;
	double min_distance=0;
	int min_center=0;
	int match=0;
	int cur_match=0;
	int new_cen=0;
	int *random_centers=new int[K];
	int found=1;
	int iterations=0;
	double var_diag=0;


	// Assign a random center to each example in the training set
	for(i=0; i<K; i++)
	{
        do{
            double random =randDouble();
            random_centers[i]=(int)((m-1) * random);
		}while(random_centers[i]>m);
		
	}
	// Search the dataset and assign duplicate examples to different centers
	for(i=0; i<K; i++)
		for(j=0; j<K; j++)
		    if(i!=j)
			if(random_centers[i] == random_centers[j]) {
			    for(l=0; l<m; l++) {
				found=1;
				for(k=0; k<K; k++) {
					if(l==random_centers[k])
					    found=0;
				}
				if(found==1) {
					new_cen=l;
					break;
				}
			    }
			    random_centers[j]=new_cen;
			}
	//for(i=0; i<K; i++)  printf("Random center is: %d %d\n", i, random_centers[i]);


	// Create the initial random centers
	for(i=0; i<K; i++) {
		for(j=0; j<n; j++) {
			//if(random_centers[i]==m)
			//	printf("error \n");
		  centers[i*n + j] = data_vectors[random_centers[i]*n + j];
		  new_centers[i*n + j] = 0;
		  variances[i*n + j] = 0;
		}
		num_of_cluster_members[i]=0;
		for(j=0; j<m; j++)
			cluster_members[i][j]=0;
	}

    // Main K-Means loop starts here
    iterations=0;
    while(1) {

	/* Loop over all points in the dataset */
	for(i=0; i<m; i++) {

	   /* Estimate the closest center to point i */
	   for(j=0; j<K; j++) {
		distance=0;
		for(l=0; l<n; l++) {
			distance += pow((data_vectors[i*n + l] - centers[j*n + l]),2.0);
		}

		if(j==0) {
		   min_distance = distance;
		   min_center = j;
		   continue;
		}
		
		if(distance < min_distance) {
		   min_distance = distance;
		   min_center = j;
		}
	   }
		
	   for(l=0; l<n; l++)
		new_centers[min_center*n + l] += data_vectors[i*n + l];
	   cluster_members[min_center][num_of_cluster_members[min_center]] = i;
	   num_of_cluster_members[min_center]++;
	   

	}

	   /* Estimate the new centers */
	   for(i=0; i<K; i++) {
		for(l=0; l<n; l++) {
			//GIANNIS
			if(num_of_cluster_members[i])
		   new_centers[i*n + l] /= (double)num_of_cluster_members[i];
			//GIANNIS
		   if(finite(new_centers[i*n + l]) == 0)
			new_centers[i*n + l] = 0;
		}
	   }

	//for(i=0; i<K; i++) {
	//	printf("Cluster members [%d]:  %d\n", i, num_of_cluster_members[i]);
	//}


	/* Here we print the total distance for each pass */
	for(i=0; i<K; i++) {
	  for(j=0; j<num_of_cluster_members[i]; j++) {
	    for(l=0; l<n; l++) {
		total_distance += pow((data_vectors[cluster_members[i][j]*n + l] - centers[i*n + l]),2.0);
	    }
	  }
	}
	//printf("Total distance: %f\n", total_distance);
	total_distance=0;

	   /* Check if converges */
	   match=0;
	   for(i=0; i<K; i++) {
		cur_match=0;
		for(j=0; j<n; j++) {
		   if(new_centers[i*n + j] == centers[i*n + j])
			cur_match++;
		}
		if(cur_match == n)
			match++;
	   }

	   /* If the centers remain the same: terminate */
	   if(match == K)
		break;


	   for(i=0; i<K; i++) {
		for(j=0; j<n; j++) {
			centers[i*n + j] = new_centers[i*n + j];
			new_centers[i*n + j]=0;
		}
		num_of_cluster_members[i]=0;
	   }

       iterations++;
       if(iterations>2000) break;
    }                        /* telos tou while(1) */ 


	/* ----------------- YPOLOGISMOS VARIANCE -------------- */

	for(i=0; i<K; i++) {
		for(j=0; j<num_of_cluster_members[i]; j++) {
			for(l=0; l<n; l++) {
				variances[i*n + l] += pow((data_vectors[cluster_members[i][j]*n + l] - centers[i*n + l]),2.0);
			}
		}
	}

	for(i=0; i<K; i++) {
		for(j=0; j<n; j++) {
			//GIANNIS
			if(num_of_cluster_members[i])
			variances[i*n + j] /= (double)num_of_cluster_members[i];
			else
			{
				variances[i*n+j]=0;
			}
		}
	}



   free(new_centers);
   delete[] num_of_cluster_members;
   delete[] random_centers;
   for(int i=0;i<K;i++) delete[] cluster_members[i];
   delete[] cluster_members;
	
}



