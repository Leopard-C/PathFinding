#ifndef UTILS_H
#define UTILS_H

namespace util {

/*   Create 2D Array
***/
template<typename T>
T** New2D(int width, int height, const T& init_val) {
    T** p = new T*[height];
    for (int i = 0; i < height; ++i) {
        p[i] = new T[width];
        for (int j = 0; j < width; ++j) {
            p[i][j] = init_val;
        }
    }
    return p;
}

/*   Destroy 2D Array
***/
template<typename T>
void Delete2D(T**& p, int/* width*/, int height) {
    for (int i = 0; i < height; ++i) {
        delete[] p[i];
    }
    delete[] p;
    p = nullptr;
}

}

#endif // UTILS_H
