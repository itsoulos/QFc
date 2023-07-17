QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
LIBS+=  -lm -fopenmp

QMAKE_CXXFLAGS_RELEASE += -O3 -march=native -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread -fopt-info-optimized-vec-loop-omp
QMAKE_CFLAGS_RELEASE += -O3 -march=native -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread -fopt-info-optimized-vec-loop-omp

QMAKE_CXXFLAGS += -O3 -march=native -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread -fopt-info-optimized-vec-loop-omp
QMAKE_CFLAGS += -O3 -march=native -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread -fopt-info-optimized-vec-loop-omp
INCLUDEPATH+=/usr/include/eigen3/

QMAKE_LFLAGS += -O3
SOURCES += \
        KmeansPP.cpp \
        QfcRandom.cc \
        Rbf.cc \
        collection.cc \
        converter.cc \
        cprogram.cc \
        doublestack.cc \
        fparser.cc \
        fpoptimizer.cc \
        gensolver.cc \
        getoptions.cpp \
        grs.cc \
        kmeans.cc \
        knn.cc \
        lbfgs.cpp \
        main.cpp \
        mapper.cc \
        matrix_functions.cc \
        model.cc \
        neural.cc \
        neuralparser.cc \
        neuralprogram.cc \
        nnc.cpp \
        nncneuralprogram.cc \
        nnprogram.cc \
        osamarbf.cpp \
        population.cc \
        problem.cc \
        program.cc \
        psoge.cpp \
        rbf_model.cc \
        rbfsolver.cc \
        rlsprogram.cc \
        rule.cc \
        sigprogram.cc \
        symbol.cc \
        tolmin.cc

LIBS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    KmeansPP.h \
    QfcRandom.h \
    Rbf.h \
    Util.h \
    collection.h \
    converter.h \
    cprogram.h \
    doublestack.h \
    f2c.h \
    fparser.hh \
    fpconfig.hh \
    fptypes.hh \
    gensolver.h \
    getoptions.h \
    grs.h \
    kmeans.h \
    knn.h \
    lbfgs.h \
    mapper.h \
    matrix_functions.h \
    model.h \
    neural.h \
    neuralparser.h \
    neuralprogram.h \
    nnc.h \
    nncneuralprogram.h \
    nnprogram.h \
    osamarbf.h \
    population.h \
    problem.h \
    program.h \
    psoge.h \
    rbf_model.h \
    rbfsolver.h \
    rlsprogram.h \
    rule.h \
    sigprogram.h \
    symbol.h
