#pragma once
#include <cmath>
#include <cassert>
#include <iostream>

template<int n> struct vec {
    double data[n] = { 0 };
    double& operator[](const int i) { assert(i >= 0 && i < n); return data[i]; }
    double  operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<int n> double operator*(const vec<n>& lhs, const vec<n>& rhs) {
    /* 向量的点积操作 */
    double ret = 0;                        
    //for (int i = n; i--; ret += lhs[i] * rhs[i]);
    for (int i = n; i >= 0; --i) ret += lhs[i] * rhs[i];
    return ret;                           
}

template<int n> vec<n> operator+(const vec<n>& lhs, const vec<n>& rhs) {
    /* 向量加法 */
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] += rhs[i]);
    return ret;
}

template<int n> vec<n> operator-(const vec<n>& lhs, const vec<n>& rhs) {
    /* 向量剑法 */
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] -= rhs[i]);
    return ret;
}

template<int n> vec<n> operator*(const vec<n>& lhs, const double& rhs) {
    /* 向量 * 数 的运算 */
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] *= rhs);
    return ret;
}

template<int n> vec<n> operator*(const double& lhs, const vec<n>& rhs) {
    /* 向量 * 数 的运算: 复用【 向量 * 数】的逻辑  */
    return rhs * lhs;
}

template<int n> vec<n> operator/(const vec<n>& lhs, const double& rhs) {
    /* 向量 / 数 的运算 */
    vec<n> ret = lhs;
    for (int i = n; i--; ret[i] /= rhs);
    return ret;
}

template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
    /* 方便cout输出 */
    for (int i = 0; i < n; i++) out << v[i] << " ";
    return out;
}

template<> struct vec<2> {
    double x = 0, y = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 2); return i ? y : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 2); return i ? y : x; }
};

template<> struct vec<3> {
    double x = 0, y = 0, z = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (1 == i ? y : z) : x; }
};

template<> struct vec<4> {
    double x = 0, y = 0, z = 0, w = 0;
    double& operator[](const int i) { assert(i >= 0 && i < 4); return i < 2 ? (i ? y : x) : (2 == i ? z : w); }
    double  operator[](const int i) const { assert(i >= 0 && i < 4); return i < 2 ? (i ? y : x) : (2 == i ? z : w); }
    vec<2> xy()  const { return { x, y }; }
    vec<3> xyz() const { return { x, y, z }; }
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;

template<int n> double norm(const vec<n>& v) {
    return std::sqrt(v * v);
}

template<int n> vec<n> normalized(const vec<n>& v) {
    return v / norm(v);
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
    return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

template<int n> struct dt;

template<int nrows, int ncols> struct mat {
    vec<ncols> rows[nrows] = { {} }; // 行向量数组，如rows[0]是一个vec<ncols>类型的向量

    vec<ncols>& operator[] (const int idx) { assert(idx >= 0 && idx < nrows); return rows[idx]; }
    const vec<ncols>& operator[] (const int idx) const { assert(idx >= 0 && idx < nrows); return rows[idx]; }

    double det() const {
        /* 计算矩阵的行列式 */
        return dt<ncols>::det(*this);
    }

    double cofactor(const int row, const int col) const {
        /* 计算代数余子式： 创建子矩阵(n - 1)×(n - 1)的子矩阵submatrix然后递归调用det(),最后根据行和列返回正负 */
        mat<nrows - 1, ncols - 1> submatrix;
        for (int i = 0; i < nrows - 1; ++i) {
            for (int j = 0; j < ncols - 1; ++j) {
                // 代数余子式行列式对应的矩阵计算源索引
                int source_i = (i < row) ? i : i + 1;
                int source_j = (j < col) ? j : j + 1;
                submatrix[i][j] = rows[source_i][source_j];
            }
        }
        return submatrix.det() * ((row + col) % 2 ? -1 : 1); // 代数余子式的正负
    }

    mat<nrows, ncols> invert_transpose() const {
        /* 计算(M^-1)^T */
        mat<nrows, ncols> adjugate_transpose; 
        // 计算伴随矩阵
        for (int i = nrows; i--; )
            for (int j = ncols; j--; adjugate_transpose[i][j] = cofactor(i, j));
        // 转置逆矩阵 = 伴随矩阵 / 行列式； 分母其实是拉普拉斯展开计算了原矩阵的行列式det(M)
        return adjugate_transpose / (adjugate_transpose[0] * rows[0]);
    }

    mat<nrows, ncols> invert() const {
        /* 计算逆矩阵，复用之前的invert_transpose，再进行transpose,就是invert了 */
        return invert_transpose().transpose();
    }

    mat<ncols, nrows> transpose() const {
        /* 转置矩阵，行列互换 */
        mat<ncols, nrows> ret;
        for (int i = ncols; i--; )
            for (int j = nrows; j--; ret[i][j] = rows[j][i]);
        return ret;
    }
};

template<int nrows, int ncols> vec<ncols> operator*(const vec<nrows>& lhs, const mat<nrows, ncols>& rhs) {
    return (mat<1, nrows>{{lhs}}*rhs)[0];
}

template<int nrows, int ncols> vec<nrows> operator*(const mat<nrows, ncols>& lhs, const vec<ncols>& rhs) {
    vec<nrows> ret;
    for (int i = nrows; i--; ret[i] = lhs[i] * rhs);
    return ret;
}

template<int R1, int C1, int C2>mat<R1, C2> operator*(const mat<R1, C1>& lhs, const mat<C1, C2>& rhs) {
    mat<R1, C2> result;
    for (int i = R1; i--; )
        for (int j = C2; j--; )
            for (int k = C1; k--; result[i][j] += lhs[i][k] * rhs[k][j]);
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator*(const mat<nrows, ncols>& lhs, const double& val) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; result[i] = lhs[i] * val);
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator/(const mat<nrows, ncols>& lhs, const double& val) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; result[i] = lhs[i] / val);
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator+(const mat<nrows, ncols>& lhs, const mat<nrows, ncols>& rhs) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; )
        for (int j = ncols; j--; result[i][j] = lhs[i][j] + rhs[i][j]);
    return result;
}

template<int nrows, int ncols>mat<nrows, ncols> operator-(const mat<nrows, ncols>& lhs, const mat<nrows, ncols>& rhs) {
    mat<nrows, ncols> result;
    for (int i = nrows; i--; )
        for (int j = ncols; j--; result[i][j] = lhs[i][j] - rhs[i][j]);
    return result;
}

template<int nrows, int ncols> std::ostream& operator<<(std::ostream& out, const mat<nrows, ncols>& m) {
    for (int i = 0; i < nrows; i++) out << m[i] << std::endl;
    return out;
}

template<int n> struct dt { // template metaprogramming to compute the determinant recursively
    static double det(const mat<n, n>& src) {
        double ret = 0;
        for (int i = n; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

template<> struct dt<1> {   // template specialization to stop the recursion
    static double det(const mat<1, 1>& src) {
        return src[0][0];
    }
};
