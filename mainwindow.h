#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

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
    void sendXRot(int);
    void sendYRot(int);
    void sendZRot(int);
    void sendScale(float);
    void sendTrans(int);

private slots:
    void on_actionOpen_triggered();
    void on_xRotSlider_actionTriggered(int action);
    void on_yRotSlider_actionTriggered(int action);
    void on_zRotSlider_actionTriggered(int action);
    void on_scaleSlider_actionTriggered(int action);

    void on_xTranSlider_actionTriggered(int action);
    void on_yTranSlider_actionTriggered(int action);
    void on_zTranSlider_actionTriggered(int action);

private:
    Ui::MainWindow *ui;
    void readMeshFile(QString);
    void getEdgePair();
    HE_Face *getFaceOppo(HE_Vert*, HE_Vert*);
    int vertCnt = 0, faceCnt = 0, edgeCnt = 0;
    HE_Vert* vert;
    HE_Face* face;
    HE_Edge* edge;

};

#endif // MAINWINDOW_H
