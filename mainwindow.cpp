#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(sendMesh(HE_Face*, int)), ui->glShowWidget, SLOT(receiveMesh(HE_Face*, int)));
    connect(this, SIGNAL(sendRot(int, int, int)), ui->glShowWidget, SLOT(setRotation(int, int, int)));
    connect(this, SIGNAL(sendScale(float)), ui->glShowWidget, SLOT(setScale(float)));
    connect(this, SIGNAL(sendTrans(int, int, int)), ui->glShowWidget, SLOT(setTrans(int, int, int)));
    connect(this, SIGNAL(sendReset()), ui->glShowWidget, SLOT(receiveReset()));
    onInit(controller);
    onConnect(controller);
}

MainWindow::~MainWindow()
{
    delete frameTrigger;
    delete ui;
}

void MainWindow::onInit(const Controller &cont)
{
    Q_UNUSED(cont);
    qDebug() << "Leap Motion Service Init";
}

void MainWindow::onConnect(const Controller &cont)
{
    qDebug() << "Leap Motion Service connected";
    cont.enableGesture(Gesture::TYPE_CIRCLE);
    cont.enableGesture(Gesture::TYPE_SWIPE);
}

void MainWindow::onFrame(const Controller &cont)
{
    const Frame frame = cont.frame();

    if (frame.gestures().count() != 0) {
        if (frame.gestures()[0].type() == Leap::Gesture::TYPE_CIRCLE) {
            isMotionStop = true;
            nonStopGestureFrames = 0;
            leftHandConCnt = 0;
            rightHandConCnt = 0;
            twoHandsConCnt = 0;
            qDebug() << "Circle Gesture detected.";
        }

        if (frame.gestures()[0].type() == Leap::Gesture::TYPE_SWIPE) {
            isMotionStop = true;
            nonStopGestureFrames = 0;
            leftHandConCnt = 0;
            rightHandConCnt = 0;
            twoHandsConCnt = 0;
            qDebug() << "Swipe Gesture detected.";
            emit sendReset();
        }
    }
    else {
        nonStopGestureFrames++;
        if (nonStopGestureFrames >= 60) {
            isMotionStop = false;
            nonStopGestureFrames = 61;
        }
    }

    HandList hands = frame.hands();

    if (frame.hands().count() == 1 && isMotionStop == false) {
        twoHandsConCnt = 0;
        Hand hand = hands[0];
        if (hand.isLeft()) {
            leftHandConCnt++;
            qDebug() << "1 Left hand is detected.";
            Vector handCenter = hand.palmPosition();

            if (leftHandConCnt > 60) {
                doTranslation(handCenter.x, handCenter.y, handCenter.z);
            }
        }
        else if (hand.isRight()) {
            rightHandConCnt++;
            qDebug() << "1 Right hand is detected.";
            Vector handCenter = hand.palmPosition();

            if (rightHandConCnt > 60) {
                doRotation(handCenter.x, handCenter.y, handCenter.z);
            }
        }
    }
    else if (frame.hands().count() == 2 && isMotionStop == false) {
        leftHandConCnt = 0;
        rightHandConCnt = 0;
        twoHandsConCnt++;
        qDebug() << "2 hands are detected.";

        Hand leftHand, rightHand;
        if (hands[0].isLeft()) {
            leftHand = hands[0];
            rightHand = hands[1];
        }
        else {
            leftHand = hands[1];
            rightHand = hands[0];
        }

        if (twoHandsConCnt > 40) {
            Vector handCenter = rightHand.palmPosition();
            doScale(handCenter.x, handCenter.y, handCenter.z);
        }
    }
    else if (frame.hands().count() == 0) {
        //qDebug() << "No hands is detected.";
    }
    else {
        //qDebug() << "More than two hands are detected.";
    }
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

        frameTrigger = new QTimer();
        frameTrigger->setInterval(30);

        connect(frameTrigger, SIGNAL(timeout()), this, SLOT(receiveTrigger()));
        connect(this, SIGNAL(sendController(const Controller&)), this, SLOT(onFrame(const Controller&)));

        frameTrigger->start();
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

void MainWindow::doTranslation(float x, float y, float z)
{
    float xTFactor = (x - preTx) / 6;
    float yTFactor = (y - preTy) / 6;
    float zTFactor = (z - preTz) / 6;

    qDebug() << "Translation:" << xTFactor << yTFactor << zTFactor;

    if ( (xTFactor > 1 || xTFactor < -1)
         || (yTFactor > 1 || yTFactor < -1)
         || (zTFactor > 1 || zTFactor < -1) ) {
        ui->xTranSlider->setValue(int(xTFactor));
        ui->yTranSlider->setValue(int(yTFactor));
        ui->zTranSlider->setValue(int(zTFactor));
        emit sendTrans(int(xTFactor), int(yTFactor), int(zTFactor));
    }
}

void MainWindow::doRotation(float x, float y, float z)
{
    float angleX = (x - preRx);
    float angleY = (y - preRy);
    float angleZ = (z - preRz);
    qDebug() << "Rotation:" << angleX << angleY << angleZ;

    ui->xRotSlider->setValue(int(angleZ));
    ui->yRotSlider->setValue(int(angleY));
    ui->zRotSlider->setValue(int(angleX));
    emit sendRot(int(angleZ), int(angleY), int(angleX));
}

void MainWindow::doScale(float x, float y, float z)
{
    Q_UNUSED(x);
    Q_UNUSED(z);
    float scale;

    if ( (y - preSy) > 0 ) {
        scale = 1 + ( (y - preSy) / 60 );
        if (scale >= 4 ) scale = 4;
    }
    else {
        scale = 1 - ( (preSy - y) / 100);
        if (double(scale) <= 0.2) scale = 0.2f;
    }

    qDebug() << "Scale:" << scale;

    ui->scaleSlider->setValue(int(scale*10));
    emit sendScale(scale);
}

void MainWindow::receiveTrigger()
{
    emit sendController(controller);
}
