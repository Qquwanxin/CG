#ifndef MODELH
#define MODELH
#include<vector>
#include <GL/glut.h>
#include <iostream>
#include "sphere.h"
#include "moving_sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "box.h"
#include "surface_texture.h"
#include "aarect.h"
#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION"
#include "pdf.h"
#include "model.h"
#include <vector>
struct vec2
{
    double x, y;
};

class Vertex
{
public: 
    int vertIndex;     // 此顶点在顶点列表中的索引
    int normIndex;     // 顶点的法线索引
    int textIndex;     // 顶点的纹理索引
};

class Face
{
public:
    vector<Vertex> vertex; // 顶点和法线索引组成的列表

    Face(){}
    ~Face(){}
};
class OFFModel {
public:
    int nVer, nFace;                // the number of vers and faces
    vector<Vec3f> vers;             // a series of vers contained in model
    vector<Vec3d> faces;            // a series of ver idxs to construct faces, each face containing 3 vers
    vector<Vec3f> faceNorms;        // the norms of the corresponding faces
    Vec3f surfColor;                // the surface color of this model

    // use .off file to generate model
    // the format of .off file is easy to parse
    OFFModel(const char* filename) {
        cout << filename;
        ifstream fin(filename);
        char tmpchar[100];
        int tmpnum;
        // define the model surface color
        surfColor.x = 0.9;  surfColor.y = 0.9; surfColor.z = 0.9;
        fin >> tmpchar;
        cout << tmpchar;
        fin >> nVer >> nFace >> tmpnum;
        cout << nVer << nFace << tmpnum;
        // get the ver information
        for (int i = 0; i < nVer; ++i) {
            Vec3f pt;
            fin >> pt.x >> pt.y >> pt.z;
            pt.z = -pt.z;
            
            pt = pt*5+ Vec3f(0, 0, -40);
            cout << pt;
            vers.push_back(pt);
        }
        // get the face information (including face norms)
        for (int i = 0; i < nFace; ++i) {
            Vec3d fs;
            Vec3f nm;
            fin >> tmpnum >> fs.x >> fs.y >> fs.z;
            faces.push_back(fs);
            // compute the face norm using outter production
            Vec3f v1 = vers[fs.y] - vers[fs.x], v2 = vers[fs.z] - vers[fs.y];
            nm = v1.outdot(v2);
            nm.normalize();
            faceNorms.push_back(nm);
        }
        fin.close();
    }
    OFFModel(const char* filename,float scale, float position_x,float position_y,float position_z) {
        cout << filename;
        ifstream fin(filename);
        char tmpchar[100];
        int tmpnum;
        // define the model surface color
        surfColor.x = 1;  surfColor.y = 0; surfColor.z = 0;
        fin >> tmpchar;
        cout << tmpchar;
        fin >> nVer >> nFace >> tmpnum;
        cout << nVer << nFace << tmpnum;
        // get the ver information
        for (int i = 0; i < nVer; ++i) {
            Vec3f pt;
            fin >> pt.x >> pt.y >> pt.z;
            pt.z = -pt.z;

            pt = pt * scale+ Vec3f(position_x, position_y, position_z);
            cout << pt;
            vers.push_back(pt);
        }
        // get the face information (including face norms)
        for (int i = 0; i < nFace; ++i) {
            Vec3d fs;
            Vec3f nm;
            fin >> tmpnum >> fs.x >> fs.y >> fs.z;
            faces.push_back(fs);
            // compute the face norm using outter production
            Vec3f v1 = vers[fs.y] - vers[fs.x], v2 = vers[fs.z] - vers[fs.y];
            nm = v1.outdot(v2);
            nm.normalize();
            faceNorms.push_back(nm);
        }
        fin.close();
    }
#endif
