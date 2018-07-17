#include "stdafx.h"
#include "Transform.h"
#include "base.h"


bool Transform::find_transform(
	vector<cv::Point2f>& p1,//��һ��ͼƬ��n����ά���ص�
	vector<cv::Point2f>& p2,
	cv::Mat& R,//��ת����
	cv::Mat& T, //ƽ�ƾ���
	cv::Mat& mask)
{
	//����ƥ�����ȡ2��ͼ��ı�������ʹ��RANSAC����һ���ų�ʧ���
	cv::Mat E = findEssentialMat(p1, p2, focal, p_p, cv::RANSAC, 0.999, 1.0, mask);
	if (E.empty()) return false;

	double feasible_count = countNonZero(mask);
	cout << (int)feasible_count << " -in- " << p1.size() << endl;
	
	//����RANSAC���ԣ�outlier��������50%ʱ������ǲ��ɿ���
	//7.16�Ķ� if (feasible_count <= 15 || (feasible_count / p1.size()) < 0.3)
		//return false;

	//�ֽⱾ�����󣬻�ȡ��Ա任
	int pass_count = recoverPose(E, p1, p2, R, T, focal, p_p, mask);

	//ͬʱλ���������ǰ���ĵ������Ҫ�㹻��
	if (((double)pass_count) / feasible_count < 0.7)
		return false;

	return true;
};

void Transform::get_matched_points(//����ƥ�䵽��keyPoints��λ��
	vector<cv::KeyPoint>& p1,
	vector<cv::KeyPoint>& p2,
	vector<cv::DMatch> matches,
	vector<cv::Point2f>& out_p1,
	vector<cv::Point2f>& out_p2)
{
	out_p1.clear();
	out_p2.clear();
	for (int i = 0; i < matches.size(); ++i)
	{
		//qeuryIdx -> �ǲ���ͼ�����������������descriptor�����±꣬ͬʱҲ����������Ӧ�����㣨keypoint)���±�
		//trainIdx�C> ������ͼ������������������±꣬ͬ��Ҳ����Ӧ����������±ꡣ
		out_p1.push_back(p1[matches[i].queryIdx].pt);
		out_p2.push_back(p2[matches[i].trainIdx].pt);
	}
}

void Transform::get_matched_colors(//����ƥ�䵽��keyPoints����ɫ��Vec3b
	/*vector<Vec3b>& c1,
	vector<Vec3b>& c2,*/

	cv::Mat& image,
	vector<cv::KeyPoint>& keypoints,
	vector<cv::DMatch> matches,
	vector<cv::Vec3b>& out

	//vector<Vec3b>& out_c2
)
{
	out.clear();
	out.resize(matches.size());;
	for (int i = 0; i < matches.size(); ++i)
	{
		int x = keypoints[matches[i].queryIdx].pt.x;
		int y = keypoints[matches[i].queryIdx].pt.y;
		if(x < image.cols && y < image.rows)
			out.push_back(image.at<cv::Vec3b>(y,x));
		//out_c2.push_back(c2[matches[i].trainIdx]);//��ɫӦ����ͬ
	}
	//for (int row = 0; row != image.rows; ++row) {
	//	auto ptr = image.ptr<uchar>(row);
	//	for (int col = 0; col != image.cols; ++col) {
	//		//the channels of openCV is BGR but not RGB
	//		/*out.push_back(Vec3b(ptr[2], ptr[1], ptr[0]));*/
	//		cout << row << ", " << col << ", " << ptr[2] << ", " << ptr[1] << ", " << ptr[0] << std::endl;
	//	}
	//}
}

void Transform::get_objpoints_and_imgpoints(
	vector<cv::DMatch>& matches,
	vector<int>& struct_indices,
	vector<cv::Point3f>& structure,

	vector<cv::KeyPoint>& key_points,

	vector<cv::Point3f>& object_points,
	vector<cv::Point2f>& image_points)
{
	object_points.clear();
	image_points.clear();

	for (int i = 0; i < matches.size(); ++i)
	{

		int query_idx = matches[i].queryIdx;
		int train_idx = matches[i].trainIdx;

		int struct_idx = struct_indices[query_idx];

		if (struct_idx < 0) continue;

		object_points.push_back(structure[struct_idx]);
		image_points.push_back(key_points[train_idx].pt);
	}
}