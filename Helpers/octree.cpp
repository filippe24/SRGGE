#include "octree.h"

#include <iostream>


//two way of creating the octree, defining or not the maxDepth

octree::octree(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max)
{
    max_depth = 100;
    //creating the octree root node
    root = new node;
    //setting the dimension of the bounding box
    root->x_min = x_min;
    root->x_max = x_max;
    root->y_min = y_min;
    root->y_max = y_max;
    root->z_min = z_min;
    root->z_max = z_max;
    //set the octree as empty
    root->isTerminal = true;
    std::cout<< "Initialized octree dimensions, x: " << x_max - x_min << " y " << y_max - y_min << " z "  << z_max - z_min <<" and max depth " << 100 << std::endl;
}
octree::octree(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max,int maxDepth)
{
    max_depth = maxDepth;
    //creating the octree root node
    root = new node;
    //setting the dimension of the bounding box
    root->x_min = x_min;
    root->x_max = x_max;
    root->y_min = y_min;
    root->y_max = y_max;
    root->z_min = z_min;
    root->z_max = z_max;
    //set the octree as empty
    root->isTerminal = true;
    std::cout<< "Initialized octree dimensions, x: " << x_max - x_min << " y " << y_max - y_min << " z "  << z_max - z_min <<" and max depth " << maxDepth<< std::endl;
}


void octree::setLevel(int selectedDepth)
{
    std::cout << "Selecting Level" << std::endl;
    std::cout << "----> selcted depth " << selectedDepth << std::endl;

    newIndexCounter = 0;
    newVertices.clear();
    oldVertices.clear();
    newMeans.clear();

    if (root==NULL) { std::cerr << "octree not initialized" << std::endl;
    return;}

    explore(root, selectedDepth);

    std::cout << "----> created " << newIndexCounter << "new Vertices" << std::endl;
}

void octree::explore(node *nd, int selectedDepth)
{
    if((nd->isTerminal) || (nd->depth == selectedDepth))
    {

        newVertices.push_back(newIndexCounter);
        oldVertices.push_back(nd->vertices);
        newMeans.push_back(nd->mean);
        newIndexCounter = newIndexCounter + 1;

    }
    else
    {
        bool has_minis = false;
        for(int i = 0; i<8; i++)
        {
            if(nd->minis[i] != NULL)
            {
                explore(nd->minis[i], selectedDepth);
                has_minis = true;
            }
        }
        if(!has_minis)
            std::cout << "non ha figli " << nd->idNode <<" ma e' terminal ? " << nd->isTerminal << std::endl;
    }

}


std::vector<int> octree::getNewVertices()
{
    return newVertices;
}

std::vector<std::vector<int>> octree::getOldVerticesIndices()
{
    return oldVertices;
}
std::vector<std::vector<float> > octree::getMeans()
{
    return newMeans;
}

void octree::insert(std::vector<float> key, int index)
{
    insert(key[0],key[1],key[2],index);
}

void octree::insert(float x,float y, float z, int index)
{
    std::vector<float> key = {x,y,z};
    if((x>root->x_max)||(x<root->x_min)||(y>root->y_max)||(y<root->y_min)||(z>root->z_max)||(z<root->z_min))
        std::cerr << "try to insert a vertex out of range" << std::endl;

    if(root->isTerminal)
    {
        std::cout << "----> creating octree" << std::endl;
        root->isTerminal = false;
        insertValue(key,root,index);
    }
    else
        insertValue(key,root,index);

}
void octree::insertValue(std::vector<float> key, node *nd, int index)
{

    float x_v = key[0];
    float y_v = key[1];
    float z_v = key[2];

//    if((x_v < nd->x_min) || (x_v > nd->x_max))
//        std::cerr << "vertex i : " << index << " at depth" <<nd->depth << " has x dimension out of bound  x_dim =" << x_v << " min =" << nd->x_min <<" max =" << nd->x_max << std::endl;
//    if((y_v < nd->y_min) || (y_v > nd->y_max))
//        std::cerr << "vertex i : " << index << " at depth" <<nd->depth << " has y dimension out of bound  y_dim = " << y_v << " min =" << nd->y_min <<" max =" << nd->y_max << std::endl;
//    if((z_v < nd->z_min) || (z_v > nd->z_max))
//        std::cerr << "vertex i : " << index << " at depth" <<nd->depth << " has z dimension out of bound  z_dim = " << z_v << " min =" << nd->z_min <<" max =" << nd->z_max << std::endl;

    if(nd->isTerminal)
    {
        if ((nd->key_value[0]==key[0] && nd->key_value[1]==key[1] && nd->key_value[2]==key[2]) || (nd->depth == max_depth))
        {
            nd->counter = nd->counter + 1;

        }
        else
        {
            nd->isTerminal = false;
            //problema indice
            insertValue(nd->key_value,nd,nd->key_index);
            insertValue(key,nd,index);
        }
    }

    else
    {
        float half_x = nd->x_min + (nd->x_max - nd->x_min)/2;
        float half_y = nd->y_min + (nd->y_max - nd->y_min)/2;
        float half_z = nd->z_min + (nd->z_max - nd->z_min)/2;


        int x_i;
        if(x_v < half_x ) x_i = 0;
        else x_i = 1;
        int y_i;
        if(y_v < half_y ) y_i = 0;
        else y_i = 1;
        int z_i;
        if(z_v < half_z ) z_i = 0;
        else z_i = 1;

        int mini_id = x_i + y_i*2 + z_i*4;

        if(nd->minis[mini_id] != NULL)
        {
            insertValue(key, nd->minis[mini_id],index);
        }
        else
        {
            //creating a node
            node *mini;
            mini = createNode(nd, key,index);
            //setting the new dimensions of the new node
            if(x_i == 1)
            {
                mini->x_min = half_x;
                mini->x_max = nd->x_max;
            }
            else
            {
                mini->x_min = nd->x_min;
                mini->x_max = half_x;
            }
            if(y_i == 1)
            {
                mini->y_min = half_y;
                mini->y_max = nd->y_max;

            }
            else
            {
                mini->y_min = nd->y_min;
                mini->y_max = half_y;
            }
            if(z_i == 1)
            {
                mini->z_min = half_z;
                mini->z_max = nd->z_max;
            }
            else
            {
                mini->z_min = nd->z_min;
                mini->z_max = half_z;
            }
//            computeDimension(mini, x_i, y_i, z_i);
            nd->minis[mini_id] = mini;

            if(x_v < mini->x_min )
                std::cerr << "creato miniid:" << mini_id << " minore x range, at depth:"<< mini->depth <<  "  vettore:" << index << "  val:" << x_v << " e xmin "<< nd->x_min << " e la meta':" << half_x <<" e classificato:" <<x_i << " e i valori del mini scelto sono min:" << mini->x_min << " max:" << mini->x_max << std::endl;
            if (x_v > mini->x_max )
                std::cerr << "creato miniid:" << mini_id << " maggiore x range, at depth:"<< mini->depth <<  "  vettore:" << index << "  val:" << x_v << " e xMax "<< nd->x_max << " e la meta':" << half_x << " e classificato:" <<x_i << " e i valori del mini scelto sono min:" << mini->x_min << " max:" << mini->x_max <<   std::endl;
            if(y_v < mini->y_min )
                std::cerr << "creato miniid:" << mini_id << " minore y range, at depth:"<< mini->depth <<  "  vettore:" << index << "  val:" << y_v << " e ymin "<< nd->y_min << " e la meta':" << half_y << " e classificato:" <<y_i <<  " e i valori del mini scelto sono min:" << mini->y_min << " max:" << mini->y_max <<  std::endl;
            if (y_v > mini->y_max )
                std::cerr << "creato miniid:" << mini_id << " maggiore y range, at depth:"<< mini->depth <<  "  vettore:" << index << "  val:" << y_v << " e yMax "<< nd->y_max << " e la meta':" << half_y << " e classificato:" <<y_i <<  " e i valori del mini scelto sono min:" << mini->y_min << " max:" << mini->y_max <<  std::endl;
            if(z_v < mini->z_min )
                std::cerr << "creato miniid:" << mini_id << " minore z range, at depth:"<< mini->depth <<  "  vettore:" << index << "  val:" << z_v << " e zmin "<< nd->z_min << " e la meta':" << half_z << " e classificato:" <<z_i <<  " e i valori del mini scelto sono min:" << mini->z_min << " max:" << mini->z_max <<  std::endl;
            if (z_v > mini->z_max )
                std::cerr << "creato miniid:" << mini_id << " maggiore z range, at depth:"<< mini->depth <<  "  vettore:" << index << "  val:" << z_v << " e zMax "<< nd->z_max <<  " e la meta':" << half_z <<" e classificato:" <<z_i <<  " e i valori del mini scelto sono min:" << mini->z_min << " max:" << mini->z_max << std::endl;



        }
    }

    nd->vertices.push_back(index);

    //update the mean of the node
    nd->sum_x = nd->sum_x + key[0];
    nd->sum_y = nd->sum_y + key[1];
    nd->sum_z = nd->sum_z + key[2];
    nd->num = nd->num + 1;
    computeMean(nd);


//    std::cout << "updated mean of node at depth "<< nd->depth << " with value " << nd->mean[0] << " " << nd->mean[1] << " " << nd->mean[2] << std::endl;
 }
node* octree::createNode(node *dad, std::vector<float> key, int index)
{
    node *new_node = new node;
    new_node->dad = dad;
    new_node->key_value = key;
    new_node->key_index = index;
    new_node->isTerminal = true;
    new_node->depth = (dad->depth) + 1;

    if(new_node->depth > depthOfTheTree)
        depthOfTheTree = new_node->depth;

    new_node->sum_x = key[0];
    new_node->sum_y = key[1];
    new_node->sum_z = key[2];
    new_node->num = 1;
    computeMean(new_node);

    new_node->counter = 1;

    num_nods = num_nods + 1;
    new_node->idNode = num_nods;

    new_node->vertices.push_back(index);

//    std::cout << "node created " << std::endl;
    return new_node;
}


//NOT WORKING :(
void octree::computeDimension(node *nd, int x_i, int y_i, int z_i)
{
    //x
    float x_min_d = nd->dad->x_min;
    float x_max_d = nd->dad->x_max;
    float x_half_d = x_min_d + ((x_max_d - x_min_d) / 2);


    nd->x_min = x_min_d  + (x_i)*x_half_d;
    nd->x_max = x_half_d + (x_i)*x_half_d;

    //y
    float y_min_d = nd->dad->y_min;
    float y_max_d = nd->dad->y_max;
    float y_half_d = y_min_d + (y_max_d - y_min_d) / 2;

    nd->y_min = y_min_d  + (y_i)*y_half_d;
    nd->y_max = y_half_d + (y_i)*y_half_d;

    //z
    float z_min_d = nd->dad->z_min;
    float z_max_d = nd->dad->z_max;
    float z_half_d = z_min_d + (z_max_d - z_min_d) / 2;

    nd->z_min = z_min_d  + (z_i)*z_half_d;
    nd->z_max = z_half_d + (z_i)*z_half_d;

//    std::cout << "dimension of the new node computed x: " << x_min_d << " " << x_max_d << " y : " << y_min_d << " " << y_max_d << " z : " << z_min_d << " " << z_max_d << std::endl;


}


void octree::computeMean(node* nd)
{

    int numVert = nd->num;
    float x = (nd->sum_x)/numVert;
    float y = (nd->sum_y)/numVert;
    float z = (nd->sum_z)/numVert;
    std::vector<float> new_vec = {x,y,z};
    nd->mean = new_vec;

}





