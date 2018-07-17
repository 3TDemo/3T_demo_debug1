#include"stdafx.h"
#include"cmvsIO.h"
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <windows.h>
#include "CalRAndTPlusColor.h"
#include <direct.h>
#include<cstring>
#include<string>
#include<tchar.h>
#include<fstream>
#include"pointsOut.h"

using namespace std;

//����cmvs�����Ŀ¼�ṹ
void cmvsIO::prepareDir(string rootdir) {
	//string root = "./cmvs";
	string cmvs = rootdir + "\\cmvs";
	string pmvs = rootdir + "cmvs\\pmvs";
	string imagedir = rootdir + "\\cmvs\\pmvs\\visualize";
	string txtdir = rootdir + "\\cmvs\\pmvs\\txt";
	string modeldir = rootdir + "\\cmvs\\pmvs\\models";
	bool flag0 = CreateDirectory(cmvs.c_str(), NULL);
	bool flag1 = CreateDirectory(pmvs.c_str(), NULL);
	bool imageflag = CreateDirectory(imagedir.c_str(), NULL);
	bool txtflag = CreateDirectory(txtdir.c_str(), NULL);
	bool molflag = CreateDirectory(modeldir.c_str(), NULL);
	if (imageflag == true && txtflag == true && molflag == true) {
		cout << "Directory created" << endl;
	}
	string fn1 = cmvs + "\\cmvs.exe";
	string fn2 = cmvs + "\\genOption.exe";
	string fn3 = cmvs + "\\msvcp100.dll";
	string fn4 = cmvs + "\\msvcr100.dll";
	string fn5 = cmvs + "\\pmvs2.exe";
	string fn6 = cmvs + "\\ws2_32.dll";
	CopyFile("./cmvs/cmvs.exe", fn1.c_str(), TRUE);
	CopyFile("./cmvs/genOption.exe", fn2.c_str(), TRUE);
	CopyFile("./cmvs/msvcp100.dll", fn3.c_str(), TRUE);
	CopyFile("./cmvs/msvcr100.dll", fn4.c_str(), TRUE);
	CopyFile("./cmvs/pmvs2.exe", fn5.c_str(), TRUE);
	CopyFile("./cmvs/ws2_32.dll", fn6.c_str(), TRUE);
};

//��ͼƬ�ļ�����ʽ���Ƶ�Ŀ¼��
/*void cmvsIO::prepareImage(string filename) {
string copy = "./cmvs/pmvs/visualize";
};
*/

//����CalRAndʵ��
CalRAndTPlusColor cmvsIO::call() {
	CalRAndTPlusColor cal = CalRAndTPlusColor();
	return cal;
}

//����P����
cv::Mat cmvsIO::getMatP(cv::Mat inner, cv::Mat rotation, cv::Mat motion) {
	cv::Mat result;
	cv::Mat outer;
	cv::hconcat(rotation, motion, outer);
	result = inner * outer;
	//int a = result.type();
	//cout << a << endl;
	return result;
};
//������Ҫ���ļ�
void cmvsIO::prepareFiles(string rootdir) {
	int imageNum = 0;
	int pointNum = 0;
	string txtdir = rootdir + "\\cmvs\\pmvs\\txt";
	string imgdir = rootdir + "\\cmvs\\pmvs\\visualize";
	CalRAndTPlusColor cratpc = CalRAndTPlusColor();
	vector<point3d> points = cratpc.getPoints();
	vector<cv::Mat> images = cratpc.getImages();
	vector<cv::Mat> rotation = cratpc.getRotation();       //�����ת����
	vector<cv::Mat> motion = cratpc.getMotion();       //���ƽ�ƾ���
	vector<int> ks = cratpc.getK1And1AndK2();          //����������k1,k2
	vector<cv::Point3f> cam3d = cratpc.getStructure();          //����ռ�����
	double focal = cratpc.getFocal();      //��ȡ�������
	cv::Mat inner = CalRAndTPlusColor::getInnerMat();
	cv::Mat rot;
	cv::Mat mot;
	cv::Mat P;
	imageNum = rotation.size();   //�����ͼƬ����
	pointNum = points.size();     //���Ƶ㣨ƥ��㣩����
	
	int k1 = 0;      //�������k1
	int k2 = 0;      //�������k2
	                     /*----------------��ͼƬ���Ƶ�visualize�ļ�����---------------------*/
						 /*----------------����������Ƭ��Ϣ��txt�ļ�-------------------------*/
	for (int i = 0; i < imageNum; i++) {
		//�ļ���
		string num;
		
		if (i <= 9) num = "0000000" + to_string(i);
		
		else if (i <= 99) num = "000000" + to_string(i);

		else num = "00000" + to_string(i);
		
		string txtname = num + ".txt";
		string imgname = num + ".jpg";          //ע�⣺txt�ļ���δ��
		//�ļ���ַ
		string txt = rootdir + "\\cmvs\\pmvs\\txt\\" + txtname;
		string imgdir = rootdir + "\\cmvs\\pmvs\\visualize\\" + imgname;
		cv::imwrite(imgdir, images[i]);
		rot = rotation[i];
		mot = motion[i];
		P = getMatP(inner, rot, mot);
		ofstream txtIO;
		txtIO.open(txt);
		txtIO << "CONTOUR" << endl;
		for (int n = 0; n < 3; n++) {
			txtIO << P.at<double>(n, 0) << "\t" << P.at<double>(n, 1) << "\t" << P.at<double>(n, 2) << "\t" << P.at<double>(n, 3) << endl;
		}
		txtIO.close();

	}
	/*--------------------------------����bundle.rd.out--------------------------------------*/
	ofstream bundleIO;
	string bundledir = rootdir + "\\cmvs\\pmvs\\bundle.rd.out";
	/*�����������*/
	bundleIO.open(bundledir);
	bundleIO << "# Bundle file v0.3" << endl << imageNum << "\t" << pointNum << endl;
	for (int i = 0; i < imageNum; i++) {
		bundleIO << focal << "\t" << k1 << "\t" << k2 << endl;
		cv::Mat rot = rotation[i];
		cv::Mat mot = motion[i];
		for (int n = 0; n < 3; n++) {
			bundleIO << rot.at<double>(n, 0) << "\t" << rot.at<double>(n, 1) << "\t" << rot.at<double>(n, 2) << endl;
		}
		bundleIO << mot.at<double>(0, 0) << "\t" << mot.at<double>(0, 1) << "\t" << mot.at<double>(0, 2) << endl;
	}
	/*����ƥ�������*/
	for (int i = 0; i < pointNum; i++) {
		point3d pointinfo = points[i];      //information of this point
		cv::Vec3b color = pointinfo.color;      //color of this point
		vector<point2d> matchpoints = pointinfo.points2d;      //match points of this point
		int matchnumber = matchpoints.size();
		cv::Point3f position = pointinfo.pt3;           //3d position of this point
		bundleIO << position.x << "\t" << position.y << "\t" << position.z << endl;
		bundleIO << int(color[2]) << "\t" << int(color[1]) << "\t" << int(color[0]) << endl << matchnumber << "\t";
		for (int n = 0; n < matchnumber - 1; n++) {
			point2d point = matchpoints[n];
			bundleIO << point.camera << "\t" << point.pointIdx << "\t" << point.pt2.x << "\t" << point.pt2.y << "\t";
		}
		point2d last = matchpoints[matchnumber - 1];
		bundleIO << last.camera << "\t" << last.pointIdx << "\t" << last.pt2.x << "\t" << last.pt2.y << endl;
	}
	bundleIO.close();
};

//void cmvsIO::changeFiles(string rootdir) {
//	int imageNum = 0;
//	int pointNum = 0;
//	string txtdir = rootdir + "\\cmvs\\pmvs\\txt";
//	string imgdir = rootdir + "\\cmvs\\pmvs\\visualize";
//	CalRAndTPlusColor cratpc = CalRAndTPlusColor();
//	vector<point3d> points = cratpc.getPoints();
//	vector<cv::Mat> images = cratpc.getImages();
//	vector<cv::Mat> rotation = cratpc.getRotation();       //�����ת����
//	vector<cv::Mat> motion = cratpc.getMotion();       //���ƽ�ƾ���
//	vector<int> ks = cratpc.getK1And1AndK2();          //����������k1,k2
//	vector<cv::Point3f> cam3d = cratpc.getStructure();          //����ռ�����
//	double focal = cratpc.getFocal();      //��ȡ�������
//	cv::Mat inner = CalRAndTPlusColor::getInnerMat();
//	cv::Mat rot;
//	cv::Mat mot;
//	cv::Mat P;
//	imageNum = rotation.size();   //�����ͼƬ����
//	pointNum = points.size();     //���Ƶ㣨ƥ��㣩����
//
//	int k1 = 0;      //�������k1
//	int k2 = 0;      //�������k2
//					 /*----------------��ͼƬ���Ƶ�visualize�ļ�����---------------------*/
//					 /*----------------����������Ƭ��Ϣ��txt�ļ�-------------------------*/
//	for (int i = 1; i < imageNum; i++) {
//		//�ļ���
//		string num;
//
//		if (i <= 10) num = "0000000" + to_string(i-1);
//
//		else if (i <= 100) num = "000000" + to_string(i-1);
//
//		else num = "00000" + to_string(i-1);
//
//		string txtname = num + ".txt";
//		string imgname = num + ".jpg";          //ע�⣺txt�ļ���δ��
//												//�ļ���ַ
//		string txt = rootdir + "\\cmvs\\pmvs\\txt\\" + txtname;
//		string imgdir = rootdir + "\\cmvs\\pmvs\\visualize\\" + imgname;
//		cv::imwrite(imgdir, images[i]);
//		rot = rotation[i];
//		mot = motion[i];
//		P = getMatP(inner, rot, mot);
//		ofstream txtIO;
//		txtIO.open(txt);
//		txtIO << "CONTOUR" << endl;
//		for (int n = 0; n < 3; n++) {
//			txtIO << P.at<double>(n, 0) << "\t" << P.at<double>(n, 1) << "\t" << P.at<double>(n, 2) << "\t" << P.at<double>(n, 3) << endl;
//		}
//
//		txtIO.close();
//
//	}
//	/*--------------------------------����bundle.rd.out--------------------------------------*/
//	ofstream bundleIO;
//	string bundledir = rootdir + "\\cmvs\\pmvs\\bundle.rd.out";
//	/*�����������*/
//	bundleIO.open(bundledir);
//	bundleIO << "# Bundle file v0.3" << endl << imageNum - 1 << "\t" << pointNum << endl;
//	for (int i = 1; i < imageNum; i++) {
//		bundleIO << focal << "\t" << k1 << "\t" << k2 << endl;
//		cv::Mat rot = rotation[i];
//		cv::Mat mot = motion[i];
//		for (int n = 0; n < 3; n++) {
//			bundleIO << rot.at<double>(n, 0) << "\t" << rot.at<double>(n, 1) << "\t" << rot.at<double>(n, 2) << endl;
//		}
//		bundleIO << mot.at<double>(0, 0) << "\t" << mot.at<double>(0, 1) << "\t" << mot.at<double>(0, 2) << endl;
//	}
//	/*����ƥ�������*/
//	for (int i = 0; i < pointNum; i++) {
//		point3d pointinfo = points[i];      //information of this point
//		cv::Vec3b color = pointinfo.color;      //color of this point
//		vector<point2d> matchpoints = pointinfo.points2d;      //match points of this point
//		int matchnumber = matchpoints.size();
//		cv::Point3f position = pointinfo.pt3;           //3d position of this point
//		bundleIO << position.x << "\t" << position.y << "\t" << position.z << endl;
//		bundleIO << int(color[2]) << "\t" << int(color[1]) << "\t" << int(color[0]) << endl << matchnumber << "\t";
//		for (int n = 0; n < matchnumber - 1; n++) {
//			point2d point = matchpoints[n];
//			if (point.camera > 0) {
//				bundleIO << point.camera - 1 << "\t" << point.pointIdx << "\t" << point.pt2.x << "\t" << point.pt2.y << "\t";
//			}
//		}
//		point2d last = matchpoints[matchnumber - 1];
//		bundleIO << last.camera << "\t" << last.pointIdx << "\t" << last.pt2.x << "\t" << last.pt2.y << endl;
//	}
//	bundleIO.close();
//};
//����bundle.rd.out
/*void cmvsIO::prepareBundle(CalRAndTlusColor cratpc) {
vector<Mat> rotate = cratpc.getRotation();
string dir = "./cmvs/pmvs";
vector<vector<vector<Point2d>>> matchPoints = cratpc.getMatchesPts();
}
*/
//���ɳ�����
string cmvsIO::init(string rootdir) {
	//��ȡ��Ŀ¼����rootStr��
	/*TCHAR szPath[MAX_PATH] = { 0 };
	if (GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		(_tcsrchr(szPath, _T('\\')))[1] = 0;
	}
	string rootStr = szPath;*/
	//string str = "/";
	//vector<string> rootStr = pathChange(str);
	//ƴ��system�������Ŀ¼
	string outStr = rootdir + "\\cmvs\\pmvs\\";
	string outOpStr = rootdir + "\\cmvs\\pmvs\\ option-0000";
	//ƴ��system����exe�ļ�λ��
	string cStr = rootdir + "\\cmvs\\cmvs.exe";
	string genStr = rootdir + "\\cmvs\\genOption.exe";
	string pStr = rootdir + "\\cmvs\\pmvs2.exe";
	string ply = rootdir + "\\cmvs\\pmvs\\models\\option-0000.ply";
	//ƴ������system����
	string sys1 = cStr + " " + outStr;
	string sys2 = genStr + " " + outStr;
	string sys3 = pStr + " " + outOpStr;
	//ִ��system����
	system(sys1.c_str());
	system(sys2.c_str());
	system(sys3.c_str());
	/*-------------------�ж��ļ��Ƿ����------------------------------*/
	fstream _file;
	      _file.open(ply, ios::in);
		  if (_file) return ply;
		  else return NULL;
		  _file.close();
}


/*vector<string> cmvsIO::pathChange(string input) {
	vector<string> inputpath, outputpath;
	string tmp;
	for (int i = 0; i<input.size(); i++) {   //��input�ַ�������/���ֿ��洢��inputpath 
		if (input[i] != '/') {			//�����С� ע�⣺��һ��Ϊ���ַ����� 
			tmp += input[i];
		}
		else {
			inputpath.push_back(tmp);
			tmp.clear();
		}
	}

	while (!inputpath.empty()) {			//�ж�vector���һ�������Ƿ�Ϊ��..������ 
		if (inputpath.back() != "..") {	//�ǣ�������2��ɾ��β���ݣ�������β�� 
			tmp = inputpath.back();		//�ݴ浽outputpath�У���ɾ��inputpath�е� 
			outputpath.push_back(tmp);	//β���ݡ� 
			inputpath.pop_back();
		}
		else {
			inputpath.pop_back();
			inputpath.pop_back();
		}
	}

	while (!outputpath.empty()) {					//��outputpath�е����ݴ�β��ͷת 
		inputpath.push_back(outputpath.back());	//�浽inputpath�еĴ�ͷ��β�� 
		outputpath.pop_back();
	}
	return inputpath;
}*/
