#include <fstream>
#include <sstream>
#include "model.h"

Model::Model(const std::string filename) {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) {
        std::cerr << "Open the file fail" << std::endl;
        return;
    }
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) { // v开头,解析顶点("v x y z")
            iss >> trash; // 丢弃"v"
            vec4 v = { 0,0,0,1 };
            for (int i : {0, 1, 2}) iss >> v[i];
            verts.push_back(v);
        }
        else if (!line.compare(0, 3, "vn ")) { // vn开头，解析法线("vn x y z")
            iss >> trash >> trash; // 丢弃"vn"
            vec4 n;
            for (int i : {0, 1, 2}) iss >> n[i];
            norms.push_back(normalized(n)); // 需要进行单位化
        }
        else if (!line.compare(0, 3, "vt ")) { // 解析UV ("vt u v [w]")
            iss >> trash >> trash;
            vec2 uv;
            for (int i : {0, 1}) iss >> uv[i];
            tex.push_back({ uv.x, 1 - uv.y }); // ？
        }
        else if (!line.compare(0, 2, "f ")) { // 解析面 ("f v/t/n v/t/n v/t/n")
            int f, t, n, cnt = 0;
            iss >> trash;
            while (iss >> f >> trash >> t >> trash >> n) { // 将‘/‘丢弃到trash
                facet_vrt.push_back(--f); // 索引减一后存入，因为文件中的索引是从1开始的
                facet_tex.push_back(--t);
                facet_nrm.push_back(--n);
                cnt++;
            }
            if (3 != cnt) { // 保证模型是三角面组成
                std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                return;
            }
        }
    }

    // 纹理加载
    auto load_texture = [&filename](const std::string suffix, TGAImage& img) {
        size_t dot = filename.find_last_of(".");
        if (dot == std::string::npos) return;
        // 自动拼出纹理文件名 (例如 "head.obj" -> "head_diffuse.tga")
        std::string texfile = filename.substr(0, dot) + suffix;
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        };
    // 尝试加载三种标准纹理
    load_texture("_diffuse.tga", diffusemap);
    load_texture("_nm_tangent.tga", normalmap);
    load_texture("_spec.tga", specularmap);
}

int Model::nverts() const { return verts.size(); }
int Model::nfaces() const { return facet_vrt.size() / 3; } // 获取面的数量

vec4 Model::vert(const int i) const {
    return verts[i];
}

vec4 Model::vert(const int iface, const int nthvert) const { //获取第iface个面的第nthvert顶点坐标
    return verts[facet_vrt[iface * 3 + nthvert]];
}

vec4 Model::normal(const int iface, const int nthvert) const { // 模型自带的法线，精度没有法线贴图高
    return norms[facet_nrm[iface * 3 + nthvert]];
}

vec4 Model::normal(const vec2& uv) const { // 法线贴图纹理：从normal_map中获取法线
    TGAColor c = normalmap.get(uv[0] * normalmap.width(), uv[1] * normalmap.height());
    return normalized(vec4{ (double)c[2],(double)c[1],(double)c[0],0 }*2. / 255. - vec4{ 1,1,1,0 }); // RGBA数值逆映射回坐标
}

vec2 Model::uv(const int iface, const int nthvert) const {
    return tex[facet_tex[iface * 3 + nthvert]];
}

const TGAImage& Model::diffuse()  const { return diffusemap; }
const TGAImage& Model::specular() const { return specularmap; }
const TGAImage& Model::normal() const {return normalmap;}
