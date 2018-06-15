#ifndef ex2_H
#define ex2_H


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
#include "SRGGE/ex1.h"
#include <tuple>
#include "Helpers/octree.h"





class ex2: public ex1
{
    Q_OBJECT

public:
    ex2 (const QGLFormat &glf, QWidget * parent = 0);
    QGroupBox*      controlPanel();
    void            initializeGL();
    void            initVertexBuffer();


    void vertexClustering();
    void octreeVertexClustering();



    int             level=2;
    bool            vertexClusteringActivate=false;
    bool            octreeActivate=false;

    int maxLevel = 100;
    int intervalLevel = 5;

    bool            createdOctree = false;
    octree *oc;


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

};

#endif // ex2_H
