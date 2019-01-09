#include "vector"
#include "iostream"
#include "string"
#include "fstream"
#include "sstream"
#include "algorithm"
#include "assert.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#pragma comment(lib, "glut.lib")

using namespace std;
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

struct vec3
{
    double x, y, z;
};

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

class Mesh
{
private:
    vector<vec3> vVertex;  // 顶点数组
    vector<vec2> vText;    // 纹理数组
    vector<vec3> vNorm;    // 法线数组
    vector<Face> vFace;    // 面片数组

public:
    Mesh(){};
    ~Mesh(){};

    bool readFile(char* path);
    bool drawMesh(vector<vec3> array[2]);
};

bool Mesh::readFile(char* path)
{   
    ifstream file(path);
    if (!file)
    {
        cerr << "Error::ObjLoader, could not open obj file:"
            << path << " for reading." << std::endl;
        return false;
    }
    string line;
    while (getline(file, line))
    {
        if (line.substr(0, 2) == "vt")     // 顶点纹理坐标数据
        {
            istringstream s(line.substr(2));
            vec2 v;
            s >> v.x; s >> v.y;
            //cout << "vt " << v.x << " " << v.y << endl;
            v.y = -v.y;                     // 注意这里加载的dds纹理 要对y进行反转
            vText.push_back(v);
        }
        else if (line.substr(0, 2) == "vn") // 顶点法向量数据
        {
            istringstream s(line.substr(2));
            vec3 v;
            s >> v.x; s >> v.y; s >> v.z;
            //cout << "vn " << v.x << " " << v.y << " " << v.z << endl;
            vNorm.push_back(v);
        }
        else if (line.substr(0, 1) == "v")  // 顶点位置数据
        {
            istringstream s(line.substr(1));
            vec3 v;
            s >> v.x; s >> v.y; s >> v.z;
            //cout << "v " << v.x << " " << v.y << " " << v.z << endl;
            vVertex.push_back(v);
        }
        else if (line.substr(0, 1) == "f")  // 面数据
        {
            Face face;
            //cout << "f ";
            istringstream vtns(line.substr(1));
            string vtn;
            while (vtns >> vtn)             // 处理一行中多个顶点属性
            {
                Vertex vertex;
                replace(vtn.begin(), vtn.end(), '/', ' ');
                istringstream ivtn(vtn);
                if (vtn.find("  ") != string::npos) // 没有纹理数据，注意这里是2个空格
                {
                    ivtn >> vertex.vertIndex        
                         >> vertex.normIndex;

                    vertex.vertIndex--;     //使得下标从0开始
                    vertex.normIndex--;
                }
                else
                {
                    ivtn >> vertex.vertIndex  
                         >> vertex.textIndex 
                         >> vertex.normIndex;

                    //cout <<  vertex.vertIndex << "/" << vertex.textIndex << "/" << vertex.normIndex << " ";
                    vertex.vertIndex--;   
                    vertex.textIndex--;
                    vertex.normIndex--;
                }
                face.vertex.push_back(vertex);
            }
            vFace.push_back(face);
            //cout << endl;
        }
        else if (line[0] == '#')           
        { }
        else  
        {
            
        }
    }

    return true;
}

bool Mesh::drawMesh(vector<vec3> array[2])
{
    if(vFace.empty())   return 0;
    for(int f = 0; f < vFace.size(); f++)  
    {
        int n = vFace[f].vertex.size();   
//        glBegin(GL_TRIANGLES);
        for(int v = 0; v < n; v++)
        {
            int in = vFace[f].vertex[v].normIndex;
            array[0][v].x=vNorm[in].x;//, vNorm[in].y, vNorm[in].z);
array[0][v].y=vNorm[in].y;
array[0][v].z=vNorm[in].z;
//            glNormal3f(vNorm[in].x, vNorm[in].y, vNorm[in].z);
            int iv = vFace[f].vertex[v].vertIndex; 
            array[1][v].x=vVertex[iv].x;//, vVertex[iv].y, vVertex[iv].z);
array[1][v].y=vVertex[iv].y;
array[1][v].z=vVertex[iv].z;
//            glVertex3f(vVertex[iv].x, vVertex[iv].y, vVertex[iv].z);
        }
//        glEnd();
    }
return 1;
}

int main(int argc, char** argv)
{
    Mesh mesh;
    mesh.readFile("bunny.obj");
vector<vec3> array[2];
    mesh.drawMesh(array);
    return 0;
}
