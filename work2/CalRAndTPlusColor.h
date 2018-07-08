#pragma once
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "FileIO.h"
#include <io.h>
#include "FeatureMatch.h"
#include <opencv2\xfeatures2d\nonfree.hpp>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Structure.h"
#include "base.h"
#include "FeatureMatch.h"

class CalRAndTPlusColor
{
private:
	vector<Mat> images_for_all;
	vector<vector<KeyPoint>> key_points_for_all;
	vector<Mat> descriptor_for_all;//����ƥ���������ͼ��������
	vector<vector<Vec3b>> colors_for_all;
	vector<vector<DMatch>> matches_for_all;

	//��
	vector<Point3f> structure;
	//�����i��ͼ���е�j���������Ӧ��structure�е������
	vector<vector<int>> correspond_struct_idx;
	//ƥ�����ɫ�洢����
	vector<Vec3b> colors;
	//��ת��������
	vector<Mat> rotations;
	//ƽ�ƾ�������
	vector<Mat> motions;

	void loadMat();
	void match_all();
	void init_and_add();

public:
	void save();
	CalRAndTPlusColor();

	vector<Mat>& getRotation() {
		return rotations;
	}//��ת����

	vector<Mat>& getMotion() {
		return motions;
	}//ƽ�ƾ���

	vector<Vec3b>& getColor() {
		return colors;
	}//��ɫ����

	static const Mat& getInnerMat() {
		return K;
	}//�ڲξ���

	static const double getFocal() {
		return focal * 180 / 25.4;
	}//����

	static vector<int>& getK1And1AndK2() {
		vector<int> kp;
		kp.resize(2);
		for (int i = 0; i < 2; i++) {
			kp.push_back(0);
		}
		return kp;
	}//�������k1��k2

	static vector<int>& getP1AndP2() {
		vector<int> p;
		p.resize(2);
		for (int i = 0; i < 2; i++) {
			p.push_back(0);
		}
		return p;
	}//�������p1��p2

	vector<Point3f>& getStructure() {

		return structure;
	}//ÿ��������������

	vector<vector<int>>& getCorrespondStru() {
		vector<vector<int>> matchStrut(matches_for_all.size());
		for (int i = 0; i < matches_for_all.size(); i++) {
			vector<int> temp(2);
			temp.push_back(i);
			temp.push_back(i + 1);
			matchStrut.push_back(temp);
		}
		return matchStrut;
	}//����ÿ��ƥ����Ӧ��������

	vector<vector<vector<Point2f>>>& getMatchesPts() {
		vector<vector<vector<Point2f>>> matchPt(matches_for_all.size());
		for (int i = 0; i < matches_for_all.size(); i++) {
			vector<vector<Point2f>> temp1;
			for (int j = 0; j < matches_for_all[i].size(); j++) {
				vector<Point2f> temp2;
				temp2.push_back(key_points_for_all[i][matches_for_all[i][j].queryIdx].pt);
				temp2.push_back(key_points_for_all[i][matches_for_all[i][j].trainIdx].pt);

				temp1.push_back(temp2);
			}
			matchPt.push_back(temp1);
		}
		return matchPt;
	}//����ÿ��ƥ����Ӧÿ��ͼƬ�е����ص�

	vector<vector<Mat>>& getPos() {
		vector<vector<Mat>> posMat;
		for (int i = 0; i < matches_for_all.size(); i++) {
			vector<Mat> pos;
			pos.push_back(Mat());
		}
		return posMat;
	}//����ÿ��ƥ���Ŀռ�����
};