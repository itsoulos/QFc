# ifndef __NNPROGRAM__H
# include <program.h>
# include <cprogram.h>
# include <model.h>
# include <neural.h>
# include <rbf_model.h>
# include <knn.h>
# include <mapper.h>
# include <vector>
# include <QString>
using namespace std;



class NNprogram	:public Program
{
	private:
		vector<string> pstring;
		vector<int>    pgenome;
		int	model_type;
		int	pattern_dimension;
		Cprogram *program;
		Model	 *model;
		Mapper	 *mapper;
        bool    setModelOutside;
	public:
        NNprogram(int type,int pdimension,QString filename);
        NNprogram(Mapper *mp,Model *m,int pdimension,QString filename);
		string	printF(vector<int> &genome);
		virtual double 	fitness(vector<int> &genome);
		Model	*getModel();
		Mapper	*getMapper();
		~NNprogram();
};
# define __NNPROGRAM__H
# endif
