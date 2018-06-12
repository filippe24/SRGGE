#ifndef ex6_H
#define ex6_H


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
#include "Helpers/visibility.h"


class ex6 : public Viewer1
{
    Q_OBJECT

public:
    ex6		(const QGLFormat &glf, QWidget *parent = 0);
    void		initVertexBuffer();		// init vertices
    QGroupBox*  controlPanel();

    bool            museumIsInitialized = false;
    bool            visibilityIsComputed = false;
    visibility      *vs;

    bool            openFile(const std::string &filename);
    bool            isInterior( int i, int j);
    void            createGround();
    void            createWall();
    bool            LoadModel(QString filename, int i);

    //define the number of models on the museum file ( how many number >2 )
    int             numberOfModels = 0;




    //variables
    bool startPrinting = false;
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

    std::vector<std::unique_ptr<data_representation::TriangleMesh>> meshes;
    std::vector<float>      scalingFactors;
//    float                   scalingFactor;


    std::vector<GLuint> vaoMs;
    std::vector<GLuint> vboVertexMs;
    std::vector<GLuint> vboNormalMs;
    std::vector<GLuint> vboIndexMs;

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
    void initializeWorld();
    void computeVisibility();



signals:
    void SetFramerate(QString);

};


#endif // ex6_H
