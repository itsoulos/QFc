#include "osamarbf.h"

OsamaRbf::OsamaRbf(Mapper *m)
    :Model(m)
{

}

void OsamaRbf::buildRBFUnits()
{
    rbf_units.assign(xpoint.size(),vector<double>());
    for(unsigned int i = 0 ; i<xpoint.size() ; i++)
    {
        for(unsigned int j = 0 ; j<rbf_centroids.size() ; j++)
            rbf_units[i].push_back(basisFunction(xpoint[i], rbf_centroids[j]));
    }
}


double OsamaRbf::basisFunction(const datapoint &data_point, const datapoint &centroid)
{
    return exp(gamma * distance(data_point,centroid));
}

double OsamaRbf::distance(const datapoint &a, const datapoint &b)
{
    double dist=0;
    for(int i = 0 ; i< a.size() ; i++)
        dist += (a[i]-b[i])*(a[i]-b[i]);
    return dist;
}
void OsamaRbf::calculateGamma()
{
    double variance = -1;
    for (int i = 0; i < rbf_centroids.size() ; ++i)
        for (int j = i + 1; j < rbf_centroids.size() ; ++j)
            variance = max(variance, distance(rbf_centroids[i], rbf_centroids[j]));
    variance *= (1.0 / ((double)rbf_centroids.size()));
    gamma = (-1.0 / (2.0 * variance));
}

double 	OsamaRbf::train1()
{

    for(unsigned int i=0;i<xpoint.size();i++)
    {
        mapper->map(origx[i],xpoint[i]);
    }
    // Calculate RBF Centroids
    KmeansPP KMPP(xpoint);
    rbf_centroids.clear();

    KMPP.RunKMeansPP(num_weights, rbf_centroids);
    calculateGamma();
    // Build First (Fixed) Layer Units
    buildRBFUnits();
    num_of_labels=1;
    // Init the Second Layer with values [-1,1]
    layer2_weights.assign(num_of_labels, vector<double>(num_weights,0));
    for(auto &label: layer2_weights)
        for(auto &arc: label)
            arc =drand48();// random_real_gen(random_engine);
    double mse = 0;
        double accuracy=0;

        const int num_iterations=10;
        const double learning_rate=0.005;
        // Train the second layer weights
            mse=0.0;

            vector<datapoint> best_prediction;
            // double accuracy=0;
            for(int iter = 0 ; iter<num_iterations ; iter++)
            {
                //prediction

                for(int i = 0 ; i<xpoint.size() ; i++)
                {
                    for(int label = 0 ; label<num_of_labels ; label++)
                    {
                        double hypothesis = Utility::multiplyVectors(rbf_units[i], layer2_weights[label]);
                        double error_direction = (ypoint[i]-hypothesis);
                        vector<double> delta = Utility::multiplyVecConst(rbf_units[i], error_direction * learning_rate);
                        Utility::AddVectors(layer2_weights[label], delta);

                    }

                }


                if(true)
                {
                    // Gathering Statistics
                    mse = 0.0;
                    for(int i = 0 ; i<xpoint.size() ; i++)
                    {
                        double error_dir=0;
                        datapoint predict_data=predictLabel(xpoint[i], ypoint[i], error_dir);
                        mse += error_dir;
                    }
                    mse *= (double)(1.0/((double)xpoint.size()*(double)num_of_labels));
                }

                if(mse < 1e-9)
                    break;
            }

            return mse;
}
datapoint OsamaRbf::predictLabel(const datapoint &data_point, const double &data_label,double &error)
{
    vector<double> cur_rbf_unit;
    for(int j = 0 ; j<rbf_centroids.size() ; j++)
        cur_rbf_unit.push_back(basisFunction(data_point, rbf_centroids[j]));
    // Get the label with maximum hypothesis
    datapoint predict_data;
    for(int label = 0 ; label<num_of_labels ; label++)
    {
        double hypothesis = Utility::multiplyVectors(cur_rbf_unit, layer2_weights[label]);
        double error_direction = (data_label-hypothesis);
        error += error_direction*error_direction;
        predict_data.push_back(hypothesis);
    }
    return predict_data;
}

double	OsamaRbf::train2()
{
    return train1();
}

double	OsamaRbf::output(Matrix x)
{
    double e;
    return predictLabel(x,0.0,e)[0];
}

void	OsamaRbf::getDeriv(Matrix x,Matrix &g)
{

}

double	OsamaRbf::funmin(Matrix x)
{
    return Model::funmin(x);
}
