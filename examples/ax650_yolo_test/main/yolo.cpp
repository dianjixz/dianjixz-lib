#include <iostream>
#include "d_hal_nn.hpp"
int main() {
    std::cout << "yolo demo start" << std::endl;
    std::shared_ptr<d_hal::d_hal_nn> nn = d_hal::create_d_hal_nn(d_hal::D_HAL_NN_AXERA_AX650C);


    if (nn->load_model("yolo_v5.axmodel") != 0) {
        std::cout << "load model failed" << std::endl;
        return -1;
    }

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


    return 0;
}