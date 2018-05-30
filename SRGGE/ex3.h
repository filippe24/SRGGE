#ifndef EX3_H
#define EX3_H


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





class ex3: public ex1
{
    Q_OBJECT

public:
    ex3 (const QGLFormat &glf, QWidget * parent = 0);
    QGroupBox*      controlPanel();
    void            initializeGL();
    void            initVertexBuffer();


    void vertexClustering();
    void octreeVertexClustering();



    int             level=2;
    bool            LODsimpleON=false;
    bool            octreeON=false;

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
    void setOFF();
    void setOnBasic();
    void setOnOctree();
    void createOctree();

};





#endif // EX3_H
