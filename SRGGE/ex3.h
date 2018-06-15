#ifndef ex3_H
#define ex3_H


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
#include "SRGGE/ex2.h"
#include <tuple>
#include "Helpers/octree.h"





class ex3: public ex2
{
    Q_OBJECT

public:
    ex3 (const QGLFormat &glf, QWidget * parent = 0);
    QGroupBox*      controlPanel();
    void            initializeGL();
    void            initVertexBuffer();

    void vertexClustering();
    void octreeVertexClustering();

    void quadricVertexClustering();



    int             level=2;
    bool            vertexClusteringActivate=false;
    bool            octreeActivate=false;
    bool            quadricActivate = false;

    int maxLevel = 100;
    int intervalLevel = 5;

    bool            createdOctree = false;
    octree *oc;

    std::vector<Eigen::Matrix4d> QMatrix;
    std::vector<float>      new_vertices;
    std::vector<int>        new_faces;
    std::vector<float>      new_normals;


protected:
    void		paintGL		();		// render GL scene

signals:
    void SetVertices(QString);
    void SetFaces(QString);

public slots:
    void setNumberlod(int);
    void deactivate();
    void activateBasic();
    void activateOctree();
    void createOctree();

    void createQuadric();

};





#endif // EX3_H
