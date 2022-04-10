# ifndef __POPULATION__H
# include <program.h>
# include <QString>

/* The Population class holds the current population. */
/* The mutation, selection and crossover operators are defined here */
class Population
{
	private:
		int	**children;
		int	**trialx;
		int	**genome;
		int	*valid;
		double *fitness_array;
		double	mutation_rate,selection_rate;
		int	genome_count;
		int	genome_size;
		int	generation;
		Program	*program;

		void	crossover();
		void	mutate();
		void	calcFitnessArray();
		void	replaceWorst();
		int	elitism;
		void	localSearch(int gpos);
        int     localSearchGenerations;
        double  localSearchRate;
        QString localSearchMethod;
	public:
		Population(int gcount,int gsize,Program *p);
		double 	fitness(vector<int> &g);
		void	setElitism(int s);
        void	select();

		int	getGeneration() const;
		int	getCount() const;
		int	getSize() const;
		void	nextGeneration();
		void	setMutationRate(double r);
		void	setSelectionRate(double r);
		double	getSelectionRate() const;
		double	getMutationRate() const;
		double	getBestFitness() const;
		double	evaluateBestFitness();
		vector<int> getBestGenome() const;
		void	reset();
        void    setLocalSearchRate(double r);
        double  getLocalSearchRate() const;
        void    setLocalSearchGenerations(int g);
        int     getLocalSearchGenerations() const;
        void    setLocalSearchMethod(QString s);
        QString  getLocalSearchMethod() const;
        void	setBest(vector<int> &g,double f);
            void	getGenome(int pos,vector<int> &genome);
            void	setGenome(int pos,vector<int> &genome,double f);
            void	setGenome(int pos,vector<int> &genome,double f,int k);
		~Population();
		
};
# define __POPULATION__H
# endif
