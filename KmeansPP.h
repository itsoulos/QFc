#pragma once

#include <vector>
#include <assert.h>
# include  <model.h>
typedef std::vector<double> datapoint;

class KmeansPP
{	
private:
    QRandomGenerator gen;

    Model *model;
public:

	//typedef std::vector<double> datapoint;

	/*	Takes the input data where each data point is in a vector<double> format */
    KmeansPP(Model *m,const std::vector<datapoint> &input_data);
	
    ~KmeansPP();

	/*	Run the K-Means++ (Plus Plus) Algorithm
		Takes the number of desired clusters K and passed vector of datapoints (vector of doubles) as input
		Returns a vector for each cluster which includes the indices of their corresponding data points	*/
	std::vector<std::vector<int> > RunKMeansPP(int K, std::vector<datapoint> &centroids);

	
private:
	
	//Store the input data for many future runs without reinitializing
	std::vector<datapoint> input_data;

	std::vector<int> nearest_cluster_idx;
	std::vector<double> nearest_cluster_dist;
	std::vector<datapoint>initial_centroids_;
	std::vector<datapoint> cur_centroids_ ;
	std::vector<datapoint> prev_centroids_;



	void init();
	void updateNearestClusters(const std::vector<datapoint> &centroids_);
	void updateCentroids(const std::vector<datapoint> &centroids_);
	int getNextInitialCentroidIndex();
	int getClosestCentroidIndex(int data_point_idx, const std::vector<datapoint> &centroids_);
	double distance(const datapoint &a , const datapoint &b);
	bool equalCentroids(const std::vector<datapoint> &a , const std::vector<datapoint> &b);
	datapoint getMeanCentroid(const std::vector<datapoint> &centroids_);

};

