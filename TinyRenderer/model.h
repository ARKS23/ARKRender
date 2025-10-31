#pragma once
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
    // ԭʼ������
    std::vector<vec4> verts = {};   // �洢���� "v" (��������, vec4)
    std::vector<vec4> norms = {};    // �洢���� "vn" (��������, vec4)
    std::vector<vec2> tex = {};      // �洢���� "vt" (����UV����, vec2)

    // �������飨�����ݣ�
    // .ojb �����㡢���ߡ�UV�в�ͬ����������˷ֿ��洢
    std::vector<int> facet_vrt = {}; // �洢ÿ���涥��� "��������"
    std::vector<int> facet_nrm = {}; // �洢ÿ���涥��� "��������"
    std::vector<int> facet_tex = {}; // �洢ÿ���涥��� "UV����"
    // ����Ĵ�С��nface() * 3

    // ������ͼ
    TGAImage diffusemap = {};     // ������(��ɫ)��ͼ
    TGAImage normalmap = {};      // ������ͼ
    TGAImage specularmap = {};    // �߹�(����)��ͼ

public:
    Model(const std::string filename); // ���캯��������ģ���ļ����н���
    int nverts() const; // ��ȡ "v" (����) ������
    int nfaces() const; // ��ȡ "f" (��) ������

    vec4 vert(const int i) const;                          // ��������������ȡһ������
    vec4 vert(const int iface, const int nthvert) const;   // ��"��"������"��"�ڵڼ�������(0, 1, 2)����ȡ��������
    vec4 normal(const int iface, const int nthvert) const; // �� "��" ������ "���ڵڼ�������" ��ȡ�ö���� "��������"
    vec4 normal(const vec2& uv) const;                     // ����һ�� UV ���꣬�� normalmap �в������������������
    vec2 uv(const int iface, const int nthvert) const;     // �� "��" ������ "���ڵڼ�������" ��ȡ�ö���� "UV����"

    const TGAImage& diffuse() const; // ��ȡ��������ͼ
    const TGAImage& specular() const; // ��ȡ�߹���ͼ

};