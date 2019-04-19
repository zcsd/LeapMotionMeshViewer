#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    static GLfloat lightPosition[4] = { 0, 0, 10, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(float(xRot), 1.0, 0.0, 0.0);
    glRotatef(float(yRot), 0.0, 1.0, 0.0);
    glRotatef(float(zRot), 0.0, 0.0, 1.0);
    drawMesh();
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.1, +0.1, -0.1, +0.1, 1.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    //lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    /*
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }

    lastPos = event->pos();*/
}

void GLWidget::setRotation(int angleX, int angleY, int angleZ)
{
    //normalizeAngle(angle);
    /*
    if (angle != xRot) {
        xRot = angle;
        updateGL();
    }*/
    xRot = angleX*2;
    yRot = angleY*2;
    zRot = angleZ*2;
    updateGL();
}

void GLWidget::setScale(float s)
{
    /*
    if (qAbs(double(s-scale)) > 0.01) {
        scale = s;
        updateGL();
    }*/
    if (s > 1) {
        scale = preScale + s - 1;
        if (scale >= 5) scale = 5;
    }
    else if (s < 1 && s > 0) {
        scale = preScale - (1 - s);
        if (scale <= 0.1) scale = 0.1;
    }

    preScale = scale;
    qDebug() << "scale in gl:" << scale;
    //preScale = scale;
    updateGL();
}

void GLWidget::setTrans(int tx, int ty, int tz)
{
    tranX = tx;
    tranY = ty;
    tranZ = tz;
    updateGL();
}

void GLWidget::receiveMesh(HE_Face *inFace, int cnt)
{
    meshReady = true;
    face = inFace;
    faceCnt = cnt;
    updateGL();
}

void GLWidget::normalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360) //360 * 16
        angle -= 360 * 16;
}

void GLWidget::drawMesh()
{
    drawXYPlane();
    drawXYZAxis();
    glScalef(0.6f*scale, 0.6f*scale, 0.6f*scale);
    glTranslatef(-0.05f, -0.03f, -0.05f);
    glTranslatef(tranX*0.01f, tranY*0.01f, tranZ*0.01f);
    HE_Edge* curr;
    if (meshReady) {
        for (int i = 0; i < faceCnt; i++) {
            //qDebug() << i << face[i].index << face[i]._edge->_vert->index;
            curr = face[i]._edge;

            glColor3f(0.2f, 0.7f, 0.2f);
            glBegin(GL_TRIANGLES);
                glNormal3f(curr->_vert->norm_x, curr->_vert->norm_y, curr->_vert->norm_z);
                glVertex3f(curr->_vert->x, curr->_vert->y, curr->_vert->z);
                glNormal3f(curr->next->_vert->norm_x, curr->next->_vert->norm_y, curr->next->_vert->norm_z);
                glVertex3f(curr->next->_vert->x, curr->next->_vert->y, curr->next->_vert->z);
                glNormal3f(curr->next->next->_vert->norm_x, curr->next->next->_vert->norm_y, curr->next->next->_vert->norm_z);
                glVertex3f(curr->next->next->_vert->x, curr->next->next->_vert->y, curr->next->next->_vert->z);
            glEnd();
        }
    }
}

void GLWidget::drawXYPlane()
{
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    for (int i = 0; i <= 10; i++) {
        glVertex3f(i*0.1f, 0, 0);
        glVertex3f(i*0.1f, 0, 10*0.1);
        glVertex3f(0, 0, i*0.1f);
        glVertex3f(10*0.1, 0, i*0.1f);
    };

    for (int i = 0; i >= -10; i--) {
        glVertex3f(i*0.1f, 0, 0);
        glVertex3f(i*0.1f, 0, -10*0.1);
        glVertex3f(0, 0, i*0.1f);
        glVertex3f(-10*0.1, 0, i*0.1f);
    };

    for (int i = 0; i <= 10; i++) {
        glVertex3f(i*(-1)*0.1f, 0, 0);
        glVertex3f(i*(-1)*0.1f, 0, 10*0.1);
        glVertex3f(0, 0, i*0.1f);
        glVertex3f(-10*0.1, 0, i*0.1f);
    };

    for (int i = 0; i <= 10; i++) {
        glVertex3f(i*0.1f, 0, 0);
        glVertex3f(i*0.1f, 0, -10*0.1);
        glVertex3f(0, 0, i*(-1)*0.1f);
        glVertex3f(10*0.1, 0, i*(-1)*0.1f);
    };
    glEnd();
}

void GLWidget::drawXYZAxis()
{
    // x
    glColor3f(1.0,0.0,0.0); // red x
    glBegin(GL_LINES);
        glVertex3f(-0.1f, 0.0f, 0.0f);
        glVertex3f(0.1f, 0.0f, 0.0f);

        glVertex3f(0.1f, 0.0f, 0.0f);
        glVertex3f(0.09f, 0.0f, 0.01f);

        glVertex3f(0.1f, 0.0f, 0.0f);
        glVertex3f(0.09f, 0.0f, -0.01f);
    glEnd();

    // y
    glColor3f(0.0,1.0,0.0); // green y
    glBegin(GL_LINES);
        glVertex3f(0.0, -0.1f, 0.0f);
        glVertex3f(0.0, 0.1f, 0.0f);

        glVertex3f(0.0, 0.1f, 0.0f);
        glVertex3f(0.01f, 0.09f, 0.0f);

        glVertex3f(0.0, 0.1f, 0.0f);
        glVertex3f(-0.01f, 0.09f, 0.0f);
    glEnd();

    // z
    glColor3f(0.0,0.0,1.0); // blue z
    glBegin(GL_LINES);
        glVertex3f(0.0, 0.0f ,-0.1f );
        glVertex3f(0.0, 0.0f ,0.1f );

        glVertex3f(0.0, 0.0f ,0.1f );
        glVertex3f(0.01f, 0.0f ,0.09f );

        glVertex3f(0.0, 0.0f, 0.1f );
        glVertex3f(-0.01f, 0.0f, 0.09f );
    glEnd();
}
