#include "ex4.h"
#include "Helpers/mesh_importer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "time.h"


ex4::ex4(const QGLFormat &glf, QWidget *parent) : Viewer1(glf, parent)
{
    // init vars
    setFocusPolicy(Qt::StrongFocus);


}

void ex4::initializeGL()
{

    // initialize GL function resolution for current context
    initializeGLFunctions();

    gShader = new Shader(QString("/home/al/Documents/Un/srgge/prj/SRGGE/shaders/try.vert"), QString("/home/al/Documents/Un/srgge/prj/SRGGE/shaders/try.frag"));
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

        std::cout << "paintGL:mesh painting" << std::endl;

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

                //RENDER MODEL
                glBindVertexArray(vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex);
                glDrawElements(GL_TRIANGLES,mesh_->faces_.size(),GL_UNSIGNED_INT,0);
                glBindVertexArray(0);

                //Reset ModelView
                model=camera_.SetModel();

                }
            }
        glUseProgram(0);
    }

    if(startPrinting)
    {
        std::cout << "paintGL: painting world" << std::endl;

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
                Eigen::Affine3f t(Eigen::Translation3f(Eigen::Vector3f(-tileDimension*float(i),0,-tileDimension*float(j))));
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

                //render ground
                if(world[i][j] == 0)
                {
                    glBindVertexArray(vaoG);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexG);
                    glDrawElements(GL_TRIANGLES,ground_faces.size(),GL_UNSIGNED_INT,0);
                    glBindVertexArray(0);

                    //render wall
                    if((i != (worldDimension-1)) && (world[i][j] != world[i+1][j]))
                    {
                        std::cout << "sono qui"<<std::endl;
                        glBindVertexArray(vaoW);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexW);
                        glDrawElements(GL_TRIANGLES,wall_faces.size(),GL_UNSIGNED_INT,0);
                        glBindVertexArray(0);

                    }
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
void ex4::initializeWorld()
{

    worldDimension = 3;
    tileDimension = 2;
    std::vector<int> column1 ={0,0,0};
    std::vector<int> column2 ={0,1,0};
    std::vector<int> column3 ={0,0,1};

    world.push_back(column1);
    world.push_back(column2);
    world.push_back(column3);

    std::cout << "initializeWorld: initialized the matrix of the world" << std::endl;
}

void ex4::createGround()
{
    ground_vertices.clear();
    ground_faces.clear();
    ground_normals.clear();


    float dimension = tileDimension;

    //0
    ground_vertices.push_back(0);
    ground_vertices.push_back(-1);
    ground_vertices.push_back(0);
    //1
    ground_vertices.push_back(dimension);
    ground_vertices.push_back(-1);
    ground_vertices.push_back(0);
    //2
    ground_vertices.push_back(0);
    ground_vertices.push_back(-1);
    ground_vertices.push_back(dimension);
    //3
    ground_vertices.push_back(dimension);
    ground_vertices.push_back(-1);
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

void ex4::createWall()
{
    wall_vertices.clear();
    wall_faces.clear();
    wall_normals.clear();


    float dimension = tileDimension;
    float off = tileDimension/10;
    //cube 1
    //0
    ground_vertices.push_back(off);
    ground_vertices.push_back(-1);
    ground_vertices.push_back(0);
    //1
    ground_vertices.push_back(off);
    ground_vertices.push_back(-1);
    ground_vertices.push_back(dimension);
    //2
    ground_vertices.push_back(off);
    ground_vertices.push_back(-1 + dimension);
    ground_vertices.push_back(0);
    //3
    ground_vertices.push_back(off);
    ground_vertices.push_back(-1 + dimension);
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
        ground_normals.push_back(1);
        ground_normals.push_back(0);
        ground_normals.push_back(0);

    }

//    data_representation::ComputeVertexNormals(ground_vertices, ground_faces, &ground_normals);

    std::cout << "createGround: creata ground tile con vertici:" << ground_vertices.size() << " , faccie:" << ground_faces.size() << " , e normals:"<< ground_normals.size() << std::endl;
}



void ex4::initVertexBuffer()
{
    std::cout << "initVertexBuffer: " << std::endl;

    t_Timer.start();

    if (mesh_ != nullptr)
    {
        glGenVertexArrays(1,&vao);
        glBindVertexArray(vao);


        glGenBuffers(1,&vboVertex);
        glGenBuffers(1,&vboNormal);
        glGenBuffers(1,&vboIndex);

        //Vertex positions
        glBindBuffer(GL_ARRAY_BUFFER,vboVertex);
        glBufferData(GL_ARRAY_BUFFER,new_vertices.size()* sizeof(GLfloat),&new_vertices[0],GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
        glEnableVertexAttribArray(0);

        //Vertex normals
        glBindBuffer(GL_ARRAY_BUFFER,vboNormal);
        glBufferData(GL_ARRAY_BUFFER,new_normals.size() * sizeof(float),&new_normals[0],GL_STATIC_DRAW);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,new_faces.size()* sizeof(int),&new_faces[0],GL_STATIC_DRAW);

    }
    if(startPrinting)
    {
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


void ex4::startMuseum()
{
    std::cout << "----> starting the application "<< std::endl;
    initializeWorld();
    startPrinting = true;
    initVertexBuffer();
    paintGL();
}

void ex4::setNumberCopies(int N)
{
    copies=N;
    paintGL();
}



