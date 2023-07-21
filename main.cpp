#include <QCoreApplication>
# include <getoptions.h>
# include <population.h>
# include <nnprogram.h>
# include <osamarbf.h>
# include <psoge.h>
# include <nnc.h>
# include <QDebug>
# include <QfcRandom.h>
# include <integergenetic.h>
#include <ostream>

# define GEMETHOD_GENETIC "genetic"
# define GEMETHOD_PSO     "pso"
# define GEMETHOD_IGENETIC  "igenetic"

QString ge_method = GEMETHOD_GENETIC;
vector<int> genome;


namespace Color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}

void printOption(QString fullName,QString shortName,
                 QString description,QString value)
{
    QString s=QString("%1 %2 %3 %4").arg(fullName,-27).
            arg(description,-35).arg("Default value:",-15).arg(value,-10);
    qDebug().noquote()<<s;
}

void printOption(QString fullName,QString shortName,
                 QString description,int value)
{
    QString s=QString("%1 %2 %3 %4").arg(fullName,-27).
            arg(description,-35).arg("Default value:",-15).arg(value,-10);
    qDebug().noquote()<<s;
}

void printOption(QString fullName,QString shortName,
                 QString description,double value)
{
    QString s=QString("%1 %2 %3 %4").arg(fullName,-27).
           arg(description,-35).arg("Default value:",-15).arg(value,-10);
    qDebug().noquote()<<s;
}

void printParams()
{
    qDebug().noquote()<<" PARAMETER LIST"<<"\n"<<"===============";
    printOption("--trainFile","-f","Define the training file to be used",trainFile);
    printOption("--testFile","-t","Define the test file to be used",testFile);
    printOption("--features","-d","Define the number of constructed features",features);
    printOption("--randomSeed","-r","Random seed",randomSeed);
    printOption("--featureCreateModel","-k",
                "The model used for feature construction (rbf,neural,knn,copy,osamaRbf)",
                featureCreatemodel);
    printOption("--featureEvaluateModel","-e",
                "The model used to evaluate the constructed features (rbf,neural,knn,osamaRbf,nnc)",
                featureEvaluateModel);
    printOption("--testIters","-i","The number of iterations for testing",testIters);
    printOption("--threads","-t","The number of threads",threads);
    printOption("--neural_weights","-w","Hidden nodes in neural network",
                neural_weights);
    printOption("--neural_trainingMethod","-m","Training method for neural network. Available options: bfgs, lbfgs, genetic",
                neural_trainingMethod);
    printOption("--knn_weights","-w","Number of weights in KNN model",
                knn_weights);
    printOption("--rbf_weights","-w","Number of weights in RBF model",
                rbf_weights);
    printOption("--ge_chromosomes","-c","Number of chromosomes for Grammatical Evolution",
                ge_chromosomes);
    printOption("--ge_maxGenerations","-g","Maximum generations for Grammatical Evolution",
                ge_maxGenerations);
    printOption("--ge_selectionRate","-s","Selection rate for Grammatical Evolution",
                ge_selectionRate);
    printOption("--ge_mutationRate","-m","Mutation rate for Grammatical Evolution",
                ge_mutationRate);
    printOption("--ge_length","-l","Length for the Grammatical Evolution",ge_length);
    printOption("--ge_localSearchGenerations","-g",
                "The amount of generations of every local search rate",
                ge_localSearchGenerations);


    printOption("--ge_localSearchRate","-l",
                "The rate for local search in Grammatical Evolution",
                ge_localSearchRate);

    printOption("--ge_localSearchMethod","-m",
                "The local search used in Grammatical Evolution(none,random,crossover,bfgs)",
                ge_localSearchMethod);

    printOption("--ge_method","-g",
                "The used method in Grammatical Evolution",
                ge_method);

    printOption("--genetic_chromosomes","-c","Chromosomes for the genetic algorithm",
                genetic_chromosomes);
    printOption("--genetic_maxGenerations","-g",
                "Maximum generations for the genetic algorithm",
                genetic_maxGenerations);
    printOption("--genetic_selectionRate","-s",
                "Selection rate for the genetic algorithm",
                genetic_selectionRate);
    printOption("--genetic_mutationRate","-m",
                "Mutation rate for the genetic algorithm",
                genetic_mutationRate);
    printOption("--genetic_localSearchRate","-l",
                "Local search rate for the genetic algorithm",
                genetic_localSearchRate);
    printOption("--genetic_localSearchMethod","-m",
                "Local search method for the genetic algorithm",
                genetic_localSearchMethod);
    printOption("--bfgs_iterations","-b",
                "Maximum number of iterations for the BFGS method",
                bfgs_iterations);

    printOption("--export_train_file","-e","Filepath where the modified train file will be printed",
                export_train_file);

    printOption("--export_train_file","-e","Filepath where the modified test file will be printed",
                export_test_file);
   exit(0);
}

void checkParameter(
                    QString name,QString value,
                    QString fullName,QString shortName,
                    QString &parameter)
{
    if(name == fullName || name == shortName)
        parameter = value;
}

void checkParameter(
                    QString name,QString value,
                    QString fullName,QString shortName,
                    int &parameter)
{
    if(name == fullName || name == shortName)
        parameter = value.toInt();
}

void checkParameter(
                    QString name,QString value,
                    QString fullName,QString shortName,
                    double &parameter)
{
    if(name == fullName || name == shortName)
        parameter = value.toDouble();
}
void error(QString message)
{
    qDebug().noquote()<<QString("Fatal Error: %1").arg(message,40)<<endl;
    qApp->exit(0);
    exit(0);
}
void parseCmdLine(QStringList args)
{
    QString lastParam="";
    for(int i=1;i<args.size();i++)
    {

        if(args[i]=="--help") printParams();
        QStringList sp=args[i].split("=");
        QString name = sp[0];
        QString value = sp[1];
        if(value=="")
        {
            error(QString("Param %1 is empty.").arg(value));
        }

        checkParameter(name,value,"--trainFile","-f",trainFile);
        checkParameter(name,value,"--testFile","-t",testFile);
        checkParameter(name,value,"--features","-d",features);
        checkParameter(name,value,"--randomSeed","-r",randomSeed);
        checkParameter(name,value,"--featureCreateModel","-k",featureCreatemodel);
        checkParameter(name,value,"--featureEvaluateModel","-e",featureEvaluateModel);
        checkParameter(name,value,"--testIters","-i",testIters);
        checkParameter(name,value,"--threads","-i",threads);
        checkParameter(name,value,"--neural_weights","-w",neural_weights);
        checkParameter(name,value,"--neural_trainingMethod","-m",neural_trainingMethod);
        checkParameter(name,value,"--knn_weights","-w",knn_weights);
        checkParameter(name,value,"--rbf_weights","-w",rbf_weights);

        checkParameter(name,value,"--ge_chromosomes","-c",ge_chromosomes);
        checkParameter(name,value,"--ge_maxGenerations","-g",ge_maxGenerations);
        checkParameter(name,value,"--ge_selectionRate","-s",ge_selectionRate);
        checkParameter(name,value,"--ge_mutationRate","-m",ge_mutationRate);
        checkParameter(name,value,"--ge_length","-l",ge_length);
        checkParameter(name,value,"--ge_localSearchRate","-l",ge_localSearchRate);
        checkParameter(name,value,"--ge_localSearchGenerations","-g",ge_localSearchGenerations);
        checkParameter(name,value,"--ge_localSearchMethod","-m",ge_localSearchMethod);

        checkParameter(name,value,"--genetic_chromosomes","-c",genetic_chromosomes);
        checkParameter(name,value,"--genetic_maxGenerations","-g",genetic_maxGenerations);
        checkParameter(name,value,"--genetic_selectionRate","-s",genetic_selectionRate);
        checkParameter(name,value,"--genetic_mutationRate","-m",genetic_mutationRate);
        checkParameter(name,value,"--genetic_localSearchRate","-l",genetic_localSearchRate);
        checkParameter(name,value,"--genetic_localSearchMethod","-m",genetic_localSearchMethod);
        checkParameter(name,value,"--bfgs_iterations","-b",bfgs_iterations);
        checkParameter(name,value,"--export_train_file","-e",export_train_file);
        checkParameter(name,value,"--export_test_file","-e",export_test_file);
        checkParameter(name,value,"--export_cpp_file","-e",export_cpp_file);
        checkParameter(name,value,"--ge_method","-g",ge_method);

    }
}


NNprogram *defaultProgram;
Model     *defaultModel;
Mapper    *defaultMapper;
int       patternDimension;
vector<Program*> tprogram;
vector<Mapper*> tmapper;
vector<Model*>  tmodel;

void checkTrainAndTest()
{
    if(trainFile=="" || testFile=="")
    {
           error("Trainfile or testfile is missing");
    }
    int d1,d2;
    FILE *fp=fopen(trainFile.toStdString().c_str(),"r");
    if(!fp)
    {
        error(QString("Can not open train file %1").arg(trainFile));
    }
    fscanf(fp,"%d",&d1);
    fclose(fp);

    fp=fopen(testFile.toStdString().c_str(),"r");
    if(!fp)
    {
        error(QString("Can not open test file %1").arg(testFile));
    }
    fscanf(fp,"%d",&d2);
    fclose(fp);
    if(d1!=d2)
    {
        error("Train file has differnt dimension than test file");
    }
    patternDimension = d1;
}


void    executeIntervalGenetic()
{
    IntegerGenetic *pop;
    if(threads<=1)
    pop = new IntegerGenetic(ge_chromosomes,ge_length,defaultProgram);
    else
    {
     pop = new IntegerGenetic(ge_chromosomes,ge_length,tprogram);
    }
    pop->setSelectionRate(ge_selectionRate);
    pop->setMutationRate(ge_mutationRate);
    pop->setMaxGenerations(20);
    pop->run();
    genome = pop->getBestGenome();

    delete pop;
}
void    executeGenetic()
{

    Population *pop;
    if(threads<=1)
    pop = new Population(ge_chromosomes,features * ge_length,defaultProgram);
    else
    {
     pop = new Population(ge_chromosomes,features * ge_length,tprogram);
    }
    pop->setSelectionRate(ge_selectionRate);
    pop->setMutationRate(ge_mutationRate);
    pop->setLocalSearchGenerations(ge_localSearchGenerations);
    pop->setLocalSearchMethod(ge_localSearchMethod);
    pop->setLocalSearchRate(ge_localSearchRate);
    pop->setMaxIters(ge_maxGenerations);
    pop->run();
    genome = pop->getBestGenome();

    delete pop;
}


void    executePso()
{

    PsoGE *pop;
    if(threads<=1)
    pop = new PsoGE(ge_chromosomes,features * ge_length,defaultProgram);
    else
    {
     pop = new PsoGE(ge_chromosomes,features * ge_length,tprogram);
    }
    pop->setMaxIters(ge_maxGenerations);
    pop->run();
    genome = pop->getBestParticle();
    delete pop;
}

void makeGrammaticalEvolution()
{
    if(threads<=1)
    defaultMapper = new Mapper(patternDimension);
    else
    {
        tmapper.resize(threads);
        tmodel.resize(threads);
        for(int i=0;i<threads;i++)
            tmapper[i]=new Mapper(patternDimension);
    }
    if(featureCreatemodel=="neural")
    {
        if(threads<=1)
        {
        defaultModel = new Neural(defaultMapper);
        defaultModel->setPatternDimension(features);
        ((Neural *)defaultModel)->setNumOfWeights(neural_weights);
        ((Neural *)defaultModel)->setLocalSearchMethod(neural_trainingMethod);
        }
        else
        {
            for(int i=0;i<threads;i++)
            {
            tmodel[i] = new Neural(tmapper[i]);
             tmodel[i]->setPatternDimension(features);

            ((Neural *) tmodel[i])->setNumOfWeights(neural_weights);
            ((Neural *) tmodel[i])->setLocalSearchMethod(neural_trainingMethod);
            }
        }
    }
    else
    if(featureCreatemodel=="rbf")
    {
        if(threads<=1)
        {
        defaultModel = new Rbf(defaultMapper);
        defaultModel->setPatternDimension(features);
        ((Rbf *)defaultModel)->setNumOfWeights(rbf_weights);
        }
        else
        {
            for(int i=0;i<threads;i++)
               {
                tmodel[i] = new Rbf(tmapper[i]);
                tmodel[i]->setPatternDimension(features);
                ((Rbf *)tmodel[i])->setNumOfWeights(rbf_weights);
                ((Rbf *)tmodel[i])->randomSeed(1);
            }
        }
    }
    else
    if(featureCreatemodel=="knn")
    {
         defaultModel = new KNN(defaultMapper);
         defaultModel->setPatternDimension(features);
         ((KNN *)defaultModel)->setNumOfWeights(knn_weights);
    }


    else
    if(featureCreatemodel=="osamaRbf")
    {
        if(threads<=1)
        {
        defaultModel=new OsamaRbf(defaultMapper);
        defaultModel->setPatternDimension(features);
        ((OsamaRbf *)defaultModel)->setNumOfWeights(rbf_weights);
        }
        else
        {
            for(int i=0;i<threads;i++)
            {
                tmodel[i]=new OsamaRbf(tmapper[i]);
                tmodel[i]->setPatternDimension(features);
                ((OsamaRbf *)tmodel[i])->setNumOfWeights(rbf_weights);
            }
        }
    }
    else
    if(featureCreatemodel=="copy")
    {
        //make default mapper showing the original features
        delete  defaultMapper;
        FILE *fp=fopen(trainFile.toStdString().c_str(),"r");
        int original_dimension;
        fscanf(fp,"%d",&original_dimension);
        fclose(fp);
        defaultMapper = new Mapper(original_dimension);
        vector<string> sexpr;
        sexpr.resize(original_dimension);
        for(int i=0;i<(int)sexpr.size();i++)
        {
            char ss[10];
            sprintf(ss,"x%d",i+1);
            sexpr[i]=ss;
        }
        defaultMapper->setExpr(sexpr);
        features = original_dimension;
        return ;
    }

    genome.resize(features * ge_length);
    if(threads<=1)
    defaultProgram = new NNprogram(defaultMapper,defaultModel,features,trainFile);


    if(threads>1)
    {
        tprogram.resize(threads);
        for(int i=0;i<threads;i++)
        {
            tprogram[i]=new NNprogram(tmapper[i],tmodel[i],features,trainFile);
        }
    }
    if(ge_method==GEMETHOD_GENETIC)
        executeGenetic();
    else
    if(ge_method == GEMETHOD_PSO)
        executePso();
    else
        executeIntervalGenetic();


    if(threads<=1)
    {
        defaultProgram->printF(genome);
        defaultProgram->fitness(genome);
    }
    else
    {
        ((NNprogram *)tprogram[0])->printF(genome);
        ((NNprogram *)tprogram[0])->fitness(genome);
        defaultMapper=tmapper[0];
        defaultModel = tmodel[0];
    }
}


void    makeTest()
{
    Model *evalModel = NULL;


    if(featureEvaluateModel=="neural")
    {
        evalModel = new Neural(defaultMapper);
         evalModel->readPatterns(trainFile);
        evalModel->setPatternDimension(features);
        ((Neural *)evalModel)->setNumOfWeights(neural_weights);
        ((Neural *)evalModel)->setLocalSearchMethod(neural_trainingMethod);

    }
    else
    if(featureEvaluateModel=="weightNeural")
    {
        evalModel = new Neural(defaultMapper);
         evalModel->readPatterns(trainFile);
        evalModel->setPatternDimension(features);
        ((Neural *)evalModel)->setNumOfWeights(neural_weights);
        ((Neural *)evalModel)->enableWeightDecay();
        ((Neural *)evalModel)->setLocalSearchMethod("genetic");
    }
    else
    if(featureEvaluateModel=="rbf")
    {
        evalModel = new Rbf(defaultMapper);
         evalModel->readPatterns(trainFile);
        evalModel->setPatternDimension(features);
        ((Rbf *)evalModel)->setNumOfWeights(rbf_weights);
    }
    else
    if(featureEvaluateModel=="knn")
    {
         evalModel = new KNN(defaultMapper);
          evalModel->readPatterns(trainFile);
         evalModel->setPatternDimension(features);
         ((KNN *)evalModel)->setNumOfWeights(knn_weights);
    }


    else
    if(featureEvaluateModel=="osamaRbf")
    {
         evalModel = new OsamaRbf(defaultMapper);
          evalModel->readPatterns(trainFile);
         evalModel->setPatternDimension(features);
         ((OsamaRbf *)evalModel)->setNumOfWeights(rbf_weights);
    }
    else
    if(featureEvaluateModel=="nnc")
    {
        evalModel = new NNC(defaultMapper);
         evalModel->readPatterns(trainFile);
        evalModel->setPatternDimension(features);
        ((NNC *)evalModel)->setNumOfWeights(knn_weights);
    }
    double average_train_error = 0.0;
    double average_test_error  = 0.0;
    double average_class_error = 0.0;
    double average_precision = 0.0;
    double average_recall = 0.0;

    if(featureEvaluateModel=="knn")
    {
        vector<Matrix> testx;
        Matrix testy;
        vector<Matrix> distance;
        KNN *knn=(KNN *)evalModel;
        knn->loadTest(testFile,testx,testy);
        knn->makeDistance(testx,distance);
        seedDouble(randomSeed);
        double d;
        d=knn->train2();
        average_train_error+=d;
        double t=evalModel->testError(testFile);
        average_test_error+=t;

        average_class_error+=knn->KNNtestError(testx,testy,distance);
        testIters=1;
    }
    else
    for(int i=0;i<testIters;i++)
    {
        seedInt(randomSeed+i);
        seedDouble(randomSeed+i);
        evalModel->randomSeed(randomSeed+i);
        evalModel->setNumOfWeights(rbf_weights);
        double d=evalModel->train2();
        double t=evalModel->testError(testFile);
        double precision=0.0,recall=0.0;
        double c=evalModel->classTestError(testFile,precision,recall);
        if(isnan(precision) || isinf(precision)) average_precision+=0.0;
        else average_precision+=precision;
        if(isnan(recall) || isinf(recall)) average_recall+=0.0;
        else average_recall+=recall;
        average_train_error+=d;
        average_test_error+=t;
        average_class_error+=c;
        printf("train[%d]=%.10lf test[%d]=%lf class[%d]=%lf%%\n",i,d,i,t,i,c*100.0);

    }
    printf("AVERAGES(TRAIN,TEST,CLASS): %15.8lg %15.8lg %15.8lg%%\n",
           average_train_error/testIters,average_test_error/testIters,
           average_class_error*100.0/testIters);
    printf("AVERAGES(PRECISION,RECALL): %15.8lg %15.8lg\n",average_precision/testIters,
		    average_recall/testIters);
    delete evalModel;
}

void    makeExports()
{
    Model *evalModel = NULL;

    if(export_train_file.length()!=0)
    {
        evalModel = new Rbf(defaultMapper);
        evalModel->readPatterns(trainFile);
        evalModel->setPatternDimension(features);
        evalModel->dumpFile(trainFile,export_train_file);
    }
    if(export_test_file.length()!=0)
    {
        evalModel = new Rbf(defaultMapper);
        evalModel->readPatterns(trainFile);
        evalModel->setPatternDimension(features);
        evalModel->dumpFile(testFile,export_test_file);
    }
    if(export_cpp_file.length()!=0)
    {
        defaultMapper->dumpCppFile(export_cpp_file.toStdString());
    }
    delete evalModel;
}

void    freeMemory()
{
     if(threads<=1)
     {
         delete defaultMapper;
        delete defaultProgram;
         delete defaultModel;

     }
     else
     {
         for(int i=0;i<threads;i++)
         {
             delete tmodel[i];
             delete tmapper[i];
             delete tprogram[i];
         }
     }

}

void    printSystem()
{
    qDebug() << "currentCpuArchitecture():" << QSysInfo::currentCpuArchitecture();
    qDebug() << "productType():" << QSysInfo::productType();
    qDebug() << "productVersion():" << QSysInfo::productVersion();
    qDebug() << "prettyProductName():" << QSysInfo::prettyProductName();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);
    setlocale(LC_ALL,"C");
    printSystem();
    //Parameter checking
    parseCmdLine(app.arguments());
    checkTrainAndTest();
    //Start Grammatical evolution
    makeGrammaticalEvolution();
    //make tests with the constructed features
    makeTest();
    //export data
    makeExports();
    //free memory;
    freeMemory();
    return 0;
}
