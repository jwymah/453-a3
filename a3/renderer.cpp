#include "renderer.h"
#include <QTextStream>
#include <QOpenGLBuffer>
#include <cmath>

// constructor
Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

// constructor
Renderer::~Renderer()
{

}

// called once by Qt GUI system, to allow initialization for OpenGL requirements
void Renderer::initializeGL()
{
    // Qt support for inline GL function calls
	initializeOpenGLFunctions();

    // sets the background clour
    glClearColor(0.7f, 0.7f, 1.0f, 1.0f);

    // links to and compiles the shaders, used for drawing simple objects
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "per-fragment-phong.vs.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "per-fragment-phong.fs.glsl");
    m_program->link();
    m_posAttr = m_program->attributeLocation("position_attr");
    m_colAttr = m_program->attributeLocation("colour_attr");
    m_norAttr = m_program->attributeLocation("normal_attr");
    m_PMatrixUniform = m_program->uniformLocation("proj_matrix");
    m_VMatrixUniform = m_program->uniformLocation("view_matrix");
    m_MMatrixUniform = m_program->uniformLocation("model_matrix");
    m_programID = m_program->programId();

}

// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
    // Clear the screen buffers

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the current shader program

    glUseProgram(m_programID);

    // Modify the current projection matrix so that we move the
    // camera away from the origin.  We'll draw the model at the
    // origin, and we need to back up to see it or scale vertices
    // so it fits in the viewing area.

    QMatrix4x4 view_matrix;
    view_matrix.translate(0.0f, 0.0f, -5.0f);
    glUniformMatrix4fv(m_VMatrixUniform, 1, false, view_matrix.data());

    // Not implemented: set up lighting (if necessary)

    QMatrix4x4 model_matrix;

    // Not implemented: scale, rotate and translate the scene

    model_matrix.translate(0.0f, 0.0f, 0.0f);
    glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());

    // Not implemented: actually draw our character
    // Here's some test code that draws a simple triangle at the center

    generateExampleTriangle();

    // example triangle
    if (triVertices.size() > 0)
    {
        // pass in the list of vertices and their associated colours
        // 3 coordinates per vertex, or per colour
        glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, &triVertices[0]);
        glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, &triColours[0]);
        glVertexAttribPointer(m_norAttr, 3, GL_FLOAT, GL_FALSE, 0, &triNormals[0]);

        glEnableVertexAttribArray(m_posAttr);
        glEnableVertexAttribArray(m_colAttr);
        glEnableVertexAttribArray(m_norAttr);

        // draw triangles
        glDrawArrays(GL_TRIANGLES, 0, triVertices.size()/3); // 3 coordinates per vertex

        glDisableVertexAttribArray(m_norAttr);
        glDisableVertexAttribArray(m_colAttr);
        glDisableVertexAttribArray(m_posAttr);
    }

    // deactivate the program
    m_program->release();

}

// called by the Qt GUI system, to allow OpenGL to respond to widget resizing
void Renderer::resizeGL(int w, int h)
{
    // width and height are better variables to use
    Q_UNUSED(w); Q_UNUSED(h);

    // update viewing projections
    glUseProgram(m_programID);

    // Set up perspective projection, using current size and aspect
    // ratio of display
    QMatrix4x4 projection_matrix;
    projection_matrix.perspective(40.0f, (GLfloat)width() / (GLfloat)height(),
                                  0.1f, 1000.0f);
    glUniformMatrix4fv(m_PMatrixUniform, 1, false, projection_matrix.data());

    glViewport(0, 0, width(), height());

}

// computes the vertices and corresponding colours-per-vertex for a triangle
// Note: the magic numbers in the vector insert commands should be better documented
void Renderer::generateExampleTriangle()
{
    // make sure array lists are clear to start with
    triVertices.clear();
    triColours.clear();

    // add vertices to triangle list
    float vectList [] = {
        -0.8, -0.75, 0.0,  // one main central triangle
        0.0, 0.75, 0.0,
        0.8, -0.75, 0.0 };
    triVertices.insert(triVertices.end(), vectList, vectList + 3*1*3); // 9 items in array

    // shader supports per-vertex colour; add colour for each vertex
    float colourList [] = {
        1.0f, 0.0f, 0.0f, // red
        0.0f, 1.0f, 0.0f, // green
        0.0f, 0.0f, 1.0f // blue
    };
    triColours.insert(triColours.end(), colourList, colourList + 3*1*3); // 9 items in array

    // shader supports per-vertex normals; add normal for each vertex - use same normal for each vertex (eg. face normal)
    float normalList [] = { 0.0f, 0.0f, 1.0f }; // facing viewer
    for (int v = 0; v < 4 * 3; v++)
    {
        triNormals.insert(triNormals.end(), normalList, normalList + 3); // 3 coordinates per vertex
    }

}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
}

// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
}

// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";
}
