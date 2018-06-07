#include "ex2.h"

#include "Helpers/mesh_importer.h"
#include "Helpers/mesh_io.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "time.h"
#include <cmath>



namespace data_representation {

namespace {
void ComputeVertexNormals(const std::vector<float> &vertices,
                          const std::vector<int> &faces,
                          std::vector<float> *normals) {
  const int kFaces = faces.size();
  std::vector<float> face_normals(kFaces, 0);

  for (int i = 0; i < kFaces; i += 3) {
    Eigen::Vector3d v1(vertices[faces[i] * 3], vertices[faces[i] * 3 + 1],
                       vertices[faces[i] * 3 + 2]);
    Eigen::Vector3d v2(vertices[faces[i + 1] * 3],
                       vertices[faces[i + 1] * 3 + 1],
                       vertices[faces[i + 1] * 3 + 2]);
    Eigen::Vector3d v3(vertices[faces[i + 2] * 3],
                       vertices[faces[i + 2] * 3 + 1],
                       vertices[faces[i + 2] * 3 + 2]);
    Eigen::Vector3d v1v2 = v2 - v1;
    Eigen::Vector3d v1v3 = v3 - v1;
    Eigen::Vector3d normal = v1v2.cross(v1v3);

    if (normal.norm() < 0.0000000001) {
      normal = Eigen::Vector3d(0.0, 0.0, 0.0);
    } else {
      normal.normalize();
    }

    for (int j = 0; j < 3; ++j) face_normals[i + j] = normal[j];
  }

  const int kVertices = vertices.size();
  normals->resize(kVertices, 0);
  for (int i = 0; i < kFaces; i += 3) {
    for (int j = 0; j < 3; ++j) {
      int idx = faces[i + j];
      Eigen::Vector3d v1(vertices[faces[i + j] * 3],
                         vertices[faces[i + j] * 3 + 1],
                         vertices[faces[i + j] * 3 + 2]);
      Eigen::Vector3d v2(vertices[faces[i + (j + 1) % 3] * 3],
                         vertices[faces[i + (j + 1) % 3] * 3 + 1],
                         vertices[faces[i + (j + 1) % 3] * 3 + 2]);
      Eigen::Vector3d v3(vertices[faces[i + (j + 2) % 3] * 3],
                         vertices[faces[i + (j + 2) % 3] * 3 + 1],
                         vertices[faces[i + (j + 2) % 3] * 3 + 2]);

      Eigen::Vector3d v1v2 = v2 - v1;
      Eigen::Vector3d v1v3 = v3 - v1;
      double angle = acos(v1v2.dot(v1v3) / (v1v2.norm() * v1v3.norm()));

      if (angle == angle) {
        for (int k = 0; k < 3; ++k) {
          (*normals)[idx * 3 + k] += face_normals[i + k] * angle;
        }
      }
    }
  }

  const int kNormals = normals->size();
  for (int i = 0; i < kNormals; i += 3) {
    Eigen::Vector3d normal((*normals)[i], (*normals)[i + 1], (*normals)[i + 2]);
    if (normal.norm() > 0.00001) {
      normal.normalize();
    } else {
      normal = Eigen::Vector3d(0, 0, 0);
    }

    for (int j = 0; j < 3; ++j) (*normals)[i + j] = normal[j];
  }
}

  }  // namespace

}  // namespace data_representation


ex2::ex2(const QGLFormat &glf, QWidget *parent) : ex1(glf, parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

void ex2::initializeGL()
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

}

void ex2::paintGL()
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


                //RENDER MODEL
                glBindVertexArray(vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vboIndex);

                if(LODsimpleON)
                {
                    glDrawElements(GL_TRIANGLES,new_faces.size(),GL_UNSIGNED_INT,0);
                }
                else
                {
                    glDrawElements(GL_TRIANGLES,mesh_->faces_.size(),GL_UNSIGNED_INT,0);
                }


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

void ex2::vertexClustering()
{


    new_vertices.clear();
    new_faces.clear();
    new_normals.clear();


    std::cout<< "vertex Clustering " << std::endl;
    std::cout<< "selected level " << level << std::endl;

    //computing grid

    float xDimension = ((mesh_->max_[0]) - (mesh_->min_[0]));
    float yDimension = ((mesh_->max_[1]) - (mesh_->min_[1]));
    float zDimension = ((mesh_->max_[2]) - (mesh_->min_[2]));

    std::cout << " x dimension " << xDimension  << " y dimension " << yDimension << " z dimension " << zDimension << std::endl;

    int numVertici = (int) mesh_->vertices_.size()/3;

    int numCells = level*level*level;


    std::vector<std::vector<int>> griglia(numCells);

    float step_x = xDimension/level;
    float step_y = yDimension/level;
    float step_z = zDimension/level;


    //selecting the cell for each vertex
    for (int i = 0; i < numVertici; i++ )
    {
        int ver_i = i*3;

        float x_d = mesh_->vertices_[ver_i+0] - (mesh_->min_[0]);
        float y_d = mesh_->vertices_[ver_i+1] - (mesh_->min_[1]);
        float z_d = mesh_->vertices_[ver_i+2] - (mesh_->min_[2]);



        int x = static_cast<int>(x_d/step_x);
        int y = static_cast<int>(y_d/step_y);
        int z = static_cast<int>(z_d/step_z);




        if(x == level) x-=1;
        if(y == level) y-=1;
        if(z == level) z-=1;

        int grid_i = x + y*level + z*level*level;

        griglia[grid_i].push_back(i);

    }


    std::vector<float> vert_cell(numVertici);

    //computing the mean
    int cella = 0;
    for (int i = 0; i < numCells ; i++ )
    {

        int num_ver_cell = griglia[i].size();

        float x_sum = 0;
        float y_sum = 0;
        float z_sum = 0;

        if(num_ver_cell != 0)
        {
            for(int j=0; j<num_ver_cell; j++ )
            {
                x_sum = x_sum + mesh_->vertices_[griglia[i][j]*3+0];
                y_sum = y_sum + mesh_->vertices_[griglia[i][j]*3+1];
                z_sum = z_sum + mesh_->vertices_[griglia[i][j]*3+2];
                vert_cell[griglia[i][j]] = cella;

            }

            x_sum = x_sum/num_ver_cell;
            y_sum = y_sum/num_ver_cell;
            z_sum = z_sum/num_ver_cell;

            new_vertices.push_back(x_sum);
            new_vertices.push_back(y_sum);
            new_vertices.push_back(z_sum);

            cella = cella + 1;

        }
    }


    //computing the faces
    int numFaces = (int) mesh_->faces_.size()/3;
    for (int i=0; i< numFaces; i++)
    {
        //every triangle of the face
//        int f_id = i*3;
        int ver_a = mesh_->faces_[3*i+0];
        int ver_b = mesh_->faces_[3*i+1];
        int ver_c = mesh_->faces_[3*i+2];

        //get the id of the cell of the vertices
        int id_a = vert_cell[ver_a];
        int id_b = vert_cell[ver_b];
        int id_c = vert_cell[ver_c];

        if(!(id_a == id_b || id_a==id_c || id_b == id_c))
        {
            new_faces.push_back(id_a);
            new_faces.push_back(id_b);
            new_faces.push_back(id_c);

        }
    }

    data_representation::ComputeVertexNormals(new_vertices,new_faces,&new_normals);

}

void ex2::octreeVertexClustering()
{

    new_vertices.clear();
    new_faces.clear();
    new_normals.clear();

    if(createdOctree)
    {
        std::cout<< "octree vertex Clustering " << std::endl;
        std::cout<< "selected level " << level << std::endl;

        int numVertici = (int) mesh_->vertices_.size()/3;

        //calibrate the selected level on the tree and the Qslider
        int lev;
        lev = static_cast<int>(level * (float)(oc->depthOfTheTree/100.0) );
        if(lev == 0) lev=1;

        //select the level in the octree and extract the informations
        oc->setLevel(lev);
        std::vector<int> newGenVertices = oc->getNewVertices();
        std::vector<std::vector<int>> cellVertices = oc->getOldVerticesIndices();
        std::vector<std::vector<float>> means = oc->getMeans();

        std::vector<int> vertexCell(numVertici);

        //getting the mean already computed in the tree
        for(uint c = 0; c<cellVertices.size(); c++)
        {
            for(uint v = 0; v< cellVertices[c].size(); v++)
            {
                int index = cellVertices[c][v];
                vertexCell[index] = c;

            }
        }
        for(uint i= 0; i<newGenVertices.size(); i++)
        {
            new_vertices.push_back(means[i][0]);
            new_vertices.push_back(means[i][1]);
            new_vertices.push_back(means[i][2]);
        }

        //**OR using the indices is possible to compute the mean again**
        //computing the mean
//        int cellNum = 0;
//        for (uint c = 0; c < newGenVertices.size() ; c++ )
//        {
//            int numVer = cellVertices[c].size();
//            float x_sum = 0;
//            float y_sum = 0;
//            float z_sum = 0;

//            if(numVer != 0)
//            {
//                for(int v = 0; v<numVer; v ++ )
//                {
//                    x_sum = x_sum + mesh_->vertices_[cellVertices[c][v]*3+0];
//                    y_sum = y_sum + mesh_->vertices_[cellVertices[c][v]*3+1];
//                    z_sum = z_sum + mesh_->vertices_[cellVertices[c][v]*3+2];
//                    vertexCell[cellVertices[c][v]] = cellNum;

//                }

//                x_sum = x_sum/numVer;
//                y_sum = y_sum/numVer;
//                z_sum = z_sum/numVer;

//                new_vertices.push_back(x_sum);
//                new_vertices.push_back(y_sum);
//                new_vertices.push_back(z_sum);

//                cellNum = cellNum + 1;

//            }
//        }



        //computing the faces, as before
        int numFaces = (int) mesh_->faces_.size()/3;
        for (int i=0; i< numFaces; i++)
        {
            //every triangle of the face
            int ver_a = mesh_->faces_[3*i+0];
            int ver_b = mesh_->faces_[3*i+1];
            int ver_c = mesh_->faces_[3*i+2];

            //get the id of the cell of the vertices
            int id_a = vertexCell[ver_a];
            int id_b = vertexCell[ver_b];
            int id_c = vertexCell[ver_c];

            if(!(id_a == id_b || id_a==id_c || id_b == id_c))
            {
                new_faces.push_back(id_a);
                new_faces.push_back(id_b);
                new_faces.push_back(id_c);

            }
        }

        data_representation::ComputeVertexNormals(new_vertices,new_faces,&new_normals);

        std::cout<<"numero vertici:"<< new_vertices.size()<<"  numero normals:"<<new_normals.size()<<"  numero faccie:"<<new_faces.size()<<std::endl;

    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You have to create the octree first");
        msgBox.exec();
    }

}

void ex2::createOctree()
{
    oc = NULL;
    int numVertici = (int) mesh_->vertices_.size()/3;

    oc = new octree(mesh_->min_[0], mesh_->max_[0], mesh_->min_[1], mesh_->max_[1],mesh_->min_[2], mesh_->max_[2]);
    //selecting the cell for each vertex
    for (int i = 0; i < numVertici; i++ )
    {
        int ver_i = i*3;
        float x_v = mesh_->vertices_[ver_i+0];
        float y_v = mesh_->vertices_[ver_i+1];
        float z_v = mesh_->vertices_[ver_i+2];
        oc->insert(x_v,y_v,z_v, i);
    }

    std::cout << "----> created octree" << " with " << oc->num_nods << " nodes" <<std::endl;
    std::cout <<"depth of the created octree = " << oc->depthOfTheTree << std::endl;

    createdOctree = true;

}

void ex2::initVertexBuffer()
{


    if (mesh_ == nullptr) return;


    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);


    glGenBuffers(1,&vboVertex);
    glGenBuffers(1,&vboNormal);
    glGenBuffers(1,&vboIndex);



    if(LODsimpleON){



        if(octreeON)
            octreeVertexClustering();
        else
            vertexClustering();

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


        emit SetFaces(QString::number(new_faces.size() / 3));
        emit SetVertices(QString::number(new_vertices.size() / 3));


    }

    else
    {
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


    }


    glBindVertexArray(0);

}


QGroupBox* ex2::controlPanel()
{
    // init group box
    QGroupBox *groupBox = ex1::controlPanel();
    groupBox->setStyleSheet(GroupBoxStyle);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);

    QPushButton *buttonOctree = new QPushButton("Create Octree");

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


    connect(buttonOctree, SIGNAL(clicked()), this, SLOT(createOctree()));

    //Layout
    row++;
    layout->addWidget(buttonOctree,row,0);
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

void ex2::setNumberlod(int n){
    if(LODsimpleON){
        level=n;
        initializeGL();
        initVertexBuffer();
        paintGL();
    }
}

void ex2::setOFF(){
    LODsimpleON=false;
    octreeON=false;
    initVertexBuffer();
    paintGL();
}

void ex2::setOnBasic(){
    LODsimpleON=true;
    octreeON=false;
    initVertexBuffer();
    paintGL();
}


void ex2::setOnOctree()
{
    LODsimpleON=true;
    octreeON=true;
    if(!createdOctree)
        createOctree();
    initVertexBuffer();


}
