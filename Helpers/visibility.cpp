#include "visibility.h"

visibility::visibility()
{


}


void visibility::computeVisibility(std::vector<std::vector<int>> inMatrix, int rayNum)
{
    worldMatrix.clear();
    worldMatrix = inMatrix;
    std::cout << "computeVisibility: read the input matrix of dimension : " << worldMatrix.size() << " and:" << worldMatrix[0].size() << std::endl;
    std::cout << "computeVisibility: number of rays : " << rayNum << std::endl;

    int xMax = worldMatrix.size();
    int yMax = worldMatrix[0].size();


    //intialize visibility matrix
    for(uint i = 0; i < worldMatrix.size(); i++)
    {
        std::vector<std::vector<int>> firstLine;
        for(uint j = 0; j <worldMatrix[0].size(); j++)
        {
            std::vector<int> firstValue;
            firstValue.push_back(worldMatrix[i][j]);
            firstLine.push_back(firstValue);
        }
        visibilityMatrix.push_back(firstLine);

    }

    //select two walls --> select two different random number
    // 0 = (0,0)->(xMax,0) , 1 = (0,yMax)->(xMax,yMax), 2 = (0,0)->(0,yMax), 3 = (xMax,0)->(xMax,yMax)

    std::vector<int> w(2);
    std::vector<int> x(2);
    std::vector<int> y(2);

    for( int r = 0; r<rayNum; r++)
    {
        w.clear();
        x.clear();
        y.clear();
        std::cout << "ray numero: " <<r <<std::endl;
        do
        {
            w[0] = std::rand() %4;
            w[1] = std::rand() %4;
        } while(w[0] == w[1]);

        std::cout << " muri scelti " << w[0] << "  e " << w[1] <<std::endl;

        for(int i = 0; i <2; i++)
        {
            if(w[i] == 0 || w[i] == 1)
            {
                x[i] = std::rand() %xMax;
                if(w[i] == 0)
                    y[i] = 0;
                else
                    y[i] = yMax-1;
            }
            else
            {
                y[i] = std::rand() %yMax;
                if(w[i] == 2)
                    x[i] = 0;
                else
                    x[i] = xMax-1;
            }
            std::cout << " valori scelti " << x[i] << "  e " << y[i] <<std::endl;

        }

        rayCasting(x[0],y[0],x[1],y[1]);
     }
}



void visibility::rayCasting(int x_a, int y_a, int x_b, int y_b)
{
    //slope
    float m = (float)(y_b - y_a)/ (float)(x_b - x_a);
    std::cout << "rayCasting: the line between a A("<<x_a<<", "<<y_a<<") e B("<<x_b<<", "<<y_b<<")  has slope :" <<m<< std::endl;

    //defining the starting x and ending x of the dominio
    int x_start;
    int x_end;
    int y_start;
    int y_end;
    //checking the endpoints

    //cheking if the sloop is in -1 < m < 1
    if(m < 1 && m > -1)
    {
        if(x_a > x_b)
        {
            x_start = x_b;
            x_end = x_a;

            y_start = y_b;
            y_end = y_a;
        }
        else
        {
            x_start = x_a;
            x_end = x_b;

            y_start = y_a;
            y_end = y_b;
        }

        float err = 0;
        int y = y_start;

        int oldValue = worldMatrix[x_start][y_start];

        for(int x = x_start; x <= x_end; x++)
        {
            std::cout << "      x-based point ("<<x<<" , "<<y<<")"<< std::endl;
            std::vector<std::vector<int>> visible_set;
            std::vector<int> point(2);
            point[0] = x;
            point[1] = y;
            visible_set.push_back(point);

            //if we pass from a visible set to another we store the visibility and clear the visible set
            if((worldMatrix[x][y] == 0 && oldValue ==1) || (worldMatrix[x][y] == 1 && oldValue == 0) || (x == x_end))
            {
                updateVisibility(visible_set);
                visible_set.clear();
            }

            oldValue = worldMatrix[x][y];

            if(m < 0)
            {
                if( err + m > -0.5)
                    err = (err + m);
                else
                {
                    y = y - 1;
                    err = err + m + 1;
                }
            }
            else
            {
//                std::cout << "      point ("<<x<<" , "<<y<<")"<< std::endl;
                if( err + m < 0.5)
                    err = (err + m);
                else
                {
                    y = y + 1;
                    err = err + m - 1;
                }
            }
        }
    }
    else
    {
        //new slope
        m = (float)(x_b - x_a)/(float)(y_b - y_a) ;
        std::cout << "new slope :" <<m<< std::endl;
        if(y_a > y_b)
        {
            x_start = x_b;
            x_end = x_a;

            y_start = y_b;
            y_end = y_a;
        }
        else
        {
            x_start = x_a;
            x_end = x_b;

            y_start = y_a;
            y_end = y_b;
        }
        std::cout << "new x_start :" << x_start << "  x_end :" << x_end <<"   y_start :"<<y_start << "   y_end :" << y_end <<std::endl;

        float err = 0;
        int oldValue = worldMatrix[x_start][y_start];


        int x = x_start;
        for(int y = y_start; y <= y_end; y++)
        {
            std::cout << "      y-based point ("<<x<<" , "<<y<<")"<< std::endl;
            std::vector<std::vector<int>> visible_set;
            std::vector<int> point(2);
            point[0] = x;
            point[1] = y;
            visible_set.push_back(point);

            //if we pass from a visible set to another we store the visibility and clear the visible set
            if((worldMatrix[x][y] == 0 && oldValue ==1) || (worldMatrix[x][y] == 1 && oldValue == 0) || ( y == y_end))            {
                updateVisibility(visible_set);
                visible_set.clear();
            }

            oldValue = worldMatrix[x][y];
            if(m < 0)
            {
                if( err + m > -0.5)
                    err = (err + m);
                else
                {
                    x = x - 1;
                    err = err + m + 1;
                }
            }
            else
            {
//                std::cout << "      point ("<<x<<" , "<<y<<")"<< std::endl;
                if( err + m < 0.5)
                    err = (err + m);
                else
                {
                    x = x + 1;
                    err = err + m - 1;
                }
            }
        }
    }



}


void visibility::updateVisibility(std::vector<std::vector<int>> visible_set)
{
    std::cout << "updateVisibility: visibility matrix dimension x:" << visibilityMatrix.size() <<"  y:"<< visibilityMatrix[0].size() << "   and visible set size: " << visible_set.size() <<std::endl;
    for (uint i = 0; i < visible_set.size() ; i++)
    {
        int cel1 = visible_set[i][0];
        int cel2 = visible_set[i][1];
        std::vector<std::vector<int>> visibility_cells;
        std::cout << "updateVisibility: start updating visibility set of cell " << cel1 << " : " << cel2 << std::endl;

        for (uint j = 0; j < visible_set.size() ; j++)
            if(j!=i)
            {
                visibilityMatrix[cel1][cel2].push_back(visible_set[j][0]);
                visibilityMatrix[cel1][cel2].push_back(visible_set[j][1]);


                visibility_cells.push_back(visible_set[j]);
            }
    std::cout << "updateVisibility: updated visibility set of cell " << cel1 << " : " << cel2 << std::endl;
    }


}

std::vector<int> visibility::getVisibilityOfCell(int i, int j)
{





}
