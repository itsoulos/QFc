# ifndef __NEURAL__H
# define __NEURAL__H
# include <model.h>
class Neural :public Model
{
private:
    QString localSearchMethod;
    bool weigh_decay_flag = false;
	public:
		Neural(Mapper *m);
        Matrix bestWeight;
        double  bestValue;
        double    localSearch();
        void enableWeightDecay();
        void    setLocalSearchMethod(QString s);
        QString  getLocalSearchMethod( ) const;
		double	countViolate(double limit);
		virtual double train1();
		virtual double train2();
		virtual double output(Matrix x);
		virtual void   getDeriv(Matrix x,Matrix &g);
		void	setWeights(Matrix x);
        double  getValueAtPos(int pos);
        void    gradientAtPos(int pos,Matrix &g);
		~Neural();
};

# endif
