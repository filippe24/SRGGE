#ifndef ex4_H
#define ex4_H


#include <QtWidgets>
#include <QGLWidget>
#include <QGLFunctions>
#include <QGLShaderProgram>
#include <QtOpenGL>
#include <fstream>
#include <iostream>
#include <memory>
#include <QKeyEvent>


#include "SRGGE/ex2.h"

class ex4 : public ex2
{
    Q_OBJECT

public:
    ex4		(const QGLFormat &glf, QWidget *parent = 0);
    void		initVertexBuffer();		// init vertices
    QGroupBox*  controlPanel();

    void computeLevels();
    void uploadNumTotalFaces();
    float computeCost(int i, int j);


    std::vector<std::vector<int>>     levels;
    int oldLevel;
    int numTotalFaces = 0;

    std::vector<std::vector<float>>   vertices;
    std::vector<std::vector<float>>   normals;
    std::vector<std::vector<int>>   faces;


    GLuint vao0, vao1, vao2, vao3;
    GLuint vboVertex0, vboVertex1, vboVertex2, vboVertex3;
    GLuint vboNormal0, vboNormal1, vboNormal2, vboNormal3;
    GLuint vboIndex0, vboIndex1, vboIndex2, vboIndex3;

    std::vector<GLuint> vaoV;
    std::vector<GLuint> vboVertexV;
    std::vector<GLuint> vboNormalV;
    std::vector<GLuint> vboIndexeV;


    //time and shader
    GLuint          depthrenderbuffer;
    int             copies=1;
    QElapsedTimer   t_Timer;
    double          lastTime;
    int             nbFrames=0;
    Shader          *gShader;
    QLabel          *print_fps;
    int final_time, initial_time=time(NULL), frames=0;

protected:
    void		initializeGL	();		// init GL state
    void		paintGL		();		// render GL scene


public slots:
    void setNumberCopies(int);


signals:
    void SetFramerate(QString);

};
#endif // ex4_H
