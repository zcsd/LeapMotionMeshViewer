#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QObject>
#include <QtOpenGL>
#include <QtMath>

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
    //QSize minimumSizeHint() const;
    //QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:

public slots:
    void setRotation(int, int, int);
    void setScale(float);
    void setTrans(int, int, int);
    void receiveMesh(HE_Face*, int);

private:
    int xRot, yRot, zRot;
    float preScale = 1.0, scale = 1.0;
    int tranX = 0, tranY = 0, tranZ = 0;
    bool meshReady = false;
    int faceCnt = 0;
    HE_Face* face;
    QPoint lastPos;
    void normalizeAngle(int &angle);
    void drawMesh();
    void drawXYPlane();
    void drawXYZAxis();

};

#endif // GLWIDGET_H
