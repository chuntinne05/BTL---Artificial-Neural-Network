/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   CrossEntropy.cpp
 * Author: ltsach
 * 
 * Created on August 25, 2024, 2:47 PM
 */

#include "loss/CrossEntropy.h"
#include "ann/functions.h"

CrossEntropy::CrossEntropy(LossReduction reduction): ILossLayer(reduction){
    
}

CrossEntropy::CrossEntropy(const CrossEntropy& orig):
ILossLayer(orig){
}

CrossEntropy::~CrossEntropy() {
}

double CrossEntropy::forward(xt::xarray<double> X, xt::xarray<double> t){
    //YOUR CODE IS HERE
    m_aCached_Ypred = X;
    auto batch_size = X.shape()[0];

    xt::xarray<double> log_preds = xt::log(X);
    xt::xarray<double> loss = -xt::sum(t * log_preds);

    if (m_eReduction == REDUCE_MEAN)
    {
        return xt::mean(loss)();
    }
    else if (m_eReduction == REDUCE_SUM)
    {
        return xt::sum(loss)();
    }
    else
    {
        return loss();
    }
    
}
xt::xarray<double> CrossEntropy::backward() {
    //YOUR CODE IS HERE
    xt::xarray<double> grad = m_aCached_Ypred - m_aYtarget;
    if (m_eReduction == REDUCE_MEAN) 
    {
        grad /= m_aCached_Ypred.shape()[0];
    }
    return grad;
}