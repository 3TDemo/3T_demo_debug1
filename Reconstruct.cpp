#include "stdafx.h"
#include "Reconstruct.h"
#include "base.h"

void Reconstruct::reconstruct(
	cv::Mat& R1,//��һ���������ת�����ƽ������
	cv::Mat& T1,
	cv::Mat& R2,//��ת�����ƽ������
	cv::Mat& T2,
	vector<cv::Point2f>& p1,
	vector<cv::Point2f>& p2,
	vector<cv::Point3f>& structure//�������
)
{
	//���������ͶӰ����[R T]��triangulatePointsֻ֧��float��
	cv::Mat proj1(3, 4, CV_32FC1);
	cv::Mat proj2(3, 4, CV_32FC1);

	R1.convertTo(proj1(cv::Range(0, 3), cv::Range(0, 3)), CV_32FC1);
	T1.convertTo(proj1.col(3), CV_32FC1);

	R2.convertTo(proj2(cv::Range(0, 3), cv::Range(0, 3)), CV_32FC1);
	T2.convertTo(proj2.col(3), CV_32FC1);

	cv::Mat fK;
	K.convertTo(fK, CV_32FC1);
	proj1 = fK * proj1;
	proj2 = fK * proj2;

	//�����ؽ�
	cv::Mat s; triangulatePoints(proj1, proj2, p1, p2, s);

	structure.clear();
	structure.reserve(s.cols);
	for (int i = 0; i < s.cols; ++i)
	{
		cv::Mat_<float> col = s.col(i);
		col /= col(3);	//������꣬��Ҫ�������һ��Ԫ�ز�������������ֵ
		structure.push_back(cv::Point3f(col(0), col(1), col(2)));
	}
}

void Reconstruct::save_structure(string file_name, vector<cv::Mat>& rotations, vector<cv::Mat>& motions, vector<cv::Point3f>& structure, vector<cv::Vec3b>& colors)
{
	int n = (int)rotations.size();

	cv::FileStorage fs;
	fs.open(file_name, cv::FileStorage::WRITE);
	if (bool bb = fs.isOpened()) {

		fs << "Camera Count" << n;
		fs << "Point Count" << (int)structure.size();

		fs << "Rotations" << "[";
		for (size_t i = 0; i < n; ++i)
		{
			fs << rotations[i];
		}
		fs << "]";

		fs << "Motions" << "[";
		for (size_t i = 0; i < n; ++i)
		{
			fs << motions[i];
		}
		fs << "]";

		fs << "Points" << "[";
		for (size_t i = 0; i < structure.size(); ++i)
		{
			fs << structure[i];
		}
		fs << "]";

		fs << "Colors" << "[";
		for (size_t i = 0; i < colors.size(); ++i)
		{
			fs << colors[i];
		}
		fs << "]";

		fs.release();
	}
}

void Reconstruct::maskout_points(vector<cv::Point2f>& p1, cv::Mat& mask)
{
	vector<cv::Point2f> p1_copy = p1;
	p1.clear();

	for (int i = 0; i < mask.rows; ++i)
	{
		if (mask.at<uchar>(i) > 0)
			p1.push_back(p1_copy[i]);
	}
}

void Reconstruct::maskout_colors(vector<cv::Vec3b>& p1, cv::Mat& mask)
{
	vector<cv::Vec3b> p1_copy = p1;
	p1.clear();

	for (int i = 0; i < mask.rows; ++i)
	{
		if (mask.at<uchar>(i) > 0)
			p1.push_back(p1_copy[i]);
	}
}

void Reconstruct::fusion_structure(
	vector<cv::DMatch>& matches,
	vector<int>& struct_indices,
	vector<int>& next_struct_indices,
	vector<cv::Point3f>& structure,
	vector<cv::Point3f>& next_structure,
	vector<cv::Vec3b>& colors,
	vector<cv::Vec3b>& next_colors
)
{
	for (int i = 0; i < matches.size(); ++i)
	{
		int query_idx = matches[i].queryIdx;
		int train_idx = matches[i].trainIdx;

		int struct_idx = struct_indices[query_idx];
		if (struct_idx >= 0) //���õ��ڿռ����Ѿ����ڣ������ƥ����Ӧ�Ŀռ��Ӧ����ͬһ��������Ҫ��ͬ
		{
			next_struct_indices[train_idx] = struct_idx;
			continue;
		}
		
		//���õ��ڿռ����Ѿ����ڣ����õ���뵽�ṹ�У������ƥ���Ŀռ��������Ϊ�¼���ĵ������
		structure.push_back(next_structure[i]);
		colors.push_back(next_colors[i]);
		struct_indices[query_idx] = next_struct_indices[train_idx] = structure.size() - 1;
	}
}