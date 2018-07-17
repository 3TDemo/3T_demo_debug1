#include "stdafx.h"
#include "FeatureMatch.h"


void FeatureMatch::FeatureObtatinAndMatchForAll(vector<cv::Mat>& images,
	vector<vector<cv::KeyPoint>>& key_points_for_all,
	vector<cv::Mat>& descriptor_for_all,
	vector<vector<cv::Vec3b>>& colors_for_all,
	vector<vector<cv::DMatch>>& matches_for_all)
{
	SiftFeatureMatch(images, key_points_for_all, descriptor_for_all
		//colors_for_all
	);
	//OrbFeatureMatch(images, key_points_for_all, descriptor_for_all, colors_for_all);
	matchFeatures(//image_for_all, 
		descriptor_for_all,
		matches_for_all);
}

void FeatureMatch::SiftFeatureMatch(
	vector<cv::Mat>& images,
	vector<vector<cv::KeyPoint>>& key_points_for_all,
	vector<cv::Mat>& descriptor_for_all
	//vector<vector<Vec3b>>& colors_for_all
)
{
	key_points_for_all.clear();
	descriptor_for_all.clear();
	cv::Mat image;

	//��ȡͼ�񣬻�ȡͼ�������㣬������
	cv::Ptr<cv::Feature2D> sift = cv::xfeatures2d::SIFT::create(0, 3, 0.04, 10);
	for (auto it = images.begin(); it != images.end(); ++it)
	{
		cv::Mat img = *it;
		if (img.empty()) continue;

		vector<cv::KeyPoint> key_points;
		cv::Mat descriptor;
		//ż�������ڴ����ʧ�ܵĴ���
		sift->detectAndCompute(img, cv::noArray(), key_points, descriptor);

		//��������٣����ų���ͼ��
		if (key_points.size() <= 10) continue;

		key_points_for_all.push_back(key_points);
		descriptor_for_all.push_back(descriptor);

		//vector<Vec3b> colors(key_points.size());

		//for (int i = 0; i < key_points.size(); ++i)
		//{
		//	Point2f& p = key_points[i].pt;
		//	auto color1 = img.at<Vec3b>(p.x, p.y);
		//	colors.push_back(color1);
		//}
		//colors_for_all.push_back(colors);
	}
}


void FeatureMatch::OrbFeatureMatch(
	vector<cv::Mat>& images,
	vector<vector<cv::KeyPoint>>& key_points_for_all,
	vector<cv::Mat>& descriptor_for_all,
	vector<vector<cv::Vec3b>>& colors_for_all
)
{
	key_points_for_all.clear();
	descriptor_for_all.clear();
	colors_for_all.clear();

	cv::Ptr<cv::ORB> orb = cv::ORB::create(500, 1.1f, 10, 31, 0, 2, cv::ORB::HARRIS_SCORE, 31);//(500, 1.1f, 10,  31, 0,  2, ORB::HARRIS_SCORE, 31);orb���������о���

	cv::Mat image;
	for (auto it = images.begin(); it != images.end(); ++it)
	{
		image = *it;
		if (image.empty()) continue;

		//cout << "Extracing features: " << *it << endl;

		vector<cv::KeyPoint> key_points;
		cv::Mat descriptor;

		//ż�������ڴ����ʧ�ܵĴ���
		orb->detectAndCompute(image, cv::Mat(), key_points, descriptor);

		//��������٣����ų���ͼ��
		if (key_points.size() <= 10) continue;

		key_points_for_all.push_back(key_points);
		descriptor_for_all.push_back(descriptor);

		vector<cv::Vec3b> colors(key_points.size());

		//for (int i = 0; i < key_points.size(); ++i)
		//{
		//	Point2f& p = key_points[i].pt;
		//	colors[i] = image.at<Vec3b>(p.y, p.x);
		//}
		//colors_for_all.push_back(colors);
	}
}

void
FeatureMatch::matchFeatures
(cv::Mat & query,
	cv::Mat & train,
	//	Mat& descriptors1,
	//	Mat& descriptors2,
	vector<cv::DMatch>& matches)
{
	//Ptr<FeatureDetector> detector = ORB::create();
	//Ptr<DescriptorExtractor> descriptor = ORB::create();
	//Ptr<DescriptorMatcher> matcher_1 = DescriptorMatcher::create("BruteForce-Hamming");
	////��� Oriented FAST �ǵ�λ��
	//vector<KeyPoint> l_keyPoints;
	//vector<KeyPoint> r_keyPoints;
	//detector->detect(query, l_keyPoints);
	//detector->detect(train, r_keyPoints);
	////���ݽǵ�λ�ü��� BRIEF ������
	//descriptor->compute(query, l_keyPoints, descriptors1);
	//descriptor->compute(query, r_keyPoints, descriptors2);
	////������ͼ���е�BRIEF�����ӽ���ƥ�䣬ʹ�� Hamming ����
	//vector<DMatch> matches_1;
	////BFMatcher matcher ( NORM_HAMMING );
	//matcher_1->match(descriptors1, descriptors2, matches_1);
	////ƥ����ɸѡ
	//double min_dist = 1000, max_dist = 10;
	////�ҳ�����ƥ��֮�����С�����������, ���������Ƶĺ�����Ƶ������֮��ľ���
	//for (int i = 0; i < descriptors1.rows; i++)
	//{
	//	double dist = matches_1[i].distance;
	//	if (dist < min_dist) min_dist = dist;
	//	if (dist > max_dist) max_dist = dist;
	//}
	////ɸѡ
	//for (int i = 0; i < descriptors1.rows; i++)
	//{
	//	if (matches_1[i].distance <= max(2 * min_dist, 20.0))
	//	{
	//		matches.push_back(matches_1[i]);
	//	}
	//}
	vector<vector<cv::DMatch>> knn_matches;
	cv::BFMatcher matcher(cv::NORM_L2);
	matcher.knnMatch(query, train, knn_matches, 2);

	//��ȡ����Ratio Test����Сƥ��ľ���
	float min_dist = FLT_MAX;
	for (int r = 0; r < knn_matches.size(); ++r)
	{
		//Ratio Test
		if (knn_matches[r][0].distance > 0.6*knn_matches[r][1].distance)
			continue;

		float dist = knn_matches[r][0].distance;
		if (dist < min_dist) min_dist = dist;
	}

	matches.clear();
	for (size_t r = 0; r < knn_matches.size(); ++r)
	{
		//�ų�������Ratio Test�ĵ��ƥ��������ĵ�
		if (
			knn_matches[r][0].distance > 0.6*knn_matches[r][1].distance ||
			knn_matches[r][0].distance > 5 * max(min_dist, 10.0f)
			)
			continue;
		//����ƥ���
		matches.push_back(knn_matches[r][0]);
	}
}

void
FeatureMatch::matchFeatures
(
	vector<cv::Mat>& descriptor_for_all,
	vector<vector<cv::DMatch>>& matches_for_all
)
{
	matches_for_all.clear();
	// n��ͼ������˳���� n-1 ��ƥ��
	for (int i = 0; i < descriptor_for_all.size() - 1; ++i)
	{
		cout << "Matching images " << i << " - " << i + 1 << endl;

		vector<cv::DMatch> matches;

		matchFeatures(descriptor_for_all[i], descriptor_for_all[i + 1], matches);

		//����n-1��matches���
		matches_for_all.push_back(matches);
	}
}
