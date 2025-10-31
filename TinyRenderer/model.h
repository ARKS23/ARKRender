#pragma once
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
    // 原始数据组
    std::vector<vec4> verts = {};   // 存储所有 "v" (顶点坐标, vec4)
    std::vector<vec4> norms = {};    // 存储所有 "vn" (法线向量, vec4)
    std::vector<vec2> tex = {};      // 存储所有 "vt" (纹理UV坐标, vec2)

    // 索引数组（面数据）
    // .ojb 允许顶点、法线、UV有不同的索引，因此分开存储
    std::vector<int> facet_vrt = {}; // 存储每个面顶点的 "顶点索引"
    std::vector<int> facet_nrm = {}; // 存储每个面顶点的 "法线索引"
    std::vector<int> facet_tex = {}; // 存储每个面顶点的 "UV索引"
    // 数组的大小是nface() * 3

    // 纹理贴图
    TGAImage diffusemap = {};     // 漫反射(颜色)贴图
    TGAImage normalmap = {};      // 法线贴图
    TGAImage specularmap = {};    // 高光(镜面)贴图

public:
    Model(const std::string filename); // 构造函数，传入模型文件进行解析
    int nverts() const; // 获取 "v" (顶点) 的总数
    int nfaces() const; // 获取 "f" (面) 的总数

    vec4 vert(const int i) const;                          // 按顶点索引来获取一个顶点
    vec4 vert(const int iface, const int nthvert) const;   // 按"面"索引和"面"内第几个顶点(0, 1, 2)来获取顶点坐标
    vec4 normal(const int iface, const int nthvert) const; // 按 "面" 索引和 "面内第几个顶点" 获取该顶点的 "法线向量"
    vec4 normal(const vec2& uv) const;                     // 给定一个 UV 坐标，从 normalmap 中采样并解码出法线向量
    vec2 uv(const int iface, const int nthvert) const;     // 按 "面" 索引和 "面内第几个顶点" 获取该顶点的 "UV坐标"

    const TGAImage& diffuse() const; // 获取漫反射贴图
    const TGAImage& specular() const; // 获取高光贴图

};