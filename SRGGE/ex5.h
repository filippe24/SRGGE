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


#include "FRR/Viewer1.h"

class ex5 : public Viewer1
{
    Q_OBJECT

public:
    ex5		(const QGLFormat &glf, QWidget *parent = 0);
    void		initVertexBuffer();		// init vertices
    QGroupBox*  controlPanel();

    void            initializeWorld();
    void            createGround();
    void            createWall();
    bool            LoadModel(QString filename);



    //variables
    bool startPrinting = false;
    int             worldDimension = 0;
    float           tileDimension = 0;
    std::vector<std::vector<int>> world;
    std::vector<float>      ground_vertices;
    std::vector<int>        ground_faces;
    std::vector<float>      ground_normals;

    std::vector<float>      wall_vertices;
    std::vector<int>        wall_faces;
    std::vector<float>      wall_normals;

    std::vector<float>      new_vertices;
    std::vector<int>        new_faces;
    std::vector<float>      new_normals;

    std::unique_ptr<data_representation::TriangleMesh> mesh1;
    float                   scalingFactor;

    GLuint vaoG, vaoW;
    GLuint vboVertexG, vboVertexW;
    GLuint vboNormalG, vboNormalW;
    GLuint vboIndexG, vboIndexW;


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
    void startMuseum();


signals:
    void SetFramerate(QString);

};


#endif // ex5_H
