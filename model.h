# ifndef __MODEL__H
# define __MODEL__H

# include <problem.h>
# include <mapper.h>
# include <QRandomGenerator>
# include <QString>
# define MODEL_NEURAL		1
# define MODEL_RBF		2
# define MODEL_KNN		3


class Model :public Problem
{
	protected:
		int    isvalidation;
		Matrix 	weight;
		int	num_weights;
		int	pattern_dimension;
		int	original_dimension;
		vector<Matrix> 	origx;
		Matrix	  	origy;
		vector<Matrix> 	xpoint;
		Matrix		ypoint;
        int model_type;
        QRandomGenerator random;
        int seed_for_random;
	public:

		Mapper	*mapper;
		Model(Mapper *m);
		void	setPatternDimension(int d);
		void	setNumOfWeights(int w);
        void 	readPatterns(QString filename);
		void	replacePattern(int pos,Matrix x,double y);
		int	getPatternDimension() const;
		int	getOriginalDimension() const;
		int	getNumOfWeights() const;
		int	getNumPatterns() const;
        int getModelType() const;
		Matrix	getWeights();
		Matrix	getXpoint(int pos);
		double  getYPoint(int pos);
		double  getModelAtPoint(int pos);
		/*	BASIKH SHMEIOSI
		 *	train1: Kaleitai gia tin ekpaideysi toy genetikou.
		 *	train2: Kaleitai otan teleiosei i parapano ekpaideysi.
		 * */
		virtual	double 	train1()=0;
		virtual double	train2()=0;
		virtual double	output(Matrix x)=0;
		virtual void	getDeriv(Matrix x,Matrix &g)=0;
		
		virtual double	funmin(Matrix x);
		virtual void    granal(Matrix x,Matrix &g);
		void	transform(Matrix x,Matrix &xx);
		double  valError();
		void	enableValidation();
        double	testError(QString filename);
        double	classTestError(QString filename,double &precision,double &recall);
		void	print(char *train,char *itest,char *otest);
		void	randomizeWeights();
        void    dumpFile(QString input,QString output);
		int nearestClassIndex(vector<double> &dclass,double y);
		void	printConfusionMatrix(
				int nclass,
				vector<double> &T,vector<double> &O,
                             vector<double> &precision,
                             vector<double> &recall);
        void    randomSeed(int s);
        double  randomDouble();
        int     randomInt(int low,int upper);
        int     getSeed() const;
		~Model();
};

# endif
