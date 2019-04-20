// MainWindow class: contain main control and half-edge parser
// Author: @ZC
// Date: created on 02 Apr 2019
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

#include "Leap.h"

using namespace Leap;

/************Half-Edge Data Structure*************/
struct HE_Vert;
struct HE_Edge;
struct HE_Face;

struct HE_Vert
{
    float x, y, z;
    float norm_x, norm_y, norm_z;
    HE_Edge* _edge;
    int index;
};

struct HE_Edge
{
    HE_Vert* _vert;
    HE_Face* _face;
    HE_Edge* pair;
    HE_Edge* prev;
    HE_Edge* next;
    int index;
};

struct HE_Face
{
    HE_Edge* _edge;
    int index;
};
/**************************************************/

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void sendMesh(HE_Face*, int);
    void sendController(const Controller&);
    void sendRot(int, int, int);
    void sendScale(float);
    void sendTrans(int, int, int);
    void sendReset();

private slots:
    void on_actionOpen_triggered();
    void receiveTrigger();
    void onFrame(const Controller&); // leap motion function

private:
    Ui::MainWindow *ui;
    QTimer *frameTrigger; // used to loop frames
    Controller controller;

    HE_Vert* vert;
    HE_Face* face;
    HE_Edge* edge;
    int vertCnt = 0, faceCnt = 0, edgeCnt = 0;

    // default or calibrated position for different motion
    float preTx = 0, preTy = 200, preTz = 0; // Translation
    float preRx = 0, preRy = 200, preRz = 0; // Rotation
    float preSy = 200; // Scale

    bool isMotionStop = false;
    int nonStopGestureFrames = 0;
    int leftHandConCnt = 0, rightHandConCnt = 0, twoHandsConCnt = 0;

    void readMeshFile(QString);
    void doTranslation(float, float, float);
    void doRotation(float, float, float);
    void doScale(float, float, float);

    // leap motion function
    void onInit(const Controller&);
    void onConnect(const Controller&);
};

#endif // MAINWINDOW_H
