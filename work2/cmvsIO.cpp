#include "cmvsIO.h";
#include <string>;
#include <string.h>
#include <vector>;
#include <iostream>;
#include <windows.h>;
#include "CalRAndTPlusColor.h";
#include <direct.h>;


using namespace std;

//����cmvs�����Ŀ¼�ṹ
void cmvsIO::prepareDir() {
	string root = "./cmvs";
	string pmvs = "./cmvs/pmvs";
	 string imagedir = "./cmvs/pmvs/visualize";
	 string txtdir = "./cmvs/pmvs/txt";
	 string modeldir = "./cmvs/pmvs/models";
	 bool flag1 = CreateDirectory(pmvs.c_str(), NULL);
	 bool imageflag = CreateDirectory(imagedir.c_str(), NULL);
	 bool txtflag = CreateDirectory(txtdir.c_str(), NULL);
	 bool molflag = CreateDirectory(modeldir.c_str(), NULL);
	 if (imageflag == true && txtflag == true && molflag == true) {
		 cout << "Directory created" << endl;
	 }
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
Mat cmvsIO::getMatP(Mat inner, Mat rotation, Mat motion) {
	Mat result;
	Mat outer;
	outer = rotation + motion;
	result = inner * outer;
	return result;
};
 //������Ҫ���ļ�
void cmvsIO::prepareFiles(CalRAndTPlusColor cratpc) {
	int imageNum = 0;
	string txtdir = "./cmvs/pmvs/txt";
	string imgdir = "./cmvs/pmvs/visualize";
	//CalRAndTPlusColor cratpc = CalRAndTPlusColor();
	vector<Mat> rotation = cratpc.getRotation();       //�����ת����
	vector<Mat> motion = cratpc.getMotion();       //���ƽ�ƾ���
	vector<int> ks = cratpc.getK1And1AndK2();          //����������k1,k2
	vector<Point3f> cam3d = cratpc.getStructure();          //����ռ�����
	vector<vector<vector<Point2f>>> points3d = cratpc.getMatchesPts();        //��ȡƥ����б�
	double focal = cratpc.getFocal();      //��ȡ�������
	vector<vector<int>> matchcam = cratpc.getCorrespondStru();         //��ȡÿ��ϡ���ƥ������
	vector<vector<Point3f>> pointspo = cratpc.getPos();
	Mat inner = CalRAndTPlusColor::getInnerMat();
	int camNum = rotation.size();   //�����ͼƬ����
	int pointNum = points3d.size();     //ϡ����Ƶ���
	int k1 = ks[0];      //�������k1
	int k2 = ks[1];      //�������k2
	/*----------------����������Ƭ��Ϣ��txt�ļ�-------------------------*/
	for (int i = 0; i < camNum; i++) {
		//�ļ���
		string num = to_string(i);
		string txtname = num + ".txt";
		string imgname = i + ".jpg";
		//�ļ���ַ
		string txtdir = "./cmvs/pmvs/txt/" + txtname;
		string imgdir = "./cmvs/pmvs/visualize/" + imgname;
		Mat rot = rotation[i];
		Mat mot = motion[i];
		Mat P = getMatP(inner, rot, mot);
		ofstream txtIO;
		txtIO.open(txtdir);
		txtIO << "CONTOUR" << endl;
		for (int n = 0; n < 3; n++) {
			txtIO << P.at<float>(n, 0) << "\t" << P.at<float>(n, 1) << "\t" << P.at<float>(n, 2) << "\t" << P.at<float>(n, 3) << endl;
		}
		txtIO.close();
	}
	/*--------------------------------����bundle.rd.out--------------------------------------*/
	ofstream bundleIO;
	string bundledir = "./cmvs/pmvs/bundle.rd.out";
	bundleIO.open("bundledir");
	bundleIO << "# Bundle file v0.3" << endl << "<" << camNum << ">" << "\t" << "<" << pointNum << ">" << endl;
	for (int i = 0; i < camNum; i++) {
		bundleIO << "<camera" << i + 1 << ">" << endl;
		bundleIO << focal << "\t" << k1 << "\t" << k2 << endl;
		Mat rot = rotation[i];
		Mat mot = motion[i];
		for (int n = 0; n < 3; n++) {
			bundleIO << rot.at<float>(n, 0) << "\t" << rot.at<float>(n, 1) << "\t" << rot.at<float>(n, 2) << endl;
		}
		bundleIO << mot.at<float>(0, 0) << "\t" << mot.at<float>(0, 1) << "\t" << mot.at<float>(0, 2) << endl;
	}

	for (int i = 0; i < pointNum; i++) {
		bundleIO << "<point" << i + 1 << ">" << endl;
		vector<int> camlist = matchcam[i];    //ƥ�������б�
	}
};

//����bundle.rd.out
/*void cmvsIO::prepareBundle(CalRAndTPlusColor cratpc) {
	vector<Mat> rotate = cratpc.getRotation();
	string dir = "./cmvs/pmvs";
	vector<vector<vector<Point2f>>> matchPoints = cratpc.getMatchesPts();
}
*/
 //���ɳ�����
  void cmvsIO::init() {
	 //��ȡ��Ŀ¼����rootStr��
	 TCHAR szPath[MAX_PATH] = { 0 };   
	 if (GetModuleFileName(NULL, szPath, MAX_PATH))
	 {
		 (_tcsrchr(szPath, _T('\\')))[1] = 0;
	 }
	 string rootStr = szPath;
	 //ƴ��system�������Ŀ¼
	 string outStr = rootStr + "\\cmvs\\pmvs";
	 string outOpStr = rootStr + "\\cmvs\\pmvs\\option-0000";
	 //ƴ��system����exe�ļ�λ��
	 string cStr = rootStr + "\\cmvs\\cmvs.exe";
	 string genStr = rootStr + "\\cmvs\\genOption.exe";
	 string pStr = rootStr + "\\cmvs\\pmvs.exe";
	 //ƴ������system����
	 string sys1 = cStr + " " + outStr;
	 string sys2 = genStr + " " + outStr;
	 string sys3 = pStr + " " + outOpStr;
	 //ִ��system����
	 system(sys1.c_str());
	 system(sys2.c_str());
	 system(sys3.c_str());

 }

 
