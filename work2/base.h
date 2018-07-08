#pragma once
#include "stdafx.h"
#include "FeatureMatch.h"


const double	
focal = 5.0,
u0 = 320,
v0 = 240,//u0��v0��ʾͼ����������������ͼ��ԭ����������֮�����ĺ����������������

//dx��dy��ʾ��x�����y�����һ�����طֱ�ռ���ٳ��ȵ�λ����һ�����ش����ʵ������ֵ�Ĵ�С������ʵ��ͼ����������ϵ����������ϵת���Ĺؼ���
dx = 5.312 / 640,
dy = 3.984 / 480,

fu = focal / dx,
fv = focal / dy;




const double	fx = focal * 180 / 25.4, 
				fy = focal * 180 / 25.4;

const Matx33d matx = Matx33d(fx, 0, u0,
							0, fy, v0,
							0, 0, 1);

const Mat K = Mat(matx);//�ڲξ���

const Point2d p_p(fu, fv);//��������