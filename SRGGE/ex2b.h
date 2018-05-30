#ifndef EX2B_H
#define EX2B_H

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





class ex2b: public ex1
{
    Q_OBJECT

public:
    ex2b (const QGLFormat &glf, QWidget * parent = 0);
    QGroupBox*      controlPanel();
    void            initializeGL();
    void            initVertexBuffer();


    void vertexClustering();
    void octreeVertexClustering();



    int             level=2;
    bool            LODsimpleON=false;

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


};










#endif // EX2B_H
