/*
 * CPSC 453 - Introduction to Computer Graphics
 * Assignment 1
 *
 * Renderer - OpenGL widget for drawing scene
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "objModel.h"
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMouseEvent>
#include <QTimer>

using namespace std;

class Renderer : public QOpenGLWidget, protected QOpenGLFunctions
{

    // informs the qmake that a Qt moc_* file will need to be generated
    Q_OBJECT

public:
    // constructor
    Renderer(QWidget *parent = 0);

    // destructor
    virtual ~Renderer();

protected:

    // override fundamental drawing functions

    // Called when OpenGL is first initialized
    void initializeGL();

    // Called when the window is resized
    void resizeGL(int w, int h);

    // Called when the window needs to be redrawn
    void paintGL();

    // override mouse event functions

    // Called when a mouse button is pressed
    virtual void mousePressEvent(QMouseEvent * event);

    // Called when a mouse button is released
    virtual void mouseReleaseEvent(QMouseEvent * event);

    // Called when the mouse moves
    virtual void mouseMoveEvent(QMouseEvent * event);

private:
    GLuint m_uvAttr;
    GLuint m_TextureUniform;
    GLuint m_testTexture;
    QTimer *m_rotationTimer;
    float m_testRotation;

    // member variables for shader manipulation
    GLuint m_programID;
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_norAttr;
    GLuint m_MMatrixUniform; // model matrix
    GLuint m_VMatrixUniform; // view matrix
    GLuint m_PMatrixUniform; // projection matrix

    QOpenGLShaderProgram *m_program;

    // for storing triangle vertices and colours
    vector<GLfloat> triVertices;
    vector<GLfloat> triColours;
    vector<GLfloat> triNormals;

    // board
    vector<GLfloat> boardVertices;
    vector<GLfloat> boardColours;
    vector<GLfloat> boardNormals;

    // object model
    ObjModel objModel;
    vector<GLfloat> outVertices;
    vector<GLfloat> outUvs;
    vector<GLfloat> normals;

    // helper function for loading shaders
    GLuint loadShader(GLenum type, const char *source);

    // helper function for drawing bordering triangles
    void generateExampleTriangle();

    // draw board
    void generateBoard();

private slots:
    void rotate_n_update();

};

#endif // RENDERER_H
