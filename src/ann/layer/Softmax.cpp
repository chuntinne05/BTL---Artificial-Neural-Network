/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   Softmax.cpp
 * Author: ltsach
 * 
 * Created on August 25, 2024, 2:46 PM
 */

#include "layer/Softmax.h"
#include "ann/functions.h"
#include "sformat/fmt_lib.h"
#include <filesystem> //require C++17
namespace fs = std::filesystem;

Softmax::Softmax(int axis, string name): m_nAxis(axis) {
    if(trim(name).size() != 0) m_sName = name;
    else m_sName = "Softmax_" + to_string(++m_unLayer_idx);
}

Softmax::Softmax(const Softmax& orig) {
}

Softmax::~Softmax() {
}

xt::xarray<double> Softmax::forward(xt::xarray<double> X) {
    //YOUR CODE IS HERE
    xt::xarray<double> X_shifted = X - xt::amax(X, {m_nAxis}, xt::keep_dims);
    xt::xarray<double> exps = xt::exp(X_shifted);
    m_aCached_Y = exps / xt::sum(exps, {m_nAxis}, xt::keep_dims);
    return m_aCached_Y;
}   
xt::xarray<double> Softmax::backward(xt::xarray<double> DY) {
    //YOUR CODE IS HERE
    return DY - m_aCached_Y * xt::sum(DY, {m_nAxis}, xt::keep_dims);
}

string Softmax::get_desc(){
    string desc = fmt::format("{:<10s}, {:<15s}: {:4d}",
                    "Softmax", this->getname(), m_nAxis);
    return desc;
}
