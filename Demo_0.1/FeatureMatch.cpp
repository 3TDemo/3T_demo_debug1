#include "FeatureMatch.h"
void FeatureMatch::OrbFeatureMatch()
{
	if (!img1.data || !img2.data)
	{
		cout << "error reading images " << endl;
	}
	Ptr<FeatureDetector> detector = ORB::create();
	Ptr<DescriptorExtractor> descriptor = ORB::create();
	Ptr<DescriptorMatcher> matcher_1 = DescriptorMatcher::create("BruteForce-Hamming");
	Ptr<ORB> orb = ORB::create();//(500, 1.1f, 10,  31, 0,  2, ORB::HARRIS_SCORE, 31);orb���������о���
	Mat descriptors1, descriptors2;//������
	orb->detectAndCompute(img1, Mat(), l_keypoints, descriptors1);
	orb->detectAndCompute(img2, Mat(), r_keypoints, descriptors2);
	BFMatcher matcher(NORM_L2);
	matcher.match(descriptors1, descriptors2, matches);
	//���ӻ�����ʾ�ؼ���
	Mat ShowKeypoints1, ShowKeypoints2;
	drawKeypoints(img1, l_keypoints, ShowKeypoints1);
	drawKeypoints(img2, r_keypoints, ShowKeypoints2);
	imshow("l_keypoints", ShowKeypoints1);
	waitKey(0);
	imshow("r_keypoints", ShowKeypoints2);
	waitKey(0);

	//��� Oriented FAST �ǵ�λ��
	detector->detect(img1, l_keypoints);
	detector->detect(img2, r_keypoints);
	//���ݽǵ�λ�ü��� BRIEF ������
	descriptor->compute(img1, l_keypoints, descriptors1);
	descriptor->compute(img2, r_keypoints, descriptors2);
	//������ͼ���е�BRIEF�����ӽ���ƥ�䣬ʹ�� Hamming ����
	vector<DMatch> matches_1;
	//BFMatcher matcher ( NORM_HAMMING );
	matcher_1->match(descriptors1, descriptors2, matches_1);
	//ƥ����ɸѡ
	double min_dist = 1000, max_dist = 10;
	//�ҳ�����ƥ��֮�����С�����������, ���������Ƶĺ�����Ƶ������֮��ľ���
	for (int i = 0; i < descriptors1.rows; i++)
	{
		double dist = matches_1[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	//ɸѡ
	vector< DMatch > f_matches;
	for (int i = 0; i < descriptors1.rows; i++)
	{
		if (matches_1[i].distance <= max(2 * min_dist, 20.0))
		{
			f_matches.push_back(matches_1[i]);
		}
	}
	//����ƥ����
	Mat img_f_match;
	drawMatches(img1, l_keypoints, img2, r_keypoints, f_matches, img_f_match);
	imshow("ƥ����", img_f_match);
	waitKey(0);
}
