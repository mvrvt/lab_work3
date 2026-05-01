#pragma once

#include <iostream>
#include "IMatrix.hpp"

template <typename T>
void PrintMatrix( std::ostream& os, const IMatrix<T>& mat ) {
    for ( size_t i = 0; i < mat.GetRows(); ++i ) {
        os << "[";
        for ( size_t j = 0; j < mat.GetCols(); ++j ) {
            os << mat.Get( i, j );
            if ( j + 1 < mat.GetCols() ) os << " ";
        }
        os << "]" << std::endl;
    }
}

template <typename T>
std::ostream& operator<<( std::ostream& os, const IMatrix<T>& mat ) {
    PrintMatrix( os, mat );
    return os;
}
