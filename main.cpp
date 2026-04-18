#include "src/Matrix.hpp"
#include "src/Matrix_io.hpp"

int main() {
    Matrix<int> m(2, 2);
    m.Set(0,0,1);
    m.Print();
    return 0;
}
