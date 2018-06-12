#ifndef visibility_H
#define visibility_H

#include <iostream>
#include <vector>
#include <random>
#include <fstream>

class visibility
{
private:
    //input world matrix
    std::vector<std::vector<int>> worldMatrix;

    //for each cell it store the list of visibility cells
    std::vector<std::vector<std::vector<int>>> visibilityMatrix;

    //ray casting 2
    void superRayCasting(int x1, int y1, int x2, int y2);
    void addPoint(int x, int y);
    std::vector<int> visibility_set;
    bool last_point;
    bool inside;
    //ray casting 1
    void rayCasting(int x_a, int y_a, int x_b, int y_b);
    void updateVisibility(std::vector<int> visible_set);


public:
    visibility();
    void computeVisibility(std::vector<std::vector<int>> inMatrix, int rayNum);
    std::vector<int> getVisibilityOfCell(int i, int j);


};

#endif // visibility_H
