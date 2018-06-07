

#include "ex5.h"
#include "Helpers/mesh_importer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "time.h"

ex5::ex5(const QGLFormat &glf, QWidget *parent) : Viewer1(glf, parent)
{
    // init vars
    setFocusPolicy(Qt::StrongFocus);


}

void ex5::initializeGL()
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


void ex5::paintGL()
{


    // Render to our framebuffer
    glViewport(0,0,width_,height_); // Render on the whole framebuffer
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(startPrinting)
    {
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

        for(int i = 0; i<worldDimension; i++)
        {
            for(int j = 0; j<worldDimension; j++)
            {
//                std::cout<<"cella matrice ["<< i << "]["<<j<<"]"<<std::endl;

                //Translation
//                Eigen::Affine3f t(Eigen::Translation3f(Eigen::Vector3f((-worldDimension/2*tileDimension)+2*tileDimension*float(i),0,(-worldDimension/2*tileDimension)+2*tileDimension*float(j))));
                Eigen::Affine3f t(Eigen::Translation3f(Eigen::Vector3f(-tileDimension*float(i),-1,-tileDimension*float(j))));
                Eigen::Matrix4f m = t.matrix();
                model = model * m;

                //Rotation matrix of 90 degree on y axes to create z-aligned wall :)
                Eigen::Vector3f w = Eigen::Vector3f(0,1,0); // rotation axis
                Eigen::Vector3f c = Eigen::Vector3f(0,0,0); // center of rotation
                Eigen::Affine3f A = Eigen::Translation3f(c) * Eigen::AngleAxisf(0.5*M_PI, w) * Eigen::Translation3f(-c);

                GLuint projection_location = glGetUniformLocation(gShaderID,"u_projection");
                glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());
                GLuint view_location = glGetUniformLocation(gShaderID,"u_view");
                glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());
                GLuint model_location = glGetUniformLocation(gShaderID,"u_model");
                glUniformMatrix4fv(model_location, 1, GL_FALSE, model.data());
                GLuint normal_matrix_location = glGetUniformLocation(gShaderID,"u_normal_matrix");
                glUniformMatrix3fv(normal_matrix_location, 1, GL_FALSE, normal.data());

                GLuint color_location = glGetUniformLocation(gShaderID,"in_color");
                glUniform3f( color_location, 1.0,1.0,1.0);


                //render ground
                if(world[i][j] ==  0)
                    glUniform3f( color_location, 1.0,1.0,1.0);
                else if(world[i][j] == 1)
                    glUniform3f( color_location, 0.3,0.0,0.0);
                else if(world[i][j] == 2)
                    glUniform3f( color_location, 0.7,0.0,0.0);
                else
                    glUniform3f( color_location, 0.3,0.3,0.0);

                glBindVertexArray(vaoG);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexG);
                glDrawElements(GL_TRIANGLES,ground_faces.size(),GL_UNSIGNED_INT,0);
                glBindVertexArray(0);


                //render x-aligned wall
                if((i != (worldDimension-1)) && (world[i][j] != world[i+1][j]) && (world[i][j] != 2) && (world[i+1][j] != 2))
                {
                    glUniform3f( color_location, 0.0,0.0,0.4);

                    glBindVertexArray(vaoW);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexW);
                    glDrawElements(GL_TRIANGLES,wall_faces.size(),GL_UNSIGNED_INT,0);
                    glBindVertexArray(0);

                }
                //render z-aligned walls
                if((j != (worldDimension-1)) && (world[i][j] != world[i][j+1]) && (world[i][j] != 2) && (world[i][j+1] != 2))
                {
                    //uploading the model matrix with 90 degree rotation
                    Eigen::Matrix4f rotatedModel = model*A.matrix();
                    GLuint rotated_model_location = glGetUniformLocation(gShaderID,"u_model");
                    glUniformMatrix4fv(rotated_model_location, 1, GL_FALSE, rotatedModel.data());

                    glUniform3f( color_location, 0.0,0.0,0.4);

                    glBindVertexArray(vaoW);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexW);
                    glDrawElements(GL_TRIANGLES,wall_faces.size(),GL_UNSIGNED_INT,0);
                    glBindVertexArray(0);

                }

                if(world[i][j]== 3 && (mesh1 != nullptr))
                {


                    //serve un'altra translazione per mettere i conigli al centro della tile!!

                    Eigen::Affine3f t2(Eigen::Translation3f(Eigen::Vector3f((tileDimension/2),0,(tileDimension/2))));
                    Eigen::Matrix4f translatonModel = t2.matrix();

                    float sf = scalingFactor;
                    Eigen::Matrix4f S;
                    S << sf, 0 , 0 , 0,
                         0 , sf, 0 , 0,
                         0 , 0 , sf, 0,
                         0 , 0 , 0 , 1;

                    Eigen::Matrix4f scaledModel = model*translatonModel*S;
                    GLuint scaled_model_location = glGetUniformLocation(gShaderID,"u_model");
                    glUniformMatrix4fv(scaled_model_location, 1, GL_FALSE, scaledModel.data());


                    //RENDER MODEL
                    glBindVertexArray(vao);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex);
                    glDrawElements(GL_TRIANGLES,mesh1->faces_.size(),GL_UNSIGNED_INT,0);
                    glBindVertexArray(0);

                }


                //Reset ModelView
                model=camera_.SetModel();
            }
        }

        glUseProgram(0);
    }
    //Set Framerate

    frames++;
    final_time=time(NULL);
    if(final_time-initial_time>0)
    {
        emit SetFramerate(QString::number(frames/(final_time-initial_time)));
        frames=0;
        initial_time=final_time;
    }



}


//read the input file and generate the matrix of the world
void ex5::initializeWorld()
{

    worldDimension = 6;
    tileDimension = 3;
    std::vector<int> column1 = {3,3,0,0,0,0};
    std::vector<int> column2 = {3,1,1,0,0,0};
    std::vector<int> column3 = {0,1,1,0,0,0};
    std::vector<int> column4 = {0,2,1,1,3,0};
    std::vector<int> column5 = {0,0,0,1,3,0};
    std::vector<int> column6 = {0,0,0,0,0,0};

    world.push_back(column1);
    world.push_back(column2);
    world.push_back(column3);
    world.push_back(column4);
    world.push_back(column5);
    world.push_back(column6);

    std::cout << "initializeWorld: initialized the matrix of the world" << std::endl;
}

//create a basic ground tile
void ex5::createGround()
{
    ground_vertices.clear();
    ground_faces.clear();
    ground_normals.clear();


    float dimension = tileDimension;

    //0
    ground_vertices.push_back(0);
    ground_vertices.push_back(0);
    ground_vertices.push_back(0);
    //1
    ground_vertices.push_back(dimension);
    ground_vertices.push_back(0);
    ground_vertices.push_back(0);
    //2
    ground_vertices.push_back(0);
    ground_vertices.push_back(0);
    ground_vertices.push_back(dimension);
    //3
    ground_vertices.push_back(dimension);
    ground_vertices.push_back(0);
    ground_vertices.push_back(dimension);

    //face 1
    ground_faces.push_back(1);
    ground_faces.push_back(0);
    ground_faces.push_back(2);
    //face 2
    ground_faces.push_back(1);
    ground_faces.push_back(2);
    ground_faces.push_back(3);

    for(int i = 0; i<4; i++)
    {
        ground_normals.push_back(0);
        ground_normals.push_back(1);
        ground_normals.push_back(0);

    }

//    data_representation::ComputeVertexNormals(ground_vertices, ground_faces, &ground_normals);

    std::cout << "createGround: creata ground tile con vertici:" << ground_vertices.size() << " , faccie:" << ground_faces.size() << " , e normals:"<< ground_normals.size() << std::endl;
}

//create a basic x-aligned wall tile
//it is possible to create also the z-aligned one
void ex5::createWall()
{
    wall_vertices.clear();
    wall_faces.clear();
    wall_normals.clear();

    //dimension of ground
    float dimension = tileDimension;
    //half-thickness of the wall
    float off = tileDimension/20;

    //NOTE: adding off to dimension in order to cover the corner

    std::cout << "createWall: dimension:"<<dimension<<" and off:"<<off<<std::endl;
    //cube 1
    //0
    wall_vertices.push_back(off);
    wall_vertices.push_back(0);
    wall_vertices.push_back(0);
    //1
    wall_vertices.push_back(off);
    wall_vertices.push_back(0);
    wall_vertices.push_back(dimension + off);
    //2
    wall_vertices.push_back(off);
    wall_vertices.push_back(dimension + off);
    wall_vertices.push_back(0);
    //3
    wall_vertices.push_back(off);
    wall_vertices.push_back(dimension + off);
    wall_vertices.push_back(dimension + off);

    //face 0
    wall_faces.push_back(1);
    wall_faces.push_back(0);
    wall_faces.push_back(2);

    wall_faces.push_back(1);
    wall_faces.push_back(2);
    wall_faces.push_back(3);

    //cube 2
    //4
    wall_vertices.push_back(-off);
    wall_vertices.push_back(0);
    wall_vertices.push_back(0);
    //5
    wall_vertices.push_back(-off);
    wall_vertices.push_back(0);
    wall_vertices.push_back(dimension + off);
    //6
    wall_vertices.push_back(-off);
    wall_vertices.push_back(dimension + off);
    wall_vertices.push_back(0);
    //7
    wall_vertices.push_back(-off);
    wall_vertices.push_back(dimension + off);
    wall_vertices.push_back(dimension + off);

    //face 1
    wall_faces.push_back(4);
    wall_faces.push_back(5);
    wall_faces.push_back(6);

    wall_faces.push_back(5);
    wall_faces.push_back(7);
    wall_faces.push_back(6);

    for(int i = 0; i<4; i++)
    {
        wall_normals.push_back(1);
        wall_normals.push_back(0);
        wall_normals.push_back(0);

    }
    for(int i = 0; i<4; i++)
    {
        wall_normals.push_back(1);
        wall_normals.push_back(0);
        wall_normals.push_back(0);

    }

    //face 2
    wall_faces.push_back(0);
    wall_faces.push_back(5);
    wall_faces.push_back(4);

    wall_faces.push_back(0);
    wall_faces.push_back(1);
    wall_faces.push_back(5);

    //face 3
    wall_faces.push_back(1);
    wall_faces.push_back(7);
    wall_faces.push_back(5);

    wall_faces.push_back(1);
    wall_faces.push_back(3);
    wall_faces.push_back(7);

    //face 4
    wall_faces.push_back(3);
    wall_faces.push_back(6);
    wall_faces.push_back(7);

    wall_faces.push_back(3);
    wall_faces.push_back(2);
    wall_faces.push_back(6);

    //face 5
    wall_faces.push_back(0);
    wall_faces.push_back(6);
    wall_faces.push_back(2);

    wall_faces.push_back(0);
    wall_faces.push_back(4);
    wall_faces.push_back(6);


//    data_representation::ComputeVertexNormals(wall_vertices, wall_faces, &wall_normals);

    std::cout << "createwall: creata wall tile con vertici:" << wall_vertices.size() << " , faccie:" << wall_faces.size() << " , e normals:"<< wall_normals.size() << std::endl;
}



void ex5::initVertexBuffer()
{
    std::cout << "initVertexBuffer: " << std::endl;

    t_Timer.start();
    if(startPrinting)
    {

        if (mesh1 != nullptr)
        {
            std::cout << "initVertexBuffer:mesh initilization" << std::endl;

            glGenVertexArrays(1,&vao);
            glBindVertexArray(vao);

            glGenBuffers(1,&vboVertex);
            glGenBuffers(1,&vboNormal);
            glGenBuffers(1,&vboIndex);

            //Vertex positions
            glBindBuffer(GL_ARRAY_BUFFER,vboVertex);
            glBufferData(GL_ARRAY_BUFFER,mesh1->vertices_.size()* sizeof(GLfloat),&mesh1->vertices_[0],GL_STATIC_DRAW);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
            glEnableVertexAttribArray(0);

            //Vertex normals
            glBindBuffer(GL_ARRAY_BUFFER,vboNormal);
            glBufferData(GL_ARRAY_BUFFER,mesh1->normals_.size() * sizeof(float),&mesh1->normals_[0],GL_STATIC_DRAW);
            glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,mesh1->faces_.size()* sizeof(int),&mesh1->faces_[0],GL_STATIC_DRAW);

            glBindVertexArray(0);


        }

        std::cout << "initVertexBuffer: store ground" << std::endl;

        createGround();

        glGenVertexArrays(1,&vaoG);
        glBindVertexArray(vaoG);

        glGenBuffers(1,&vboVertexG);
        glGenBuffers(1,&vboNormalG);
        glGenBuffers(1,&vboIndexG);


        //Vertex positions
        glBindBuffer(GL_ARRAY_BUFFER,vboVertexG);
        glBufferData(GL_ARRAY_BUFFER,ground_vertices.size()* sizeof(GLfloat),&ground_vertices[0],GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
        glEnableVertexAttribArray(0);

        //Vertex normals
        glBindBuffer(GL_ARRAY_BUFFER,vboNormalG);
        glBufferData(GL_ARRAY_BUFFER,ground_normals.size() * sizeof(float),&ground_normals[0],GL_STATIC_DRAW);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(1);

        //vertex index
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexG);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,ground_faces.size()* sizeof(int),&ground_faces[0],GL_STATIC_DRAW);

        glBindVertexArray(0);

        std::cout << "initVertexBuffer: store wall" << std::endl;

        createWall();

        glGenVertexArrays(1,&vaoW);
        glBindVertexArray(vaoW);

        glGenBuffers(1,&vboVertexW);
        glGenBuffers(1,&vboNormalW);
        glGenBuffers(1,&vboIndexW);


        //Vertex positions
        glBindBuffer(GL_ARRAY_BUFFER,vboVertexW);
        glBufferData(GL_ARRAY_BUFFER,wall_vertices.size()* sizeof(GLfloat),&wall_vertices[0],GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
        glEnableVertexAttribArray(0);

        //Vertex normals
        glBindBuffer(GL_ARRAY_BUFFER,vboNormalW);
        glBufferData(GL_ARRAY_BUFFER,wall_normals.size() * sizeof(float),&wall_normals[0],GL_STATIC_DRAW);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(1);

        //vertex index
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,wall_faces.size()* sizeof(int),&wall_faces[0],GL_STATIC_DRAW);

        std::cout << "initVertexBuffer: initilazied virtual array objects " << std::endl;

        glBindVertexArray(0);

    }


}





QGroupBox* ex5::controlPanel()
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


    QPushButton *initializeMuseum = new QPushButton("PUSH");

    auto layout = dynamic_cast<QGridLayout*>(groupBox->layout());
    int row = layout->rowCount() + 1;

    connect(num_copies, SIGNAL(valueChanged(int)),this,SLOT(setNumberCopies(int)));
    connect(this,SIGNAL(SetFramerate(QString)),print_fps,SLOT(setText(QString)));
    connect(initializeMuseum, SIGNAL(clicked()), this, SLOT(startMuseum()));


    row++;
    layout->addWidget(initializeMuseum,0,1);
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


void ex5::startMuseum()
{
    std::cout << "----> starting the application "<< std::endl;
    initializeWorld();
    LoadModel(QString("/home/al/Downloads/bunny.ply"));
    startPrinting = true;
    initVertexBuffer();
    paintGL();
}

void ex5::setNumberCopies(int N)
{
    copies=N;
    paintGL();
}


bool ex5::LoadModel(QString filename) {
  std::string file = filename.toUtf8().constData();
  uint pos = file.find_last_of(".");
  std::string type = file.substr(pos + 1);

  mesh1 = std::unique_ptr<data_representation::TriangleMesh>(new data_representation::TriangleMesh);

  bool res = false;
  if (type.compare("ply") == 0) {
    res = data_representation::ReadFromPly(file, mesh1.get());
  }

  if (res) {

        std::cout << "LoadModel : creata mesh con vertici:" << mesh1->vertices_.size() << " faccie:" << mesh1->faces_.size() << " normals:" <<mesh1->normals_.size() <<std::endl;

        float x_dim_mesh = mesh1->max_[0] - mesh1->min_[0];
        float z_dim_mesh = mesh1->max_[0] - mesh1->min_[0];
        std::cout << "LoadModel : dimensione x:" << x_dim_mesh << " e y:"<< z_dim_mesh  <<std::endl;
        float maxDim;
        if(x_dim_mesh < z_dim_mesh)
            maxDim = z_dim_mesh;
        else
            maxDim = x_dim_mesh;
        scalingFactor = (tileDimension - (tileDimension/3))/maxDim;


//      mesh1.reset(mesh1.release());
//      camera_.UpdateModel(mesh1->min_, mesh1->max_);

//      m_facesCount->setText(QString(std::to_string(m_hw->mesh1->faces_.size() / 3).c_str()));
//      m_vertexCount->setText(QString(std::to_string(m_hw->mesh1->vertices_.size() / 3).c_str()));

//      camera_.UpdateModel(mesh1->min_, mesh1->max_);

//      initVertexBuffer();
      return true;
  }

  return false;
}
