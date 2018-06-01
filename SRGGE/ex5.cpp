#include "ex5.h"
#include "Helpers/mesh_importer.h"
#include "Helpers/mesh_io.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "time.h"


ex5::ex5(const QGLFormat &glf, QWidget *parent) : Viewer1(glf, parent)
{
 setFocusPolicy(Qt::StrongFocus);
}

void ex5::initializeGL()
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

void ex5::paintGL()
{
    std::cout<< "paintGl: preparing painting" <<std::endl;
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

    //    float size=2*mesh_->max_[0];

        std::cout << "paintGL: sono qui fuori" << std::endl;

        for(int i = 0; i<worldDimension; i++)
        {
            for(int j = 0; j<worldDimension; j++)
            {
                std::cout<<"cella matrice ["<< i << "]["<<j<<"]"<<std::endl;

                //Translation
                Eigen::Affine3f t(Eigen::Translation3f(Eigen::Vector3f((-worldDimension/2*tileDimension)+2*tileDimension*float(i),0,(-worldDimension/2*tileDimension)+2*tileDimension*float(j))));
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
    //                glBindVertexArray(vao);
    //                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex);


                //render ground
                if(world[i][j] == 0)
                {
                    glBindVertexArray(vaoG);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexG);
                    glDrawElements(GL_TRIANGLES,ground_faces.size(),GL_UNSIGNED_INT,0);
                }

                glBindVertexArray(0);


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

    worldDimension = 2;
    tileDimension = 2;
    std::vector<int> column ={0,0};
    world.push_back(column);
    world.push_back(column);
    std::cout << "initializeWorld: initialized the matrix of the world" << std::endl;
}

void ex5::createGround()
{
    ground_vertices.clear();
    ground_faces.clear();
    ground_normals.clear();


//    float pos_x = pos[0];
//    float pos_y = pos[1];
//    float pos_z = pos[2];

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
    ground_faces.push_back(0);
    ground_faces.push_back(1);
    ground_faces.push_back(2);
    //face 2
    ground_faces.push_back(1);
    ground_faces.push_back(2);
    ground_faces.push_back(3);


//    data_representation::ComputeVertexNormals(ground_vertices, ground_faces, &ground_normals);

    std::cout << "createGround: creata ground tile con vertici:" << ground_vertices.size() << " , faccie:" << ground_faces.size() << " , e normals:"<< ground_normals.size() << std::endl;


}

void ex5::createWall()
{





}

void ex5::initVertexBuffer()
{
    //ground
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

    std::cout << "initVertexBuffer: initilazied virtual array objects " << std::endl;
//    if (mesh_ == nullptr) return;


//    glGenVertexArrays(1,&vao);
//    glBindVertexArray(vao);


//    glGenBuffers(1,&vboVertex);
//    glGenBuffers(1,&vboNormal);
//    glGenBuffers(1,&vboIndex);



//    if(LODsimpleON){

//        //Vertex positions
//        glBindBuffer(GL_ARRAY_BUFFER,vboVertex);
//        glBufferData(GL_ARRAY_BUFFER,new_vertices.size()* sizeof(GLfloat),&new_vertices[0],GL_STATIC_DRAW);
//        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
//        glEnableVertexAttribArray(0);

//        //Vertex normals
//        glBindBuffer(GL_ARRAY_BUFFER,vboNormal);
//        glBufferData(GL_ARRAY_BUFFER,new_normals.size() * sizeof(float),&new_normals[0],GL_STATIC_DRAW);
//        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
//        glEnableVertexAttribArray(1);

//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER,new_faces.size()* sizeof(int),&new_faces[0],GL_STATIC_DRAW);


////        emit SetFaces(QString::number(new_faces.size() / 3));
////        emit SetVertices(QString::number(new_vertices.size() / 3));


//    }

//    else
//    {
//        //Vertex positions
//        glBindBuffer(GL_ARRAY_BUFFER,vboVertex);
//        glBufferData(GL_ARRAY_BUFFER,mesh_->vertices_.size()* sizeof(GLfloat),&mesh_->vertices_[0],GL_STATIC_DRAW);
//        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
//        glEnableVertexAttribArray(0);

//        //Vertex normals
//        glBindBuffer(GL_ARRAY_BUFFER,vboNormal);
//        glBufferData(GL_ARRAY_BUFFER,mesh_->normals_.size() * sizeof(float),&mesh_->normals_[0],GL_STATIC_DRAW);
//        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
//        glEnableVertexAttribArray(1);

//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER,mesh_->faces_.size()* sizeof(int),&mesh_->faces_[0],GL_STATIC_DRAW);


//    }


//    glBindVertexArray(0);


}

QGroupBox* ex5::controlPanel()
{
    // init group box
    QGroupBox *groupBox = Viewer1::controlPanel();
    groupBox->setStyleSheet(GroupBoxStyle);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);

    QPushButton *buttonMuseum = new QPushButton("Museum");

    //Unable LOD
    QButtonGroup *buttonGroup = new QButtonGroup;
    QRadioButton *OnNormal = new QRadioButton("Normal");
    QRadioButton *OnBasic = new QRadioButton("Vertex clustering");
    QRadioButton *OnOctree = new QRadioButton("Octree LOD");
    OnNormal -> setChecked(true);
    buttonGroup -> addButton(OnNormal);
    buttonGroup -> addButton(OnBasic);
    buttonGroup -> addButton(OnOctree);


    //Selection level
    QLabel *title_lod = new QLabel("<b>LOD</b>");
    QLabel *select_level = new QLabel("Select level:");

    QSpinBox *in_val = new QSpinBox(this);
    in_val->setMinimum(2);
    in_val->setMaximum(100);
    in_val->setSingleStep(1);
    in_val->setValue(2);

    QSlider *num_div = new QSlider(Qt::Horizontal);
    num_div -> setMinimum(2);
    num_div -> setMaximum(100);
    num_div -> setValue(2);
    num_div -> setTickInterval(1);
    num_div -> setTickPosition(QSlider::TicksBelow);

    //LOD counters
    QLabel *title_v = new QLabel("LOD Vertices:    ");
    QLabel *title_f = new QLabel("LOD Faces:    ");
    QLabel *v_count = new QLabel("0");
    QLabel *f_count = new QLabel("0");


    auto layout = dynamic_cast<QGridLayout*>(groupBox->layout());
    int row = layout->rowCount() + 1;

    connect(in_val, SIGNAL(valueChanged(int)),num_div,SLOT(setValue(int)) );
    connect(num_div,SIGNAL(valueChanged(int)),in_val,SLOT(setValue(int)) );

    connect(num_div, SIGNAL(valueChanged(int)),this,SLOT(setNumberlod(int)));
    connect(OnNormal,SIGNAL(toggled(bool)),this,SLOT(setOFF()));
    connect(OnBasic,SIGNAL(toggled(bool)),this,SLOT(setOnBasic()));
    connect(OnOctree,SIGNAL(toggled(bool)),this,SLOT(setOnOctree()));
    connect(this,SIGNAL(SetVertices(QString)),v_count,SLOT(setText(QString)));
    connect(this,SIGNAL(SetFaces(QString)),f_count,SLOT(setText(QString)));


    connect(buttonMuseum, SIGNAL(clicked()), this, SLOT(startMuseum()));

    //Layout
    row++;
    layout->addWidget(buttonMuseum,row,0);
    row++;
    layout->addWidget(line,row,0, 1 ,layout->columnCount());
    row++;
    layout->addWidget(OnNormal,row,0,1,2);
    row++;
    layout->addWidget(OnBasic,row,0);
    layout->addWidget(OnOctree,row,1);
    row++;

    row++;
    layout->addWidget(line,row,0, 1 ,layout->columnCount());
    row++;

    layout->addWidget(title_lod,row,0);
    row++;
    layout->addWidget(select_level,row,0);
    row++;
    layout->addWidget(num_div,row,0,1,2);
    row++;
    layout->addWidget(in_val,row,0,1,2);

    row++;
    layout->addWidget(title_v,row,0);
    layout->addWidget(v_count,row,1);
    row++;
    layout->addWidget(title_f,row,0);
    layout->addWidget(f_count,row,1);
    row++;


    return(groupBox);
}

void ex5::startMuseum()
{
    std::cout << "----> starting the application "<< std::endl;
    initializeWorld();
    startPrinting = true;
    paintGL();
}
