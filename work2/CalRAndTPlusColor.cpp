#include "stdafx.h"
#include "CalRAndTPlusColor.h"

void CalRAndTPlusColor::loadMat()
{
	FilesIO files("src/", ".jpg");
	this->images_for_all = files.getImages();
}

void CalRAndTPlusColor::match_all()
{
	//��ȡ����ͼ�������
	//������ͼ�����˳�ε�����ƥ��
	FeatureMatch::FeatureObtatinAndMatchForAll(images_for_all, key_points_for_all, descriptor_for_all, colors_for_all, matches_for_all);

}



void CalRAndTPlusColor::init_and_add() {
	//��ʼ���ṹ����ά���ƣ�
	Structure::init_structure(
		key_points_for_all,
		colors_for_all,
		matches_for_all,

		structure,
		correspond_struct_idx,
		colors,//ɸѡ��ɫ
		rotations,
		motions
	);
	for (int i = 1; i < matches_for_all.size(); ++i)
	{
		vector<Point3f> object_points;
		vector<Point2f> image_points;
		Mat r, R, T;
		//Mat mask;

		//��ȡ��i��ͼ����ƥ����Ӧ����ά�㣬�Լ��ڵ�i+1��ͼ���ж�Ӧ�����ص�
		Transform::get_objpoints_and_imgpoints(
			matches_for_all[i],
			correspond_struct_idx[i],
			structure,
			key_points_for_all[i + 1],

			object_points,
			image_points
		);


		//���任����
		solvePnPRansac(object_points, image_points, K, noArray(), r, T);
		//����ת����ת��Ϊ��ת����
		Rodrigues(r, R);

		//����任����
		rotations.push_back(R);
		motions.push_back(T);

		vector<Point2f> p1, p2;
		vector<Vec3b> c1, c2;
		Transform::get_matched_points(key_points_for_all[i], key_points_for_all[i + 1], matches_for_all[i], p1, p2);
		Transform::get_matched_colors(colors_for_all[i], colors_for_all[i + 1], matches_for_all[i], c1, c2);

		//����֮ǰ��õ�R��T������ά�ؽ�
		vector<Point3f> next_structure;
		Reconstruct::reconstruct(rotations[i], motions[i], R, T, p1, p2, next_structure);

		//���µ��ؽ������֮ǰ���ں�
		Reconstruct::fusion_structure(
			matches_for_all[i],
			correspond_struct_idx[i],
			correspond_struct_idx[i + 1],
			structure,
			next_structure,
			colors,
			c1
		);
	}
}

void CalRAndTPlusColor::save()
{
	Reconstruct::save_structure(".\\Viewer\\structure.yml", rotations, motions, structure, colors);
}

CalRAndTPlusColor::CalRAndTPlusColor()
{
	loadMat();
	match_all();
	init_and_add();
}
