#include "ex4.h"
#include "Helpers/mesh_importer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "time.h"



//TO DO:
// try differet levels for the meshes
//implement the selection



ex4::ex4(const QGLFormat &glf, QWidget *parent) : ex2(glf, parent)
{
    // init vars
    setFocusPolicy(Qt::StrongFocus);


}

void ex4::initializeGL()
{
    // initialize GL function resolution for current context
    initializeGLFunctions();

    gShader = new Shader(QString("/home/al/Documents/Un/srgge/prj/SRGGE/shaders/try.vert"), QString("/home/al/Documents/Un/srgge/prj/SRGGE/shaders/try_color.frag"));
    gShader->m_program.bindAttributeLocation("vert", ATTRIB_VERTEX);
    gShader->m_program.bindAttributeLocation("normal" , ATTRIB_NORMAL);
    gShaderID = gShader -> getId();

    glGenVertexArrays = (_glGenVertexArrays) QGLWidget::context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray) QGLWidget::context()->getProcAddress("glBindVertexArray");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    // init state variables
    glClearColor(1.0, 1.0, 1.0, 1.0);	// set background color
    glColor3f   (1.0, 1.0, 0.0);		// set foreground color

    std::cout << "initilizeGL:" <<std::endl;

}


void ex4::paintGL()
{


    // Render to our framebuffer
    glViewport(0,0,width_,height_); // Render on the whole framebuffer
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mesh_ != nullptr) {


        camera_.SetViewport();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(gShaderID);

        Eigen::Matrix4f projection = camera_.SetProjection();
        Eigen::Matrix4f view = camera_.SetView();
        Eigen::Matrix4f model = camera_.SetModel();

        Eigen::Matrix4f t = view * model;
        Eigen::Matrix3f normal;
        for (int i = 0; i < 3; ++i)
          for (int j = 0; j < 3; ++j) normal(i, j) = t(i, j);

        normal = normal.inverse().transpose();

        float size=2*mesh_->max_[0];


        for(int i =0; i<copies; i++){

            for(int j=0; j<copies; j++){

                //Translation
                Eigen::Affine3f t(Eigen::Translation3f(Eigen::Vector3f((-copies/2*size)+2*size*float(i),0,(-copies/2*size)+2*size*float(j))));
                Eigen::Matrix4f m = t.matrix();
                model = model * m;

                GLuint projection_location = glGetUniformLocation(gShaderID,"u_projection");
                glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());
                GLuint view_location = glGetUniformLocation(gShaderID,"u_view");
                glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());
                GLuint model_location = glGetUniformLocation(gShaderID,"u_model");
                glUniformMatrix4fv(model_location, 1, GL_FALSE, model.data());
                GLuint normal_matrix_location = glGetUniformLocation(gShaderID,"u_normal_matrix");
                glUniformMatrix3fv(normal_matrix_location, 1, GL_FALSE, normal.data());

                GLuint color_location = glGetUniformLocation(gShaderID,"in_color");


                //RENDER MODEL
//                glUniform3f( color_location, 1.0,0.0,1.0);
//                glBindVertexArray(vao);
//                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex);
//                glDrawElements(GL_TRIANGLES,mesh_->faces_.size(),GL_UNSIGNED_INT,0);
//                glBindVertexArray(0);

//                glUniform3f( color_location, 0.0,1.0,1.0);
//                glBindVertexArray(vao0);
//                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex0);
//                glDrawElements(GL_TRIANGLES,faces[0].size(),GL_UNSIGNED_INT,0);
//                glBindVertexArray(0);

//                glUniform3f( color_location, 0.0,0.0,1.0);
//                glBindVertexArray(vao1);
//                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex1);
//                glDrawElements(GL_TRIANGLES,faces[1].size(),GL_UNSIGNED_INT,0);
//                glBindVertexArray(0);

//                glUniform3f( color_location, 1.0,0.0,0.0);
//                glBindVertexArray(vao2);
//                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex2);
//                glDrawElements(GL_TRIANGLES,faces[2].size(),GL_UNSIGNED_INT,0);
//                glBindVertexArray(0);

                glUniform3f( color_location, 0.0,1.0,0.0);
                glBindVertexArray(vao3);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex3);
                glDrawElements(GL_TRIANGLES,faces[3].size(),GL_UNSIGNED_INT,0);
                glBindVertexArray(0);





                //Reset ModelView
                model=camera_.SetModel();

                }

            }


        glUseProgram(0);


    }

//    Set Framerate
    frames++;
    final_time=time(NULL);
    if(final_time-initial_time>0)
    {
        emit SetFramerate(QString::number(frames/(final_time-initial_time)));
        frames=0;
        initial_time=final_time;
    }



}



//generate vertices, normals and indices of all the levels of details
void ex4::generateMeshes()
{
    vertices.clear();
    normals.clear();
    faces.clear();

    level = 8;
    vertexClustering();
    vertices.push_back(new_vertices);
    normals.push_back(new_normals);
    faces.push_back(new_faces);
    level = 16;
    vertexClustering();
    vertices.push_back(new_vertices);
    normals.push_back(new_normals);
    faces.push_back(new_faces);
    level = 32;
    vertexClustering();
    vertices.push_back(new_vertices);
    normals.push_back(new_normals);
    faces.push_back(new_faces);
    level = 64;
    vertexClustering();
    vertices.push_back(new_vertices);
    normals.push_back(new_normals);
    faces.push_back(new_faces);




}


void ex4::initVertexBuffer()
{
    std::cout << "initVertexBuffer: " << std::endl;

    t_Timer.start();

    if (mesh_ != nullptr)
    {
        std::cout << "initVertexBuffer:mesh initilization" << std::endl;





        //origianl mesh
        /*-----------------------*/
        glGenVertexArrays(1,&vao);
        glBindVertexArray(vao);

        glGenBuffers(1,&vboVertex);
        glGenBuffers(1,&vboNormal);
        glGenBuffers(1,&vboIndex);

        //Vertex positions
        glBindBuffer(GL_ARRAY_BUFFER,vboVertex);
        glBufferData(GL_ARRAY_BUFFER,mesh_->vertices_.size()* sizeof(GLfloat),&mesh_->vertices_[0],GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
        glEnableVertexAttribArray(0);

        //Vertex normals
        glBindBuffer(GL_ARRAY_BUFFER,vboNormal);
        glBufferData(GL_ARRAY_BUFFER,mesh_->normals_.size() * sizeof(float),&mesh_->normals_[0],GL_STATIC_DRAW);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,mesh_->faces_.size()* sizeof(int),&mesh_->faces_[0],GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        glBindVertexArray(0);


        generateMeshes();


        /*-------------------*/
        glGenVertexArrays(1,&vao0);
        glBindVertexArray(vao0);

        glGenBuffers(1,&vboVertex0);
        glGenBuffers(1,&vboNormal0);
        glGenBuffers(1,&vboIndex0);

        //Vertex positions
        glBindBuffer(GL_ARRAY_BUFFER,vboVertex0);
        glBufferData(GL_ARRAY_BUFFER,vertices[0].size()* sizeof(GLfloat),&vertices[0][0],GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
        glEnableVertexAttribArray(0);

        //Vertex normals
        glBindBuffer(GL_ARRAY_BUFFER,vboNormal0);
        glBufferData(GL_ARRAY_BUFFER,normals[0].size() * sizeof(float),&normals[0][0],GL_STATIC_DRAW);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex0);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,faces[0].size()* sizeof(int),&faces[0][0],GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        glBindVertexArray(0);

        /*-------------------*/
        glGenVertexArrays(1,&vao1);
        glBindVertexArray(vao1);

        glGenBuffers(1,&vboVertex1);
        glGenBuffers(1,&vboNormal1);
        glGenBuffers(1,&vboIndex1);

        //Vertex positions
        glBindBuffer(GL_ARRAY_BUFFER,vboVertex1);
        glBufferData(GL_ARRAY_BUFFER,vertices[1].size()* sizeof(GLfloat),&vertices[1][0],GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
        glEnableVertexAttribArray(0);

        //Vertex normals
        glBindBuffer(GL_ARRAY_BUFFER,vboNormal1);
        glBufferData(GL_ARRAY_BUFFER,normals[1].size() * sizeof(float),&normals[1][0],GL_STATIC_DRAW);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex1);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,faces[1].size()* sizeof(int),&faces[1][0],GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        glBindVertexArray(0);

        /*-------------------*/
        glGenVertexArrays(1,&vao2);
        glBindVertexArray(vao2);

        glGenBuffers(1,&vboVertex2);
        glGenBuffers(1,&vboNormal2);
        glGenBuffers(1,&vboIndex2);

        //Vertex positions
        glBindBuffer(GL_ARRAY_BUFFER,vboVertex2);
        glBufferData(GL_ARRAY_BUFFER,vertices[2].size()* sizeof(GLfloat),&vertices[2][0],GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
        glEnableVertexAttribArray(0);

        //Vertex normals
        glBindBuffer(GL_ARRAY_BUFFER,vboNormal2);
        glBufferData(GL_ARRAY_BUFFER,normals[2].size() * sizeof(float),&normals[2][0],GL_STATIC_DRAW);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex2);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,faces[2].size()* sizeof(int),&faces[2][0],GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        glBindVertexArray(0);

        /*-------------------*/
        glGenVertexArrays(1,&vao3);
        glBindVertexArray(vao3);

        glGenBuffers(1,&vboVertex3);
        glGenBuffers(1,&vboNormal3);
        glGenBuffers(1,&vboIndex3);

        //Vertex positions
        glBindBuffer(GL_ARRAY_BUFFER,vboVertex3);
        glBufferData(GL_ARRAY_BUFFER,vertices[3].size()* sizeof(GLfloat),&vertices[3][0],GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
        glEnableVertexAttribArray(0);

        //Vertex normals
        glBindBuffer(GL_ARRAY_BUFFER,vboNormal3);
        glBufferData(GL_ARRAY_BUFFER,normals[3].size() * sizeof(float),&normals[3][0],GL_STATIC_DRAW);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex3);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,faces[3].size()* sizeof(int),&faces[3][0],GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        glBindVertexArray(0);


    }

}





QGroupBox* ex4::controlPanel()
{
    // init group box
    QGroupBox *groupBox = Viewer1::controlPanel();
    groupBox->setStyleSheet(GroupBoxStyle);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    QFrame* line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);

    QLabel *title_fps = new QLabel("Framerate:");
    print_fps = new QLabel(QString::number(0));

    //Selection number of copies
    QLabel *title_copies = new QLabel("Number of copies:");
    QSpinBox *num_copies = new QSpinBox();
    num_copies -> setMinimum(1);


    auto layout = dynamic_cast<QGridLayout*>(groupBox->layout());
    int row = layout->rowCount() + 1;

    connect(num_copies, SIGNAL(valueChanged(int)),this,SLOT(setNumberCopies(int)));
    connect(this,SIGNAL(SetFramerate(QString)),print_fps,SLOT(setText(QString)));

    row++;
    layout->addWidget(line,row,0, 1 ,layout->columnCount());
    row++;

    layout->addWidget(title_fps,row,0);
    layout->addWidget(print_fps,row,1);

    row++;
    layout->addWidget(line1,row,0, 1 ,layout->columnCount());
    row++;

    layout->addWidget(title_copies,row,0);
    layout->addWidget(num_copies,row,1);

    row++;
    layout->addWidget(line2,row,0, 1 ,layout->columnCount());
    row++;


    return(groupBox);

}



void ex4::setNumberCopies(int N)
{
    copies=N;
    paintGL();
}

