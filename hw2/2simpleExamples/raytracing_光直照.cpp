//正方体，茶壶，球体，光照
//#include "stdafx.h"
#include <GL/glut.h>
//#include <stdlib.h>

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat xR = 0.0f;
static GLfloat yR = 0.0f;

//自定义初始化opengl函数
void init(void)
{
    //材质反光性设置
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };  //镜面反射参数
    GLfloat mat_shininess[] = { 50.0 };               //高光指数
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };   //灯位置(1,1,1), 最后1-开关
    GLfloat Light_Model_Ambient[] = { 0.3, 0.5, 0.2, 1.0 }; //环境光参数

    glClearColor(0.0, 0.0, 0.0, 0.0);  //背景色
    glShadeModel(GL_SMOOTH);           //多变性填充模式

    //材质属性
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    //灯光设置
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);   //散射光属性
    glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);  //镜面反射光
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Light_Model_Ambient);  //环境光参数

    glEnable(GL_LIGHTING);   //开关:使用光
    glEnable(GL_LIGHT0);     //打开0#灯
    glEnable(GL_DEPTH_TEST); //打开深度测试
}

//Cube
void cube(GLfloat side)
{
    glBegin(GL_QUADS);
    glVertex3f(side, side, side);
    glVertex3f(side, side, -side);
    glVertex3f(side, -side, -side);
    glVertex3f(side, -side, side);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(-side, side, side);
    glVertex3f(-side, side, -side);
    glVertex3f(-side, -side, -side);
    glVertex3f(-side, -side, side);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(side, side, side);
    glVertex3f(-side, side, side);
    glVertex3f(-side, -side, side);
    glVertex3f(side, -side, side);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(side, side, -side);
    glVertex3f(-side, side, -side);
    glVertex3f(-side, -side, -side);
    glVertex3f(side, -side, -side);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(side, side, side);
    glVertex3f(side, side, -side);
    glVertex3f(-side, side, -side);
    glVertex3f(-side, side, side);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(side, -side, side);
    glVertex3f(side, -side, -side);
    glVertex3f(-side, -side, -side);
    glVertex3f(-side, -side, side);
    glEnd();
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glRotatef(xRot-xR, 1.0f, 0.0f, 0.0f);//绕x轴旋转  
    glRotatef(yRot-yR, 0.0f, 1.0f, 0.0f);//绕y轴旋转  

    glTranslated(-1,0,0);
    glutSolidTeapot(0.3);
    glTranslated(1,0,0);

    glTranslated(0,1,1);
    cube(0.2);
    glTranslated(0,-1,-1);

    /*
    glBegin(GL_QUADS);
    glVertex3f(0, 0, 10);
    glVertex3f(0, 0, 10);
    glVertex3f(20, 5, 10);
    glVertex3f(30, 40, -10);
    glEnd();
    */
    //cube();
    glTranslated(1,0,0);
    glutSolidSphere(0.2f,20,20);
    glTranslated(-1,0,0);
    //sphere();
    glFlush();
    //glSwapBuffers();
}

void SpecialKeys(int key, int x, int y){
    xR=xRot;
    yR=yRot;
    glTranslated(0,0,0);
    if (key == GLUT_KEY_UP)		xRot -= 1.0f;
    if (key == GLUT_KEY_DOWN)	xRot += 1.0f;
    if (key == GLUT_KEY_LEFT)	yRot -= 1.0f;
    if (key == GLUT_KEY_RIGHT)	yRot += 1.0f;
    glutPostRedisplay();
    // 刷新窗口 
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //设置投影参数
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //正交投影
    if (w <= h)
        glOrtho(-1.5, 1.5, -1.5*(GLfloat)h / (GLfloat)w, 1.5*(GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
        glOrtho(-1.5*(GLfloat)w / (GLfloat)h, 1.5*(GLfloat)w / (GLfloat)h, -1.5, 1.5, -10.0, 10.0);

    //设置模型参数--几何体参数
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("test");

    init();

    glutSpecialFunc(SpecialKeys);
    //glClearColor(0.0f,1.0f,1.0f,1.0f);//颜色
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}