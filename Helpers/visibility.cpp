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
        for(uint j = 0; j < worldMatrix[0].size(); j++)
        {
            std::vector<int> firstValue;
            firstValue.push_back(i);
            firstValue.push_back(j);
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

//        rayCasting(x[0],y[0],x[1],y[1]);
        superRayCasting(x[0],y[0],x[1],y[1]);

//        x.clear();
//        y.clear();
//        w.clear();
//        //method 2
//        for (int i = 0; i < xMax; i++)
//        {
//            x[0] = i;
//            x[1] = i;
//            y[0] = 0;
//            y[1] = yMax-1;
//            superRayCasting(x[0],y[0],x[1],y[1]);
//        }
//        for (int i = 0; i < yMax; i++)
//        {
//            y[0] = i;
//            y[1] = i;
//            x[0] = 0;
//            x[1] = xMax-1;
//            superRayCasting(x[0],y[0],x[1],y[1]);
//        }

     }
}


void visibility::addPoint(int x, int y)
{
    std::cout << "          addPoint: casted point ("<<x<<", "<<y<<")" << std::endl;


    int cell_num = worldMatrix[x][y];

    visibility_set.push_back(x);
    visibility_set.push_back(y);

    if(inside)
    {
        if(cell_num == 0 || last_point == true)
        {
            //finished visibility
            updateVisibility(visibility_set);
            visibility_set.clear();
            inside = false;
            visibility_set.push_back(x);
            visibility_set.push_back(y);
        }
        else if(cell_num == 2)
        {
            inside = false;
        }
    }
    else
    {
        if(cell_num == 1 || last_point == true)
        {
            //finished visibility
            updateVisibility(visibility_set);
            visibility_set.clear();
            inside = true;
            visibility_set.push_back(x);
            visibility_set.push_back(y);
        }
        else if(cell_num == 2)
        {
            inside = true;
        }
    }

}

void visibility::superRayCasting(int x1, int y1, int x2, int y2)
{
    int i;
    int ystep, xstep;
    int error;
    int errorprev;
    int y = y1, x = x1;
    int ddy, ddx;
    int dx = x2 - x1;
    int dy = y2 - y1;

    std::cout << "superRayCasting: the line between a A("<<x1<<", "<<y1<<") e B("<<x2<<", "<<y2<<")  has slope :" <<((float)dy/(float)dx)<< std::endl;


    //initialization
    visibility_set.clear();
    last_point = false;
    if(worldMatrix[x1][y1] != 1)
        inside = false;
    else
        inside = true;


    //point 1
    addPoint(x1, y1);

    if (dy < 0){
      ystep = -1;
      dy = -dy;
    }else
      ystep = 1;

    if (dx < 0){
      xstep = -1;
      dx = -dx;
    }else
      xstep = 1;

    ddy = 2 * dy;
    ddx = 2 * dx;

    if (ddx >= ddy){  // first octant (0 <= slope <= 1)

        error = dx;
        errorprev = error;  // start in the middle of the square
        for (i=0 ; i < dx ; i++){  // do not use the first point (already done)
          x += xstep;
          error += ddy;
          if (error > ddx){  // increment y if AFTER the middle ( > )
            y += ystep;
            error -= ddx;
            // three cases (octant == right->right-top for directions below):
            if (error + errorprev < ddx)  // bottom square also
                addPoint(x, y-ystep);
            else if (error + errorprev > ddx)  // left square also
                addPoint (x-xstep, y);
            else{  // corner: bottom and left squares also
                addPoint (x, y-ystep);
                addPoint (x-xstep, y);
            }
          }
          addPoint(x, y);
          errorprev = error;
        }
      }else{  // the same as above
        errorprev = error = dy;
        for (i=0 ; i < dy ; i++){
          y += ystep;
          error += ddx;
          if (error > ddy){
            x += xstep;
            error -= ddy;
            if (error + errorprev < ddy)
                addPoint(x-xstep, y);
            else if (error + errorprev > ddy)
                addPoint(x, y-ystep);
            else{
                addPoint(x-xstep, y);
                addPoint(x, y-ystep);
            }
          }
          addPoint(x, y);
          errorprev = error;
        }
      }
    //adding again the last point to compute his visibility
    if((y == y2) && (x == x2))
    {
        last_point = true;
        addPoint(x,y);
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

    float err = 0;

    int oldValue;

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

        err = 0;
        int y = y_start;

        oldValue = worldMatrix[x_start][y_start];

        std::vector<int> visible_set;
        for(int x = x_start; x <= x_end; x++)
        {
            std::cout << "      x-based point ("<<x<<" , "<<y<<")"<< std::endl;

            visible_set.push_back(x);
            visible_set.push_back(y);


            //if we pass from a visible set to another we store the visibility and clear the visible set
            if((worldMatrix[x][y] == 0 && oldValue ==1) || (worldMatrix[x][y] == 1 && oldValue == 0) || (x == x_end))
            {
                updateVisibility(visible_set);
                visible_set.clear();
                if(x!=x_end)
                {
                    visible_set.push_back(x);
                    visible_set.push_back(y);
                 }

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

        err = 0;
        oldValue = worldMatrix[x_start][y_start];


        int x = x_start;
        std::vector<int> visible_set;

        for(int y = y_start; y <= y_end; y++)
        {
            std::cout << "      y-based point ("<<x<<" , "<<y<<")"<< std::endl;

            visible_set.push_back(x);
            visible_set.push_back(y);

            //if we pass from a visible set to another we store the visibility and clear the visible set
            if((worldMatrix[x][y] == 0 && oldValue ==1) || (worldMatrix[x][y] == 1 && oldValue == 0) || ( y == y_end))
            {
                updateVisibility(visible_set);
                visible_set.clear();
                if(y != y_end)
                {
                    visible_set.push_back(x);
                    visible_set.push_back(y);
                }

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


void visibility::updateVisibility(std::vector<int> visible_set)
{
    std::cout << "updateVisibility: visibility matrix dimension x:" << visibilityMatrix.size() <<"  y:"<< visibilityMatrix[0].size() << "   and visible set size: " << visible_set.size() <<std::endl;
    for (uint i = 0; i < (visible_set.size() - 1) ; i = i+2)
    {
        int cel_x = visible_set[i];
        int cel_y = visible_set[i + 1];
        std::cout << "updateVisibility: start updating visibility set of cell " << cel_x << " : " << cel_y << std::endl;

        for (uint j = 0; j < (visible_set.size() - 1) ; j = j+2)
            if(j!=i)
            {
                int a = visible_set[j];
                int b = visible_set[j +1];
                int new_a = a;
                int new_b = b;

                for(int on_a = -1; on_a <= 1; on_a ++ )
                {
                    for(int on_b = -1; on_b <= 1; on_b ++)
                    {
                        new_a = a + on_a;
                        new_b = b + on_b;
                        //check if it is outside the map
                        bool insideMap = false;
                        if(new_a >= 0 && new_a < worldMatrix.size() && new_b >= 0 && new_b < worldMatrix[0].size())
                            insideMap = true;

                        //check if the cell is already isnide the visibility list
                        bool newValue = true;
                        for(uint k = 0; k < visibilityMatrix[cel_x][cel_y].size()-1; k = k+2)
                        {
                            int old_a = visibilityMatrix[cel_x][cel_y][k];
                            int old_b = visibilityMatrix[cel_x][cel_y][k+1];
                            if(new_a == old_a && new_b == old_b)
                                newValue = false;
                        }


                        if(newValue && insideMap)
                        {
                            visibilityMatrix[cel_x][cel_y].push_back(new_a);
                            visibilityMatrix[cel_x][cel_y].push_back(new_b);
                        }

                    }
                }


            }
    std::cout << "updateVisibility: updated visibility set of cell " << cel_x << " : " << cel_y << std::endl;
    }


}

std::vector<int> visibility::getVisibilityOfCell(int i, int j)
{

    std::vector<int> visibility_list;
    std::cout << "getVisibility: prendere lista di dimensione " <<visibilityMatrix[i][j].size() << std::endl;

    for(uint a = 0; a < visibilityMatrix[i][j].size(); a++)
    {
        if(i == 0 && j == 0)
        {
//           std::cerr<< " visibility cell of 0 : 0" << visibilityMatrix[i][j][a] << std::endl;
        }
        visibility_list.push_back( visibilityMatrix[i][j][a]);
    }

    std::cout << "getVisibility: restituita lista visibility di dimensione " <<visibility_list.size() << "   e quindi celle visibili sono " << visibility_list.size() /2 << std::endl;

    return visibility_list;
}


