#include "stdafx.h"
#include "Structure.h"

void Structure::init_structure(
	vector<vector<cv::KeyPoint>>& key_points_for_all,
	/*vector<vector<Vec3b>>& colors_for_all,*/
	vector<cv::Mat>& images_for_all,
	vector<vector<cv::DMatch>>& matches_for_all,

	vector<cv::Point3f>& structure,//��������������������
	vector<vector<int>>& correspond_struct_idx,//2��ͼ��Ľṹ����
	vector<cv::Vec3b>& colors,
	vector<cv::Mat>& rotations,
	vector<cv::Mat>& motions
)
{
	//����ͷ����ͼ��֮��ı任����
	vector<cv::Point2f> p1, p2;
	//vector<Vec3b> c2;//������
	cv::Mat R, T;	//��ת�����ƽ������

	cv::Mat mask;	//mask�д�����ĵ����ƥ��㣬���������ʧ���
	Transform::get_matched_points(key_points_for_all[0], key_points_for_all[1], matches_for_all[0], p1, p2);
	Transform::get_matched_colors(images_for_all[0], key_points_for_all[0], matches_for_all[0], colors);

	Transform::find_transform(p1, p2, R, T, mask);

	//��ͷ����ͼ�������ά�ؽ�
	//�ų���ƥ��ĵ�
	Mask::maskout_points(p1, mask);
	Mask::maskout_points(p2, mask);
	Mask::maskout_colors(colors, mask);

	//��һ���������Ϊ��������
	cv::Mat R0 = cv::Mat::eye(3, 3, CV_64FC1);//����淶��
	cv::Mat T0 = cv::Mat::zeros(3, 1, CV_64FC1);

	Reconstruct::reconstruct(R0, T0, R, T, p1, p2, structure);

	//����ÿһ����������ǰһ������ı任����//��ʼ������2�� n-1+1
	rotations = { R0, R };
	motions = { T0, T };

	//��correspond_struct_idx�Ĵ�С��ʼ��Ϊ��key_points_for_all��ȫһ��
	correspond_struct_idx.clear();
	correspond_struct_idx.resize(key_points_for_all.size());//n�����n���ṹ����
	for (int i = 0; i < key_points_for_all.size(); ++i)//
	{
		correspond_struct_idx[i].resize(key_points_for_all[i].size(), -1);//
	}

	//��дͷ����ͼ��Ľṹ����
	int idx = 0;
	vector<cv::DMatch>& matches = matches_for_all[0];
	for (int i = 0; i < matches.size(); i++)
	{
		if (mask.at<uchar>(i) == 0) {
			continue;
		}
		//matches[i].queryIdxΪkeyPoints��matches�������
		correspond_struct_idx[0][matches[i].queryIdx] = idx;
		correspond_struct_idx[1][matches[i].trainIdx] = idx;
		++idx;
	}
	//for (int i = 0; i < matches_for_all.size(); i++) {
	//	vector<DMatch>& matches = matches_for_all[i];
	//	for (int j = 0; j < matches.size(); i++)
	//	{
	//		if (mask.at<uchar>(j) == 0) {
	//			cout << "skip" << endl;
	//			continue;
	//		}
	//		//matches[i].queryIdxΪkeyPoints��matches�������
	//		correspond_struct_idx[i][matches[j].queryIdx] = idx;
	//		correspond_struct_idx[i + 1][matches[j].trainIdx] = idx;
	//		++idx;
	//	}
	//}
}