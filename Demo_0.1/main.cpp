#include <stdio.h>
#include <iostream>
#include <fstream>
#include "FileIO.h"
#include <io.h>
#include "FeatureMatch.h"
#include <opencv2\xfeatures2d\nonfree.hpp>
#include <iostream>
#include <vector>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
int main()
{
	cout << "demo" << endl;
	//��ȡĿ¼������jpg�ļ�
	FilesIO files("src/", ".jpg");
	vector<Mat> imgs = files.getImages();
	FeatureMatch matcher(imgs[0], imgs[1]);
	matcher.OrbFeatureMatch();//ƥ�䷽��
	vector<KeyPoint> l_keypoints(matcher.getKeypoints_L());
	vector<KeyPoint> r_keypoints(matcher.getKeypoints_R());
	vector<DMatch> matches(matcher.getMatches());
}
