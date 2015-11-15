#include "renderer.h"
#include <QTextStream>
#include <QOpenGLBuffer>
#include <cmath>
#include<iostream>

using namespace std;

const float tet_verts[] = {
    1,1,1, 1,2,1, 2,1,1,
    1,1,1, 1,1,2, 1,2,1,
    1,1,1, 2,1,1, 1,1,2,
    2,1,1, 1,2,1, 1,1,2,
};

const float tet_normals[] = {
    0,0,-1, 0,0,-1, 0,0,-1,
    -1,0,0, -1,0,0, -1,0,0,
    0,-1,0, 0,-1,0, 0,-1,0,
    0.5773, 0.5773, 0.5773,
    0.5773, 0.5773, 0.5773,
    0.5773, 0.5773, 0.5773,
};

const float tet_uvs[] = {
    1, 1,  1, 0,  0, 0,
    1, 1,  1, 0,  0, 1,
    1, 1,  0, 0,  1, 0,
    1, 0,  0, 0,  1, 1,
};

const float tet_col[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
};

// constructor
//Renderer::Renderer(QWidget *parent)
//    : QOpenGLWidget(parent)
//{

//}
Renderer::Renderer(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_testRotation = 0;
    m_rotationTimer = new QTimer(this);
    connect(m_rotationTimer, SIGNAL(timeout()), this, SLOT(rotate_n_update()));
    m_rotationTimer->start(33);

    if (objModel.LoadModel("tris.obj"))
    {
        cout << "successfullly loaded" << endl;
    }

    for (int i=0; i<objModel.num_tris; i++)
    {
        tri triAt = objModel.tris[i];

        GLuint vertexIndex;
        glm::vec3 vertex;

        vertexIndex = triAt.index_xyz[0];
        vertex = objModel.m_vertices[vertexIndex-1];
        outVertices.push_back(vertex[0]);
        outVertices.push_back(vertex[1]);
        outVertices.push_back(vertex[2]);

        vertexIndex = triAt.index_xyz[1];
        vertex = objModel.m_vertices[vertexIndex-1];
        outVertices.push_back(vertex[0]);
        outVertices.push_back(vertex[1]);
        outVertices.push_back(vertex[2]);

        vertexIndex = triAt.index_xyz[2];
        vertex = objModel.m_vertices[vertexIndex-1];
        outVertices.push_back(vertex[0]);
        outVertices.push_back(vertex[1]);
        outVertices.push_back(vertex[2]);

        GLuint uvIndex;
        glm::vec2 uv;

        uvIndex = triAt.index_uv[0];
        uv = objModel.texs[uvIndex-1];
        outUvs.push_back(uv[0]);

        uvIndex = triAt.index_uv[1];
        uv = objModel.texs[uvIndex-1];
        outUvs.push_back(uv[1]);

        // shader supports per-vertex colour; add colour for each vertex
        float colourList [] = {
            1.0f, 0.0f, 0.0f, // red
            0.0f, 1.0f, 0.0f, // green
            0.0f, 0.0f, 1.0f // blue
        };
        outColours.insert(outColours.end(), colourList, colourList + 3*1*3); // 9 items in array

        // shader supports per-vertex normals; add normal for each vertex - use same normal for each vertex (eg. face normal)
        float normalList [] = { 0.0f, 0.0f, 1.0f }; // facing viewer
        outNormals.insert(outNormals.end(), normalList, normalList + 3); // 3 coordinates per vertex
        outNormals.insert(outNormals.end(), normalList, normalList + 3); // 3 coordinates per vertex
        outNormals.insert(outNormals.end(), normalList, normalList + 3); // 3 coordinates per vertex
    }

    mouse_x = 0;
    mouse_left = false;
    mouse_middle = false;
    mouse_right = false;
    key_shift = false;
    key_ctrl = false;

    view_matrix_translation.translate(-(objModel.max_x+objModel.min_x)/2,
                          -(objModel.max_y+objModel.min_y)/2,
                          -80.0f);

    cout << "num_tris: " << objModel.num_tris << endl;
    cout << "num_xyz: " << objModel.num_xyz << endl;
    cout << "vertices: " << objModel.m_vertices.size() << endl;
    cout << "texture coordinates size: " << objModel.texs.size() << endl;
    cout << "glm::vec2 size= " << sizeof(glm::vec2) << endl;
    cout << "glm::vec3 size= " << sizeof(glm::vec3) << endl;
}
// and add
void Renderer::rotate_n_update()
{
    m_testRotation += 1;
    this->update();
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

    glEnable(GL_DEPTH_TEST);

    // sets the background clour
    glClearColor(0.7f, 0.7f, 1.0f, 1.0f);

    // links to and compiles the shaders, used for drawing simple objects
    m_program = new QOpenGLShaderProgram(this);;
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "textured-phong.vs.glsl");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "textured-phong.fs.glsl");

    m_program->link();

    m_posAttr = m_program->attributeLocation("position_attr");
    m_colAttr = m_program->attributeLocation("colour_attr");
    m_norAttr = m_program->attributeLocation("normal_attr");
    m_uvAttr = m_program->attributeLocation("texcoords_attr");
    m_PMatrixUniform = m_program->uniformLocation("proj_matrix");
    m_VMatrixUniform = m_program->uniformLocation("view_matrix");
    m_MMatrixUniform = m_program->uniformLocation("model_matrix");
    m_TextureUniform = m_program->uniformLocation("texObject");

    m_programID = m_program->programId();

    // and add this
    glGenTextures(1, &m_testTexture); // Generate a texture handle
    glActiveTexture(GL_TEXTURE0); // Make sure we're using texture unit 0
    glBindTexture(GL_TEXTURE_2D, m_testTexture); // bind the texture handle
    glUniform1i(m_TextureUniform, 0); // Give it the 0'th texture unit

    QImage image; // Load the image
    image.load("texture.png");
    image = image.convertToFormat(QImage::Format_RGBA8888); // Convert it to a usable format

    // Write it to the GPU
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 image.width(),
                 image.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_INT_8_8_8_8_REV,
                 (const GLvoid *)image.bits());
    // Set the filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    generateExampleTriangle();
}

// called by the Qt GUI system, to allow OpenGL drawing commands
void Renderer::paintGL()
{
    // Clear the screen buffers

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the current shader program

    glUseProgram(m_programID);

    // and add
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_testTexture);
    glUniform1i(m_TextureUniform, 0); // Give it the 0'th texture unit

    // Modify the current projection matrix so that we move the
    // camera away from the origin.  We'll draw the model at the
    // origin, and we need to back up to see it or scale vertices
    // so it fits in the viewing area.

//    glUniformMatrix4fv(m_VMatrixUniform, 1, false, view_matrix.data());

    QMatrix4x4 model_matrix = objModel.getTransformMatrix();

    model_matrix.rotate(m_testRotation, 0,1,0);

    glUniformMatrix4fv(m_MMatrixUniform, 1, false, model_matrix.data());


    QMatrix4x4 vm = view_matrix_translation * view_matrix_rotation;
    glUniformMatrix4fv(m_VMatrixUniform, 1, false, vm.data());

    // draw the model
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, &outVertices[0]);  // vertices
    glVertexAttribPointer(m_norAttr, 3, GL_FLOAT, GL_FALSE, 0, &outNormals[0]); // no normals yet
    glVertexAttribPointer(m_uvAttr, 2, GL_FLOAT, GL_FALSE, 0, &outUvs[0]); // textures, we have these
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, &outColours[0]); // colors

    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_norAttr);
    glEnableVertexAttribArray(m_uvAttr);
    glEnableVertexAttribArray(m_colAttr);

    // draw triangles
    glDrawArrays(GL_TRIANGLES, 0, outVertices.size()/3);

    glDisableVertexAttribArray(m_colAttr);
    glDisableVertexAttribArray(m_uvAttr);
    glDisableVertexAttribArray(m_norAttr);
    glDisableVertexAttribArray(m_posAttr);


    //demo triangel
    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, &tet_verts[0]);
    glVertexAttribPointer(m_norAttr, 3, GL_FLOAT, GL_FALSE, 0, &tet_normals[0]);
    glVertexAttribPointer(m_uvAttr, 2, GL_FLOAT, GL_FALSE, 0, &tet_uvs[0]);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, &tet_col[0]);

    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_norAttr);
    glEnableVertexAttribArray(m_uvAttr);
    glEnableVertexAttribArray(m_colAttr);

    // draw triangles
    glDrawArrays(GL_TRIANGLES, 0, 12);

    glDisableVertexAttribArray(m_colAttr);
    glDisableVertexAttribArray(m_uvAttr);
    glDisableVertexAttribArray(m_norAttr);
    glDisableVertexAttribArray(m_posAttr);

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

void Renderer::generateBoard()
{
    boardVertices.clear();
    boardColours.clear();

//    float vectList [] =
}

// override mouse press event
void Renderer::mousePressEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
    if (event->button() == 1)
    {
        mouse_left = true;
    }
    if (event->button() == 4)
    {
        mouse_middle = true;
    }
    if (event->button() == 2)
    {
        mouse_right = true;
    }
    mouse_x = 0;
}

// override mouse release event
void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Button " << event->button() << " pressed.\n";
    if (event->button() == 1)
    {
        mouse_left = false;
    }
    else if (event->button() == 4)
    {
        mouse_middle = false;
    }
    else if (event->button() == 2)
    {
        mouse_right = false;
    }
}

// override mouse move event
void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    QTextStream cout(stdout);
    cout << "Stub: Motion at " << event->x() << ", " << event->y() << ".\n";

    if (mouse_x == 0 || mouse_y == 0)
    {
        mouse_x = event->x();
        mouse_y = event->y();
        return;
    }
    magnitude_x =  (event->x() - mouse_x); //TODO: could make these vary by screensize..
    magnitude_y =  (event->y() - mouse_y);
    cout << magnitude_x << "," << magnitude_y << endl;

    if (!key_shift && !key_ctrl)
    {
        if (mouse_left)
        {
            objModel.translation_x += magnitude_x;
            objModel.translation_y -= magnitude_y;
        }
        if (mouse_middle)
        {
            objModel.translation_z += magnitude_x;
        }
        if (mouse_right)
        {
            // TODO: trackball rotation
        }
    }

    if (key_shift)
    {
        if (mouse_left)
        {
            view_matrix_translation.translate(magnitude_x, -magnitude_y, 0);
        }
        if (mouse_middle)
        {
            view_matrix_translation.translate(0, 0, magnitude_x);
        }
        if (mouse_right)
        {
            // TODO: rotate up direction of camera..
            // is this just rotating the view matrix?
            view_matrix_rotation.rotate(magnitude_x, 0, 0, 1);
        }
    }

    if (key_ctrl)
    {
        if (mouse_left)
        {
        }
        if (mouse_middle)
        {
        }
    }

    mouse_x = event->x();
    mouse_y = event->y();
    update();
}

void Renderer::setShiftStatus(bool status)
{
    key_shift = status;
}

void Renderer::setCtrlStatus(bool status)
{
    key_ctrl = status;
}
