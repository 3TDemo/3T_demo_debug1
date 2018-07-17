#include "stdafx.h"
#include "ToMeshWithRGB.h"
void plytomesh(string plyname) {
    pcl::PCLPointCloud2 clod;
    pcl::PLYReader reader;
    reader.read(plyname, clod);
    pcl::PCDWriter writer;
    writer.writeASCII("temporary.pcd", clod);
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
    if (pcl::io::loadPCDFile("temporary.pcd", *cloud)==-1)
        {
        PCL_ERROR("�򲻿�pcd�ļ���\n");
        return ;
        }
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr filtered(new pcl::PointCloud<pcl::PointXYZRGB>());
    pcl::PassThrough<pcl::PointXYZRGB> filter;
    filter.setInputCloud(cloud);
    filter.filter(*filtered);
    pcl::NormalEstimationOMP<pcl::PointXYZRGB, pcl::Normal> ne;//������Ʒ���
    pcl::search::KdTree<pcl::PointXYZRGB>::Ptr tree1(new pcl::search::KdTree<pcl::PointXYZRGB>);
    pcl::PointCloud<pcl::Normal>::Ptr cloud_normals(new pcl::PointCloud<pcl::Normal>());
    tree1->setInputCloud(filtered);
    ne.setNumberOfThreads(32);
    ne.setInputCloud(filtered);
    ne.setSearchMethod(tree1);
    ne.setKSearch(5);//kȡ�ù����ʧ��
    ne.compute(*cloud_normals);
    //��ʾ����
    Eigen::Vector4f centroid;
    compute3DCentroid(*filtered, centroid);//�����������
    ne.setViewPoint(centroid[0], centroid[1], centroid[2]);//����������ԭ�����ڵ�������
                                                           //�ں�RGB���ƺͷ���,�����Ʒ�����Ϣ������ԭ���ƣ�Normal��ʾ�Լ�����������������ʹ���
    pcl::PointCloud<pcl::PointXYZRGBNormal>::Ptr cloud_smoothed_normals(new pcl::PointCloud<pcl::PointXYZRGBNormal>());
    concatenateFields(*filtered, *cloud_normals, *cloud_smoothed_normals);
    //�����ؽ�
    pcl::search::KdTree<pcl::PointXYZRGBNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointXYZRGBNormal>);
    tree2->setInputCloud(cloud_smoothed_normals);
    pcl::Poisson<pcl::PointXYZRGBNormal> pn;
    pn.setSearchMethod(tree2);
    pn.setInputCloud(cloud_smoothed_normals);
    pn.setConfidence(false);//�Է��߽��й�һ������true�÷�������������Ϊ���Ŷ���Ϣ
    pn.setManifold(false);//true�Զ���ν���ϸ�����ǻ�ʱ������ģ�����false�����
    pn.setOutputPolygons(false);//�����Ƿ����Ϊ�����
    pn.setIsoDivide(8);
    pn.setSamplesPerNode(3);//����ÿ���˲����ڵ������ٲ�������Ŀ
    pcl::PolygonMesh mesh;//�γ�mesh����
    pn.reconstruct(mesh);
    //��ʾ��ɫģ��
    //boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer0(new pcl::visualization::PCLVisualizer("3Dģ��"));
    //viewer0->setBackgroundColor(255, 255, 255);
    //viewer0->addPolygonMesh(mesh, "my");
    //viewer0->initCameraParameters();
    //while (!viewer0->wasStopped())
    //    {
    //    viewer0->spinOnce(100);
    //    boost::this_thread::sleep(boost::posix_time::microseconds(100000));
    //    }
    //�����ؽ����
    //��meshȾɫ
    pcl::PointCloud<pcl::PointXYZRGB> cloud_color_mesh;
    pcl::fromPCLPointCloud2(mesh.cloud, cloud_color_mesh);//mesh��Ϣ�������
    pcl::KdTreeFLANN<pcl::PointXYZRGB> kdtree;//ʹ��kdtree��ԭ���Ƶ���Ϣӳ��������ɫmesh�ϣ������ɲ�ɫmesh
    kdtree.setInputCloud(cloud);
    //k�����㷨
    //kԽСԽ��Ҫ������㣬��Ϳ�ǰ�����
    int K=2;
    std::vector<int> pointIdxNKNSearch(K);
    std::vector<float> pointNKNSquaredDistance(K);
    for (int i=0; i<cloud_color_mesh.points.size(); ++i)
        {
        uint8_t r=0;
        uint8_t g=0;
        uint8_t b=0;
        float dist=0.0;
        int red=0;
        int green=0;
        int blue=0;
        //uint32_t rgb;
        if (kdtree.nearestKSearch(cloud_color_mesh.points[i], K, pointIdxNKNSearch, pointNKNSquaredDistance)>0)
            {
            for (int j=0; j<pointIdxNKNSearch.size(); ++j)
                {
                r=cloud->points[pointIdxNKNSearch[j]].r;
                g=cloud->points[pointIdxNKNSearch[j]].g;
                b=cloud->points[pointIdxNKNSearch[j]].b;
                red+=int(r);
                green+=int(g);
                blue+=int(b);
                dist+=1.0/pointNKNSquaredDistance[j];
                }
            }
        cloud_color_mesh.points[i].r=int(red/pointIdxNKNSearch.size()+0.5);
        cloud_color_mesh.points[i].g=int(green/pointIdxNKNSearch.size()+0.5);
        cloud_color_mesh.points[i].b=int(blue/pointIdxNKNSearch.size()+0.5);
        }
    //������Ϣ����mesh����rgb��Ϣ
    toPCLPointCloud2(cloud_color_mesh, mesh.cloud);
    boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer(new pcl::visualization::PCLVisualizer("3Dģ����ɫ��"));
    viewer->setBackgroundColor(255, 255, 255);
    viewer->addPolygonMesh(mesh, "my");
    viewer->initCameraParameters();
    while (!viewer->wasStopped())
        {
        viewer->spinOnce(100);
        boost::this_thread::sleep(boost::posix_time::microseconds(100000));
        }
    return ;
    //-----------------------------------------------------------------------------------------------------------
    //������
    //pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    //if (pcl::io::loadPCDFile("l.pcd", *cloud)==-1)
    //    {
    //    PCL_ERROR("�򲻿�pcd�ļ���\n");
    //    return 0;
    //    }
    //pcl::PointCloud<pcl::Normal>::Ptr cloud_normals(new pcl::PointCloud<pcl::Normal>());
    //pcl::IntegralImageNormalEstimation<pcl::PointXYZ, pcl::Normal> ne;
    //ne.setNormalEstimationMethod(ne.COVARIANCE_MATRIX);
    //ne.setMaxDepthChangeFactor(0.02f);
    //ne.setNormalSmoothingSize(10.0f);
    //ne.setInputCloud(cloud);
    //ne.compute(*cloud_normals);
    //////��ʾ����
    //pcl::visualization::PCLVisualizer viewer("PCL Viewer");
    //viewer.setBackgroundColor(0.0, 0.0, 0.5);
    //viewer.addPointCloudNormals<pcl::PointXYZ, pcl::Normal>(cloud, cloud_normals);
    //while (!viewer.wasStopped())
    //    {
    //    viewer.spinOnce();
    //    }
    //return 0;
    }