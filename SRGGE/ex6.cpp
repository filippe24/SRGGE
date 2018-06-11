#include "ex6.h"
#include "Helpers/mesh_importer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "time.h"

#include "Helpers/visibility.h"

ex6::ex6(const QGLFormat &glf, QWidget *parent) : Viewer1(glf, parent)
{
    // init vars
    setFocusPolicy(Qt::StrongFocus);


}

void ex6::initializeGL()
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


void ex6::paintGL()
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

        for(uint i = 0; i<world.size(); i++)
        {
            for(uint j = 0; j<world[0].size(); j++)
            {

                //Translation
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


                //render ground interior=1 exterior=0
                if(world[i][j] ==  0)
                    glUniform3f( color_location, 0.2,0.6,0.0);
                else if(world[i][j] == 1)
                    glUniform3f( color_location, 0.3,0.0,0.0);
                else if(world[i][j] == 2)
                    glUniform3f( color_location, 0.7,0.0,0.0);
                else
                    glUniform3f( color_location, 0.8,0.0,0.0);

                //render ground ground=0 wall=1
//                if(!isInterior(i,j))
//                    glUniform3f( color_location, 1.0,1.0,1.0);
//                else if(isInterior(i,j))
//                    glUniform3f( color_location, 0.3,0.0,0.0);
//                else
//                    glUniform3f( color_location, 0.3,0.3,0.0);
//                if(world[i][j] == 2)
//                    glUniform3f( color_location, 0.7,0.0,0.0);


                glBindVertexArray(vaoG);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexG);
                glDrawElements(GL_TRIANGLES,ground_faces.size(),GL_UNSIGNED_INT,0);
                glBindVertexArray(0);

                //test-----
//                isInterior(i,j);

                //render x-aligned wall
                if((i != (world.size()-1)) && ((world[i][j] == 1 && world[i+1][j] == 0) || (world[i][j] == 0 && world[i+1][j] == 1)) && (world[i][j] != 2) && (world[i+1][j] != 2))
                {
                    glUniform3f( color_location, 0.7,0.7,0.7);

                    glBindVertexArray(vaoW);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexW);
                    glDrawElements(GL_TRIANGLES,wall_faces.size(),GL_UNSIGNED_INT,0);
                    glBindVertexArray(0);

                }
                //render z-aligned walls
                if((j != (world[0].size()-1)) && ((world[i][j] == 1 && world[i][j+1] == 0) || (world[i][j] == 0 && world[i][j+1] == 1))  && (world[i][j] != 2) && (world[i][j+1] != 2))
                {
                    //uploading the model matrix with 90 degree rotation
                    Eigen::Matrix4f rotatedModel = model*A.matrix();
                    GLuint rotated_model_location = glGetUniformLocation(gShaderID,"u_model");
                    glUniformMatrix4fv(rotated_model_location, 1, GL_FALSE, rotatedModel.data());

                    glUniform3f( color_location, 0.7,0.7,0.7);

                    glBindVertexArray(vaoW);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexW);
                    glDrawElements(GL_TRIANGLES,wall_faces.size(),GL_UNSIGNED_INT,0);
                    glBindVertexArray(0);

                }

                //render the uploaded models
                if(world[i][j] > 2)
                {
                    //index of the model and buffers
                    int m = world[i][j]-3;

                    if(meshes[m] != nullptr)
                   {

                        //serve un'altra translazione per mettere i conigli al centro della tile!!
                        float onTheGround = -(meshes[m]->min_[1]);
                        float sf = scalingFactors[m];

                        Eigen::Affine3f t2(Eigen::Translation3f(Eigen::Vector3f((tileDimension/2), sf*onTheGround , (tileDimension/2))));
                        Eigen::Matrix4f translatonModel = t2.matrix();

                        Eigen::Matrix4f S;
                        S << sf, 0 , 0 , 0,
                             0 , sf, 0 , 0,
                             0 , 0 , sf, 0,
                             0 , 0 , 0 , 1;

                        Eigen::Matrix4f scaledModel = model*translatonModel*S;
                        GLuint scaled_model_location = glGetUniformLocation(gShaderID,"u_model");
                        glUniformMatrix4fv(scaled_model_location, 1, GL_FALSE, scaledModel.data());

                        glUniform3f( color_location, 1.0,1.0,1.0);

                        //RENDER MODEL
                        glBindVertexArray(vaoMs[m]);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexMs[m]);
                        glDrawElements(GL_TRIANGLES,meshes[m]->faces_.size(),GL_UNSIGNED_INT,0);
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


//check if a ground is interior or not ( only if the maps are ground=0, walls= 1)
bool ex6::isInterior( int i, int j)
{

    bool up_bool = false;
    bool dw_bool = false;
    bool rx_bool = false;
    bool lx_bool = false;

    int up = i;
    int dw = i;
    int rx = j;
    int lx = j;

    //UP
    if(i>0){
        while(!up_bool && up>0){
            up_bool = (world[up-1][j]==1);
            up-=1;
        }
    }

    //DOWN
    if(i<((int) world.size())){
        while(!dw_bool && dw<((int) world.size()-1)){
            dw_bool = (world[dw+1][j]==1);
            dw+=1;
        }
    }

    //LEFT
    if(j>0){
        while(!lx_bool && lx>0){
            lx_bool=(world[i][lx-1]==1);
            lx-=1;
        }
    }

    //RIGHT
    if(j<((int) world[0].size())){
        while(!rx_bool && rx<((int) world[0].size()-1)){
            rx_bool=(world[i][rx+1]==1);
            rx+=1;
        }
    }

    bool returned = up_bool && dw_bool && lx_bool && rx_bool;

    if(returned)
        std::cout << "la cella i:"<<i<<", j;"<<j<<" e' interna" <<std::endl;
    else
        std::cout << "la cella i:"<<i<<", j;"<<j<<" e' esterna" <<std::endl;

    return (returned);

}


//read the input file and generate the matrix of the world and uploads the models
void ex6::initializeWorld()
{

    world.clear();
    meshes.clear();

    //message before load model
    QMessageBox loadMsgBox;
    loadMsgBox.setText("Choose a map file");
    loadMsgBox.exec();


    //load map
    QString filename;
    filename = QFileDialog::getOpenFileName(this, tr("Load map"), "/home/al/Documents/Un/srgge/prj/SRGGE/models",
                                              tr("TXT Files ( *.txt )"));
    if (!filename.isNull())
    {

        std::string file = filename.toUtf8().constData();
        uint pos = file.find_last_of(".");
        std::string type = file.substr(pos + 1);

         if(!openFile(file))
         {
              QMessageBox::warning(this, tr("Error"),
                              tr("The file could not be opened, initialize the default world"));

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


         }
         else
         {
             tileDimension = 3;
         }
    }


    //load models
    for(int i=0; i<numberOfModels; i++)
    {
        //message before loading the models
        QMessageBox modelMsgBox;
        QString modelMsg = QString("Load the ") + QString::number(i+1) + QString( " model, out of ") + QString::number(numberOfModels);
        modelMsgBox.setText(modelMsg);
        modelMsgBox.exec();

        meshes.push_back(std::unique_ptr<data_representation::TriangleMesh>(new data_representation::TriangleMesh));
        QString modelfilename;
        modelfilename = QFileDialog::getOpenFileName(this, tr("Load model"), "/home/al/Documents/Un/srgge/prj/SRGGE/models",
                                                  tr("TXT Files ( *.ply )"));
        if (!modelfilename.isNull())
        {
            if(!LoadModel(modelfilename, i))
            {
                 QMessageBox::warning(this, tr("Error"),
                                 tr("The file could not be opened, initialize the default world"));
            }
        }

    }
    museumIsInitialized=true;

    std::cout << "initializeWorld: initialized the matrix of the world" << std::endl;
}



//open the file with the matrix of the world
bool ex6::openFile(const std::string &filename)
{
    world.clear();
    numberOfModels = 0;

    //maximum number saved in file ---> to define the number of models to upload
    int maxVal = 0;


    std::ifstream fin;
    fin.open(filename.c_str());
    if (!fin.is_open() || !fin.good()) return false;

    std::string line;
    for(int i=0; std::getline(fin,line);i++){

        std::stringstream ss;
        ss << line;
        std::vector<int> temp;

        world.push_back(temp);

        int val;
        while(ss >> val){
            world[i].push_back(val);
            if(val > maxVal)
                maxVal = val;
        }
    }

    fin.close();


    if(maxVal > 2)
    {
        numberOfModels = maxVal - 2;
        std::cout << " number of models in the file " << filename << " is:" << numberOfModels << std::endl;
    }


    return true;

}

//create a basic ground tile
void ex6::createGround()
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
void ex6::createWall()
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



void ex6::initVertexBuffer()
{
    std::cout << "initVertexBuffer: " << std::endl;


    //clear the buffer of the models
    vaoMs.clear();
    vboVertexMs.clear();
    vboNormalMs.clear();
    vboIndexMs.clear();

    t_Timer.start();
    if(startPrinting)
    {

        for(int m=0; m<numberOfModels; m++)
        {

           if(meshes[m] != nullptr)
           {
                std::cout << "initVertexBuffer: inizializzazione della mesh "<< m << std::endl;
                std::cout << "initVertexBuffer: mesh vertices :" << meshes[m]->vertices_.size() << "  mesh faces: "<<meshes[m]->faces_.size() << std::endl;


                GLuint tempVao = 0;
                GLuint tempVboVertex;
                GLuint tempVboNormal;
                GLuint tempVboIndex;

                std::cout << "initVertexBuffer:mesh initilization" << std::endl;
                std::cout << "initVertexBuffer: vao prima" <<tempVao << std::endl;

                glGenVertexArrays(1,&tempVao);
                vaoMs.push_back(tempVao);
                glBindVertexArray(vaoMs[m]);

                std::cout << "initVertexBuffer: vao dopo:" <<tempVao << " e quello salvato nel vettore:" <<vaoMs[m] << "  a posizione m=" <<m<< std::endl;


                glGenBuffers(1,&tempVboVertex);
                glGenBuffers(1,&tempVboNormal);
                glGenBuffers(1,&tempVboIndex);

                vboVertexMs.push_back(tempVboVertex);
                vboNormalMs.push_back(tempVboNormal);
                vboIndexMs.push_back(tempVboIndex);

                //Vertex positions
                glBindBuffer(GL_ARRAY_BUFFER,vboVertexMs[m]);
                glBufferData(GL_ARRAY_BUFFER,meshes[m]->vertices_.size()* sizeof(GLfloat),&meshes[m]->vertices_[0],GL_STATIC_DRAW);
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0 ,0);
                glEnableVertexAttribArray(0);

                //Vertex normals
                glBindBuffer(GL_ARRAY_BUFFER,vboNormalMs[m]);
                glBufferData(GL_ARRAY_BUFFER,meshes[m]->normals_.size() * sizeof(float),&meshes[m]->normals_[0],GL_STATIC_DRAW);
                glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
                glEnableVertexAttribArray(1);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndexMs[m]);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,meshes[m]->faces_.size()* sizeof(int),&meshes[m]->faces_[0],GL_STATIC_DRAW);


                glBindBuffer(GL_ARRAY_BUFFER,0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
                glBindVertexArray(0);
            }

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





QGroupBox* ex6::controlPanel()
{
    QGroupBox *groupBox = new QGroupBox("Menu");
    groupBox->setStyleSheet(GroupBoxStyle);
    QGridLayout *layout = new QGridLayout;
    groupBox->setLayout(layout);
    // init group box
//    QGroupBox *groupBox = Viewer1::controlPanel();
//    groupBox->setStyleSheet(GroupBoxStyle);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    QFrame* line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);
    QFrame* line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);

    QLabel *title_fps = new QLabel("Framerate:");
    print_fps = new QLabel(QString::number(0));

    //Selection number of copies
//    QLabel *title_copies = new QLabel("Number of copies:");
//    QSpinBox *num_copies = new QSpinBox();
//    num_copies -> setMinimum(1);


    QPushButton *initializeMuseum = new QPushButton("INIT MUSEUM");
    QPushButton *compVisibility = new QPushButton("COMPUTE VISIBILITY");
    QPushButton *startMuseum = new QPushButton("START MUSEUM");

//    auto layout = dynamic_cast<QGridLayout*>(groupBox->layout());
    int row = layout->rowCount() + 1;

//    connect(num_copies, SIGNAL(valueChanged(int)),this,SLOT(setNumberCopies(int)));
    connect(this,SIGNAL(SetFramerate(QString)),print_fps,SLOT(setText(QString)));
    connect(initializeMuseum, SIGNAL(clicked()), this, SLOT(initializeWorld()));

    connect(compVisibility, SIGNAL(clicked()), this, SLOT(computeVisibility()));

    connect(startMuseum, SIGNAL(clicked()), this, SLOT(startMuseum()));


    row++;
    layout->addWidget(initializeMuseum,0,0);
    layout->addWidget(compVisibility,0,1);
    row++;
    layout->addWidget(line,row,0, 1 ,layout->columnCount());
    row++;
    layout->addWidget(startMuseum,row,0);
    row++;
    layout->addWidget(title_fps,row,0);
    layout->addWidget(print_fps,row,1);


    return(groupBox);

}


void ex6::computeVisibility()
{
    visibility *vs;
    vs = new visibility();

    if(museumIsInitialized)
    {

        vs->computeVisibility(world, 100);

    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(" you should initialize the museum first");
        msgBox.exec();


    }
}


void ex6::startMuseum()
{
    if(museumIsInitialized)
    {
        std::cout << "----> starting the application "<< std::endl;
    //    initializeWorld();
    //    LoadModel(QString("/home/al/Downloads/bunny.ply"));
        startPrinting = true;
        camera_.activateMuseumCamera();
        initVertexBuffer();
        paintGL();
        std::cout << "startMuseum: afterPaintGL" << std::endl;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(" you should initialize the museum first");
        msgBox.exec();

    }
}

void ex6::setNumberCopies(int N)
{
    copies=N;
    paintGL();
}


bool ex6::LoadModel(QString filename, int i)
{
  std::string file = filename.toUtf8().constData();
  uint pos = file.find_last_of(".");
  std::string type = file.substr(pos + 1);

//    meshes.push_back(std::unique_ptr<data_representation::TriangleMesh>(new data_representation::TriangleMesh));


  bool res = false;
  if (type.compare("ply") == 0) {
    res = data_representation::ReadFromPly(file, meshes[i].get());
  }

  if (res) {

        std::cout << "LoadModel : creata mesh con vertici:" << meshes[i]->vertices_.size() << " faccie:" << meshes[i]->faces_.size() << " normals:" <<meshes[i]->normals_.size() <<std::endl;

        float x_dim_mesh = meshes[i]->max_[0] - meshes[i]->min_[0];
        float z_dim_mesh = meshes[i]->max_[2] - meshes[i]->min_[2];
        std::cout << "LoadModel : dimensione x:" << x_dim_mesh << " e z:"<< z_dim_mesh  <<std::endl;
        float maxDim;
        if(x_dim_mesh < z_dim_mesh)
            maxDim = z_dim_mesh;
        else
            maxDim = x_dim_mesh;
        scalingFactors.push_back((tileDimension - (tileDimension/3))/maxDim);

      return true;
  }

  return false;
}
