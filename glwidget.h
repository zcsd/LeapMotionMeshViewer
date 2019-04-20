// GLWidget class: subclass of QGLWifget, to render mesh
// Author: @ZC
// Date: created on 04 Apr 2019
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QObject>
#include <QtOpenGL>

#include "mainwindow.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

public slots:
    void receiveMesh(HE_Face*, int);
    void setRotation(int, int, int);
    void setScale(float);
    void setTrans(int, int, int);
    void receiveReset();

private:
    HE_Face* face;
    int faceCnt = 0;
    bool meshReady = false;

    int xRot = 0, yRot = 0, zRot = 0;
    float scale = 1.0;
    int tranX = 0, tranY = 0, tranZ = 0;

    void drawMesh();
    void drawXYPlane();
    void drawXYZAxis();
};

#endif // GLWIDGET_H
