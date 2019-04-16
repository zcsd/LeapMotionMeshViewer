#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(sendMesh(HE_Face*, int)), ui->glShowWidget, SLOT(receiveMesh(HE_Face*, int)));
    connect(this, SIGNAL(sendXRot(int)), ui->glShowWidget, SLOT(setXRotation(int)));
    connect(this, SIGNAL(sendYRot(int)), ui->glShowWidget, SLOT(setYRotation(int)));
    connect(this, SIGNAL(sendZRot(int)), ui->glShowWidget, SLOT(setZRotation(int)));
    connect(this, SIGNAL(sendScale(float)), ui->glShowWidget, SLOT(setScale(float)));
    connect(this, SIGNAL(sendTrans(int)), ui->glShowWidget, SLOT(setTrans(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString openFilePath = QFileDialog::getOpenFileName(this, "Open Mesh File", "/", "Mesh (*.m)");

    if (openFilePath.isEmpty()) {
        QMessageBox::warning(this,"File Selection Error", "Please Select Correct Mesh File.", QMessageBox::Yes);
    }
    else {
        readMeshFile(openFilePath);
        sendMesh(face, faceCnt);
        //getEdgePair();
    }
}

void MainWindow::readMeshFile(QString filePath)
{
    QFile meshFile(filePath);
    meshFile.open(QIODevice::ReadOnly);
    QTextStream in(&meshFile);

    vertCnt = 0;
    faceCnt = 0;
    QString line;

    //get the number of vertx and face in mesh file
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.contains("Vertex")) {
            vertCnt++;
        }
        else if (line.contains("Face")){
            faceCnt++;
        }
    }
    in.seek(0); // back to stream 0 position

    edgeCnt = faceCnt * 3; // each triangle contain three edges
    int edgeIndex = 0, faceIndex = 0;

    vert = new HE_Vert[vertCnt];
    edge = new HE_Edge[edgeCnt];
    face = new HE_Face[faceCnt];

    HE_Vert* vert0;
    HE_Vert* vert1;
    HE_Vert* vert2; // for face temp ussing

    //load raw data to HE structure
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.contains("Vertex")) {
            line.remove("Vertex ").remove("{normal=(").remove(")}");
            QStringList tempStrList1 = line.replace("  ", " ").split(" ");

            if (tempStrList1.size() == 7) {
                vert[tempStrList1.at(0).toInt() - 1].index = tempStrList1.at(0).toInt() - 1;
                vert[tempStrList1.at(0).toInt() - 1].x = tempStrList1.at(1).toFloat();
                vert[tempStrList1.at(0).toInt() - 1].y = tempStrList1.at(2).toFloat();
                vert[tempStrList1.at(0).toInt() - 1].z = tempStrList1.at(3).toFloat();
                vert[tempStrList1.at(0).toInt() - 1].norm_x = tempStrList1.at(4).toFloat();
                vert[tempStrList1.at(0).toInt() - 1].norm_y = tempStrList1.at(5).toFloat();
                vert[tempStrList1.at(0).toInt() - 1].norm_z = tempStrList1.at(6).toFloat();
            }
            else {
                qDebug() << "Wrong vertex format is detected in mesh file.";
            }

        }
        else if (line.contains("Face")) {
            line.remove("Face ");
            QStringList tempStrList2 = line.replace("  ", " ").split(" ");

            if (tempStrList2.size() == 4) {
                face[tempStrList2.at(0).toInt() - 1].index = tempStrList2.at(0).toInt() - 1;

                vert0 = &vert[tempStrList2.at(1).toInt() - 1];
                vert1 = &vert[tempStrList2.at(2).toInt() - 1];
                vert2 = &vert[tempStrList2.at(3).toInt() - 1];

                edge[edgeIndex]._face = &face[faceIndex];
                edge[edgeIndex + 1]._face = &face[faceIndex];
                edge[edgeIndex + 2]._face = &face[faceIndex];
                edge[edgeIndex].prev = &edge[edgeIndex + 2];
                edge[edgeIndex].next = &edge[edgeIndex + 1];
                edge[edgeIndex + 1].prev = &edge[edgeIndex];
                edge[edgeIndex + 1].next = &edge[edgeIndex + 2];
                edge[edgeIndex + 2].prev = &edge[edgeIndex + 1];
                edge[edgeIndex + 2].next = &edge[edgeIndex];
                edge[edgeIndex]._vert = vert1;
                edge[edgeIndex + 1]._vert = vert2;
                edge[edgeIndex + 2]._vert = vert0;
                edge[edgeIndex].index = edgeIndex;
                edge[edgeIndex + 1].index = edgeIndex + 1;
                edge[edgeIndex + 2].index = edgeIndex + 2;
                edge[edgeIndex].pair = nullptr;
                edge[edgeIndex + 1].pair = nullptr;
                edge[edgeIndex + 2].pair = nullptr;
                vert0->_edge = &edge[edgeIndex];
                vert1->_edge = &edge[edgeIndex + 1];
                vert2->_edge = &edge[edgeIndex + 2];
                face[faceIndex]._edge = &edge[edgeIndex];
                faceIndex++;
                edgeIndex += 3;
            }
            else {
                qDebug() << "Wrong face format is detected in mesh file";
            }
        }
    }

    meshFile.close();
    qDebug() << "Mesh file loaded successfully:" << vertCnt << "vertex and" << faceCnt << "face.";
}

void MainWindow::getEdgePair()
{
    HE_Face* oppo;
    HE_Edge* edge1;
    HE_Vert* vert1;
    HE_Vert* vert2;

    for (int i =0; i < edgeCnt; i++) {
        vert1 = edge[i]._vert;
        vert2 = edge[i].prev->_vert;
        oppo = getFaceOppo(vert2, vert1);
        if (oppo != nullptr) {
            edge1 = oppo->_edge;
            while (edge1->_vert != vert2) {
                edge1 = edge1->next;
            }

            edge[i].pair = edge1;
        }
        else {
            edge[i].pair = nullptr;
        }
    }
}

HE_Face* MainWindow::getFaceOppo(HE_Vert* inVert1, HE_Vert* inVert2)
{
    HE_Face* tmpFace;
    HE_Vert *vert0, *vert1, *vert2;
    for (int i = 0; i < edgeCnt; i = i + 3) {
        tmpFace = edge[i]._face;
        vert0 = edge[i]._vert;
        vert1 = edge[i + 1]._vert;
        vert2 = edge[i + 2]._vert;
        if (vert1->index == inVert1->index && vert0->index == inVert2->index)
            return tmpFace;
        if (vert0->index == inVert1->index && vert2->index == inVert2->index)
            return tmpFace;
        if (vert2->index == inVert1->index && vert1->index == inVert2->index)
            return tmpFace;
    }
    return nullptr;
}

void MainWindow::on_xRotSlider_actionTriggered(int action)
{
    Q_UNUSED(action);
    qDebug() << "X Rot:" << ui->xRotSlider->value();
    sendXRot(ui->xRotSlider->value());
}

void MainWindow::on_yRotSlider_actionTriggered(int action)
{
    Q_UNUSED(action);
    qDebug() << "Y Rot:" << ui->yRotSlider->value();
    sendYRot(ui->yRotSlider->value());
}

void MainWindow::on_zRotSlider_actionTriggered(int action)
{
    Q_UNUSED(action);
    qDebug() << "Z Rot:" << ui->zRotSlider->value();
    sendZRot(ui->zRotSlider->value());
}

void MainWindow::on_scaleSlider_actionTriggered(int action)
{
    Q_UNUSED(action);
    qDebug() << "Scale:" << ui->scaleSlider->value();
    sendScale(float(ui->scaleSlider->value() * 0.1));
}

void MainWindow::on_xTranSlider_actionTriggered(int action)
{
    Q_UNUSED(action);
    qDebug() << "x Tran:" << ui->xTranSlider->value();
    sendTrans(ui->xTranSlider->value());
}

void MainWindow::on_yTranSlider_actionTriggered(int action)
{
    Q_UNUSED(action);
}

void MainWindow::on_zTranSlider_actionTriggered(int action)
{
    Q_UNUSED(action);
}
