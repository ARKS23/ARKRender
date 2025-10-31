#pragma once
#include "tgaimage.h"
#include "geometry.h"

class Model;

class IShader
{
	/* ������࣬��������Ⱦ�����еĿɱ�̲��� */
public:
	virtual ~IShader() = default;

	virtual vec4 vertex(Model *model, int iface, int nthvert, const mat<4, 4>& ViewProjection) = 0;
	/*
		������ɫ�� Vertex Shader
		����: ģ�ͣ�ģ���ϵ�iface���棬��nthvert������, ��Ϻõ�VP����
		���: �任���"�ü��ռ�"����
	*/

	virtual bool fragment(vec3 barycentric, TGAColor& color) = 0;
	/* 
		Ƭ����ɫ�� Fragment Shader
		����: ���ص���������
		���: ���յ���ɫ�������������أ�
		����ֵ�� true����ƣ�false����
	*/
};

