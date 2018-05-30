#ifndef OCTREE_H
#define OCTREE_H

#include <vector>


struct node
{
  int idNode = 0;
  int depth = 0;
  bool isTerminal = true;

  //value of the vertex insert in the node
  std::vector<float> key_value;
  int key_index = 0;
  int counter = 0;

  //indices
  std::vector<int> vertices;


  //mean computation
  float sum_x = 0;
  float sum_y = 0;
  float sum_z = 0;
  int num = 0;
  std::vector<float> mean;


  //pointers
  node *dad = NULL;

  node *mini0 = NULL;
  node *mini1 = NULL;
  node *mini2 = NULL;
  node *mini3 = NULL;
  node *mini4 = NULL;
  node *mini5 = NULL;
  node *mini6 = NULL;
  node *mini7 = NULL;

  std::vector<node*> minis = {mini0, mini1, mini2, mini3, mini4, mini5, mini6, mini7};

  //dimensions
  float x_min;
  float x_max;

  float y_min;
  float y_max;

  float z_min;
  float z_max;

};

class octree
{

private:
    node *root;
    int max_depth;

    int newIndexCounter = 0;
    //indices
    std::vector<int> newVertices;
    //indices per cell
    std::vector<std::vector<int>> oldVertices;
    //value of the new vertices
    std::vector<std::vector<float>> newMeans;

public:
    int num_nods = 0;
    int depthOfTheTree = 0;

    octree(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max);
    octree(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max, int maxDepth);

    void setLevel(int selectedDepth);
    std::vector<int> getNewVertices();
    std::vector<std::vector<int>> getOldVerticesIndices();
    std::vector<std::vector<float>> getMeans();


    //index-octree
    void insert(float x, float y, float z, int index);
    void insert(std::vector<float> key, int index);

    void computeMean(node *nd);




private:
    void insertValue(std::vector<float> key, node *nd, int index);

    void computeDimension(node *nd,int x_i, int y_i, int z_i);

    node* createNode(node *dad, std::vector<float> key, int index);

    void explore(node *nd, int selectedDepth);



};

#endif // OCTREE_H
