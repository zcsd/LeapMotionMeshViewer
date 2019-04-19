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
    void sendRot(int, int, int);
    void sendScale(float);
    void sendTrans(int, int, int);

    void sendController(const Controller&);

private slots:
    void on_actionOpen_triggered();
    void on_xRotSlider_actionTriggered(int action);
    void on_yRotSlider_actionTriggered(int action);
    void on_zRotSlider_actionTriggered(int action);
    void on_scaleSlider_actionTriggered(int action);

    void on_xTranSlider_actionTriggered(int action);
    void on_yTranSlider_actionTriggered(int action);
    void on_zTranSlider_actionTriggered(int action);

    void receiveTrigger();
    void onFrame(const Controller&);

private:
    Ui::MainWindow *ui;
    Controller controller;
    void readMeshFile(QString);
    void getEdgePair();
    HE_Face *getFaceOppo(HE_Vert*, HE_Vert*);
    int vertCnt = 0, faceCnt = 0, edgeCnt = 0;
    HE_Vert* vert;
    HE_Face* face;
    HE_Edge* edge;

    float preTx = 0, preTy = 200, preTz = 0;
    float preRx = 0, preRy = 200, preRz = 0;
    int angleX = 0, angleY = 0, angleZ = 0;
    void doTranslation(float, float, float);
    void doRotation(float, float, float);
    void doScale(float, float, float);

    void onInit(const Controller&);
    void onConnect(const Controller&);

    QTimer *frameTrigger;

    bool isMotionStop = false;
    int nonStopGestureFrames = 0;
    float firstHandsDistance = 0, preDistance = 0;
    int leftHandConCnt = 0, rightHandConCnt = 0, twoHandsConCnt = 0;

};

#endif // MAINWINDOW_H
