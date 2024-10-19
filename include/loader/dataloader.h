#ifndef DATALOADER_H
#define DATALOADER_H
#include "tensor/xtensor_lib.h"
#include "dataset.h"

using namespace std;

template <typename DType, typename LType>
class DataLoader
{
public:
private:
  Dataset<DType, LType> *ptr_dataset;
  int batch_size;
  bool shuffle;
  bool drop_last;
  xt::xarray<unsigned long> indices;
  int m_seed;

public:
  DataLoader(Dataset<DType, LType> *ptr_dataset,
             int batch_size,
             bool shuffle = true,
             bool drop_last = false, int seed = -1)
      : ptr_dataset(ptr_dataset), batch_size(batch_size), shuffle(shuffle), drop_last(drop_last), m_seed(seed)
  {
    int dataset_size = this->ptr_dataset->len();
    this->indices = xt::arange<unsigned long>(0, dataset_size);

    if (this->shuffle)
    {
      this->handleShuffle();
    }
  }
  void handleShuffle()
  {
    if (m_seed >= 0)
    {
      xt::random::seed(m_seed);
    }
    else
    {
      xt::random::seed(std::random_device{}());
    }
    xt::random::shuffle(this->indices);

    // cout << "Shuffled indices: " << this->indices << endl;
  }
  virtual ~DataLoader()
  {
    this->ptr_dataset = nullptr;
    this->batch_size = 0;
    this->shuffle = false;
    this->drop_last = false;
    this->indices = xt::xarray<unsigned long>();
  }

  /////////////////////////////////////////////////////////////////////////
  // The section for supporting the iteration and for-each to DataLoader //
  /// START: Section                                                     //
  /////////////////////////////////////////////////////////////////////////

  class Iterator
  {
  private:
    DataLoader<DType, LType> *loader;
    int batch_start;

  public:
    Iterator(DataLoader<DType, LType> *loader, int start)
        : loader(loader), batch_start(start) {}

    bool operator!=(const Iterator &other) const
    {
      return batch_start != other.batch_start;
    }

    Iterator &operator=(const Iterator &other)
    {
      this->loader = other.loader;
      this->batch_start = other.batch_start;
      return *this;
    }
    Iterator &operator++()
    {
      // batch_start += loader->batch_size;
      // loader->current_idx = batch_start;
      batch_start++;
      // if (batch_start >= loader->ptr_dataset->len())
      // {
      // batch_start = loader->ptr_dataset->len();
      // }
      return *this;
    }
    Iterator operator++(int)
    {
      Iterator iterator = *this;
      ++(*this);
      return iterator;
    }

    Batch<DType, LType> operator*() const
    {
      // size_t batch_end = std::min<size_t>(batch_start + loader->batch_size, static_cast<size_t>(loader->ptr_dataset->len()));

      // // Lấy phần tử đầu tiên để khởi tạo batchData và bacthLabel
      // DataLabel<DType, LType> first_dl = loader->ptr_dataset->getitem(loader->indices(batch_start));
      // xt::xarray<DType> batchData = first_dl.getData();
      // xt::xarray<LType> bacthLabel = first_dl.getLabel();

      // for (size_t i = batch_start + 1; i < batch_end; ++i)
      // {
      //   DataLabel<DType, LType> dl = loader->ptr_dataset->getitem(loader->indices(i));
      //   batchData = xt::concatenate(xt::xtuple(batchData, dl.getData()), 0); // Ghép theo trục 0 (batch dimension)
      //   bacthLabel = xt::concatenate(xt::xtuple(bacthLabel, dl.getLabel()), 0);
      // }

      // return Batch<DType, LType>(batchData, bacthLabel);

      // cach 2
      xt::xarray<DType> batchData;
      xt::xarray<LType> batchLabel;
      xt::svector<unsigned long> tempD_shape = this->loader->ptr_dataset->get_data_shape();
      xt::svector<unsigned long> tempL_shape = this->loader->ptr_dataset->get_label_shape();
      int num_batches = this->loader->ptr_dataset->len() / this->loader->batch_size;
      // cout << "batch_start : " << batch_start << "  ----- loader->batch_size: " << loader->batch_size << "\n";
      int start_index = this->batch_start * this->loader->batch_size;
      // int dataset_size = static_cast<int>(loader->indices.size());
      // cout << "loader->indices.size() : " << loader->indices.size() << "\n";
      // int end_idx = std::min<int>(start_index + loader->batch_size, dataset_size);
      // cout << "start_idx : " << start_index << " " << "end_idx : " << end_idx << "\n";

      // unsigned long shuffled_start_idx = this->loader->indices[start_index];

      int res = this->loader->ptr_dataset->len() % this->loader->batch_size;

      if (num_batches - 1 == this->batch_start && !this->loader->drop_last)
      {
        tempD_shape[0] = this->loader->batch_size + res;
        batchData = xt::empty<DType>(tempD_shape);
        if (tempL_shape.size() == 0)
        {
          for (int i = 0; i < this->loader->batch_size + res; i++)
          {
            xt::view(batchData, i, xt::all()) = this->loader->ptr_dataset->getitem(this->loader->indices[i + start_index]).getData();
          }
          return Batch<DType, LType>(batchData, xt::xarray<LType>());
        }
        else
        {
          tempL_shape[0] = this->loader->batch_size + res;
          batchLabel = xt::empty<LType>(tempL_shape);
          for (int i = 0; i < this->loader->batch_size + res; i++)
          {
            xt::view(batchData, i, xt::all()) = this->loader->ptr_dataset->getitem(this->loader->indices[i + start_index]).getData();
            xt::view(batchLabel, i, xt::all()) = this->loader->ptr_dataset->getitem(this->loader->indices[i + start_index]).getLabel();
          }
          return Batch<DType, LType>(batchData, batchLabel);
        }
        // batchData = xt::view(dataLabel.getData(), xt::range(start_index, this->loader->ptr_dataset->len()), xt::all());
        // bacthLabel = xt::view(dataLabel.getLabel(), xt::range(start_index, this->loader->ptr_dataset->len()), xt::all());
        // return Batch<DType, LType>(batchData, bacthLabel);
      }
      else
      {
        tempD_shape[0] = this->loader->batch_size;
        batchData = xt::empty<DType>(tempD_shape);
        if (tempL_shape.size() == 0)
        {
          for (int i = 0; i < this->loader->batch_size; i++)
          {
            xt::view(batchData, i, xt::all()) = this->loader->ptr_dataset->getitem(this->loader->indices[i + start_index]).getData();
          }
          return Batch<DType, LType>(batchData, xt::xarray<LType>());
        }
        else
        {
          tempL_shape[0] = this->loader->batch_size;
          batchLabel = xt::empty<LType>(tempL_shape);
          for (int i = 0; i < this->loader->batch_size; i++)
          {
            xt::view(batchData, i, xt::all()) = this->loader->ptr_dataset->getitem(this->loader->indices[i + start_index]).getData();
            xt::view(batchLabel, i, xt::all()) = this->loader->ptr_dataset->getitem(this->loader->indices[i + start_index]).getLabel();
          }
          return Batch<DType, LType>(batchData, batchLabel);
        }
      }
      // batchData = xt::view(dataLabel.getData(), xt::range(start_index, end_idx), xt::all());
      // bacthLabel = xt::view(dataLabel.getLabel(), xt::range(start_index, end_idx), xt::all());
      // return Batch<DType, LType>(batchData, bacthLabel);
    }
  };

  Iterator begin()
  {
    return Iterator(this, 0);
  }

  Iterator end()
  {
    // size_t end_idx = ptr_dataset->len();
    // if (drop_last && (end_idx % batch_size != 0))
    // {
    //   end_idx = (end_idx / batch_size) * batch_size;
    // }
    // return Iterator(this, end_idx);

    int batch = this->ptr_dataset->len() / this->batch_size;
    // if (!this->drop_last && this->ptr_dataset->len() < this->batch_size)
    // {
    //   batch = 1;
    // }
    return Iterator(this, batch);
  }
};

#endif /* DATALOADER_H */
