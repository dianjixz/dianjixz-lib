#include <stdio.h>
#include <iostream>
#include "linalg.h"
int main() {
    printf("Hello, World!\n");
    linalg::aliases::float2 nihao;
    nihao.x = 1.0;
    nihao.y = 2.0;

    linalg::aliases::int2 nihao_int;
    nihao_int.x = 1;
    nihao_int.y = 2;
    nihao_int.w = 3;
    nihao_int.h = 4;
    linalg::vec<float, 4> nihao_vec;
    nihao_vec.X = 3.0;
    nihao_vec.Y = 4.0;
    nihao_vec.W = 5.0; // use w to access y component
    nihao_vec.H = 6.0; // use h to access y component
    nihao_vec.C0 = 7.0; // use C0 to access y component
    nihao_vec.C1 = 8.0; // use C1 to access y component
    nihao_vec[0] = 9.0; // use operator[] to access x component
    nihao_vec[1] = 10.0; // use operator[] to access
    std::cout << "nihao_vec.y = " << nihao_vec.y << std::endl;

    linalg::vec<float, 10> nihao_vec1;


    linalg::aliases::int4x4 mat;
    linalg::mat<char,4,4> mat2;

    // mat.x.x = 1;
    // mat.x.x = 1;
    // mat[0][0] = 1;
    // mat.x.x = 1.0;
    // mat.y.y = 2.0;
    // mat.z.z = 3.0;
    // mat.w.w = 4.0;
    // std::cout << "mat = " << mat << std::endl;

    return 0;
}