#ifndef DATASET_H
#define DATASET_H
#include "tensor/xtensor_lib.h"
using namespace std;

template <typename DType, typename LType>
class DataLabel
{
private:
  xt::xarray<DType> data;
  xt::xarray<LType> label;

public:
  DataLabel(xt::xarray<DType> data, xt::xarray<LType> label)
      : data(data), label(label) {}
  xt::xarray<DType> getData() const { return data; }
  xt::xarray<LType> getLabel() const { return label; }
};
template <typename DType, typename LType>
class Batch
{
private:
  xt::xarray<DType> data;
  xt::xarray<LType> label;

public:
  Batch(xt::xarray<DType> data, xt::xarray<LType> label) : data(data), label(label) {}
  virtual ~Batch() {}
  xt::xarray<DType> &getData() { return data; }
  xt::xarray<LType> &getLabel() { return label; }
};
template <typename DType, typename LType>
class Dataset
{
public:
  Dataset() {};
  virtual ~Dataset() {};
  virtual int len() = 0;
  virtual DataLabel<DType, LType> getitem(int index) = 0;
  // virtual DataLabel<DType, LType> getitem(int index, bool shuffle) = 0;
  virtual xt::svector<unsigned long> get_data_shape() = 0;
  virtual xt::svector<unsigned long> get_label_shape() = 0;
};

//////////////////////////////////////////////////////////////////////
template <typename DType, typename LType>
class TensorDataset : public Dataset<DType, LType>
{
private:
  xt::xarray<DType> data;
  xt::xarray<LType> label;
  xt::svector<unsigned long> data_shape, label_shape;

public:
  TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label)
      : data(data), label(label)
  {
    for (int i = 0; i < data.dimension(); i++)
    {
      this->data_shape.push_back(data.shape()[i]);
    }
    for (int i = 0; i < label.dimension(); i++)
    {
      this->label_shape.push_back(label.shape()[i]);
    }
  }

  int len()
  {
    return data_shape[0];
  }

  DataLabel<DType, LType> getitem(int index)
  {
    if (index < 0 || index > data_shape[0])
    {
      throw out_of_range("Index is out of range!");
    }
    if (label.dimension() == 0)
    {
      return DataLabel<DType, LType>(xt::view(data, index), xt::xarray<LType>());
    }
    return DataLabel<DType, LType>(xt::view(data, index), xt::view(label, index));
  }

  xt::svector<unsigned long> get_data_shape()
  {
    return data_shape;
  }
  xt::svector<unsigned long> get_label_shape()
  {
    if (label_shape.size() == 0)
    {
      return {};
    }
    return label_shape;
  }
};

#endif /* DATASET_H */