#ifndef ex5_H
#define ex5_H

#include <QtWidgets>
#include <QGLWidget>
#include <QGLFunctions>
#include <QGLShaderProgram>
#include <QtOpenGL>
#include <fstream>
#include <iostream>
#include <memory>
#include <QKeyEvent>
#include <vector>

#include "FRR/Viewer1.h"

class ex5: public Viewer1
{

public:
    ex5             (const QGLFormat &glf, QWidget *parent = 0);
    void            initVertexBuffer();
    QGroupBox*      controlPanel();

    void            initializeWorld();
    void            createGround();
    void            createWall();

    bool            startPrinting = false;
    int             worldDimension = 0;
    float           tileDimension = 0;

    std::vector<std::vector<int>> world;
    std::vector<float>      ground_vertices;
    std::vector<int>        ground_faces;
    std::vector<float>      ground_normals;

    std::vector<float>      new_vertices;
    std::vector<int>        new_faces;
    std::vector<float>      new_normals;


    GLuint vao, vaoG;
    GLuint vboVertexG;
    GLuint vboNormalG;
    GLuint vboIndexG;



    QElapsedTimer   t_Timer;
    double          lastTime;
    int nbFrames=0;
    int final_time, initial_time=time(NULL), frames=0;

protected:
    void            initializeGL	(); // init GL state
    void    		paintGL 		();


signals:

public slots:
    void startMuseum();
};

#endif // ex5_H
