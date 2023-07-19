#ifndef OSAMARBF_H
#define OSAMARBF_H
# include <model.h>
# include <Util.h>
# include <KmeansPP.h>
class OsamaRbf :public Model
{
private:
    int num_of_labels;
    double gamma;
    bool istrain2;
    vector< std::vector<double> > rbf_units;
    vector< std::vector<double> > layer2_weights;
    vector<Matrix> rbf_centroids;


    void buildRBFUnits();
    void calculateGamma();
    double distance(const datapoint &a ,const datapoint &b);
    double basisFunction(const datapoint &data_point, const datapoint &centroid);
public:
    OsamaRbf(Mapper *m);
    virtual	double 	train1();
    virtual double	train2();
    virtual double	output(Matrix x);
    virtual void	getDeriv(Matrix x,Matrix &g);
    virtual double	funmin(Matrix x);
    datapoint predictLabel(const datapoint &data_point, const double &data_label, double &error);
};

#endif // OSAMARBF_H
