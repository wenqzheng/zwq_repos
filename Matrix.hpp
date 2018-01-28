//-------------------------------------------------------------------------------
// Matrix.hpp
//
// @brief
//     thread-unsafe
//
// @author
//     Millhaus.Chen @time 2017/07/27 14:21
//-------------------------------------------------------------------------------
#pragma once

#include <cstdlib>
#include <cmath>
#include <memory>
#include <utility>
#include <array>
#include <functional>

namespace mtl {

template<typename DataType, int ROW = 1, int COL = 1>
class Matrix
{
private:
template<int R, int C>
using dataType = std::array<std::array<DataType,R>,C>;

using Func = std::function<DataType(DataType)>;

constexpr auto sign(const int I) const
{
    return (I % 2) ? 1 : -1;
}

constexpr Matrix<DataType,ROW-1,COL-1> subdet(const int I,const int J) const
{
    dataType<ROW-1,COL-1> temp{{0}};
    for (int i = 0; i < I; ++i) {
        for (int j = 0; j < J; ++j)
            temp[i][j] = data[i][j];
        for (int j = J; j < COL-1; ++j)
            temp[i][j] = data[i][j + 1];
    }

    for (int i = I; i < ROW-1; ++i) {
        for (int j = 0; j < J; ++j)
            temp[i][j] = data[i + 1][j];
        for (int j = J; j < COL-1; ++j)
            temp[i][j] = data[i + 1][j + 1];
    }

    Matrix<DataType, ROW-1, COL-1> tempM(temp);
    return tempM;
}

public:
constexpr Matrix(const dataType<ROW,COL>& temp)
    :data(temp)
{}

constexpr Matrix(const Matrix<DataType,ROW,COL>& tempM)
    :data(tempM.data)
{}

constexpr DataType det() const
{
    DataType ret = 0;
    if constexpr (ROW != COL)
        ret = 0;
    if constexpr (ROW <= 0)
        ret = 0;
    if constexpr (1 == ROW)
        ret = data[0][0];
    if constexpr (ROW > 1) {
        for (int i = 0; i < ROW; ++i)
            ret += sign(i) * data[i][0] * subdet(i,0).det();
    }
    return ret;
}

constexpr Matrix<DataType,ROW,COL> foreach(const Func func) const
{
    dataType<ROW,COL> temp{{0}};
    for(int i = 0; i < ROW; ++i) {
        for(int j = 0; j < COL; ++j) {
            auto tem = data[i][j];
            temp[i][j] = func(tem);
        }
    }
    Matrix<DataType,ROW,COL> tempM(temp);
    return tempM;
}

constexpr DataType squariance() const
{
    DataType ret = 0;
    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COL; ++j)
            ret += data[i][j]*data[i][j];
    }
    return ret;
}

constexpr auto normalize_one() const
{
    auto sqc = squariance();
    dataType<ROW,COL> temp{{0}};
    for (int i = 0; i < ROW; ++i)
        for (int j = 0; j < COL; ++j)
            temp[i][j] = data[i][j]/std::sqrt(sqc);
    Matrix<DataType,ROW,COL> tempM(temp);
    return tempM;
}

public:
    constexpr int Row() { return ROW; }
    constexpr int Col() { return COL; }


public:
    dataType<ROW,COL> data;
};

}
