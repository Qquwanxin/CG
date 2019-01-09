#include <fstream>
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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "pdf.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <vector>

using namespace std;

#define PI 3.1415926
#define INFINITY 1e8
#define MAX_RAY_DEPTH 4  
float Image[640 * 480 * 4];

inline vec3 de_nan(const vec3& c) {
    vec3 temp = c;
    if (!(temp[0] == temp[0])) temp[0] = 0;
    if (!(temp[1] == temp[1])) temp[1] = 0;
    if (!(temp[2] == temp[2])) temp[2] = 0;
    return temp;
}
template<typename T>
class Vector3 {
public:
	T x, y, z; 
	Vector3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vector3(T xx) : x(xx), y(xx), z(xx) {}
	Vector3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	Vector3& normalize() {
		T nor2 = length2();
		if (nor2 > 0) {
			T invNor = 1 / sqrt(nor2);
			x *= invNor, y *= invNor, z *= invNor;
		}
		return *this;
	}

	//operator overload
	Vector3<T> operator * (const T &f) const { return Vector3<T>(x * f, y * f, z * f); }
	Vector3<T> operator * (const Vector3<T> &v) const { return Vector3<T>(x * v.x, y * v.y, z * v.z); }
	T dot(const Vector3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
	Vector3<T> outdot(const Vector3<T> &v) const { return Vector3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
	Vector3<T> operator - (const Vector3<T> &v) const { return Vector3<T>(x - v.x, y - v.y, z - v.z); }
	Vector3<T> operator + (const Vector3<T> &v) const { return Vector3<T>(x + v.x, y + v.y, z + v.z); }
	Vector3<T>& operator += (const Vector3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
	Vector3<T>& operator *= (const Vector3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
	Vector3<T> operator - () const { return Vector3<T>(-x, -y, -z); }
	T length2() const { return x * x + y * y + z * z; }
	T length() const { return sqrt(length2()); }
	friend std::ostream & operator << (std::ostream &os, const Vector3<T> &v) {os << "[" << v.x << " " << v.y << " " << v.z << "]";return os;}
};
typedef Vector3<float> Vec3f;
typedef Vector3<int> Vec3d;
class Sphere {
public:
	Vec3f ct;								
	float r, r2;						
	Vec3f surfColor, emisColor;				
	float transparency, reflection;  

	Sphere(const Vec3f &c, const float &rr, const Vec3f &sc, const float &refl = 0, const float &transp = 0, const Vec3f &ec = 0):
		   ct(c), r(rr), r2(rr * rr), surfColor(sc), emisColor(ec),transparency(transp), reflection(refl) {}
	bool intersect(const Vec3f &rs, const Vec3f &rd, float &t0, float &t1) const {
		Vec3f l = ct - rs;
		float tca = l.dot(rd);
		if (tca < 0) return false;
		float d2 = l.dot(l) - tca * tca;
		if (d2 > r2) return false;
		float thc = sqrt(r2 - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}
};
class OFFModel {
public:
	int nVer, nFace;				
	vector<Vec3f> vers;				
	vector<Vec3d> faces;			
	vector<Vec3f> faceNorms;		
	Vec3f surfColor;
	OFFModel(const char* filename) {
		ifstream fin(filename);
		char tmpchar[100];
		int tmpnum;
		surfColor.x = 0.9;  surfColor.y = 0.9; surfColor.z = 0.9;
		fin >> tmpchar;
		fin >> nVer >> nFace >> tmpnum;
		for (int i = 0; i < nVer; ++i) {
			Vec3f pt;
			fin >> pt.x >> pt.y >> pt.z;
			pt.z = -pt.z;
			
			pt = pt*5+ Vec3f(10, 10, 20);
			vers.push_back(pt);
		}
	
		for (int i = 0; i < nFace; ++i) {
			Vec3d fs;
			Vec3f nm;
			fin >> tmpnum >> fs.x >> fs.y >> fs.z;
			faces.push_back(fs);
			Vec3f v1 = vers[fs.y] - vers[fs.x], v2 = vers[fs.z] - vers[fs.y];
			nm = v1.outdot(v2);
			nm.normalize();
			faceNorms.push_back(nm);
		}
		fin.close();
	}
	OFFModel(const char* filename,float scale, float position_x,float position_y,float position_z) {
		ifstream fin(filename);
		char tmpchar[100];
		int tmpnum;
		surfColor.x = 1;  surfColor.y = 0; surfColor.z = 0;
		fin >> tmpchar;
		fin >> nVer >> nFace >> tmpnum;
		for (int i = 0; i < nVer; ++i) {
			Vec3f pt;
			fin >> pt.x >> pt.y >> pt.z;
			pt.z = -pt.z;

			pt = pt * scale+ Vec3f(position_x, position_y, position_z);
			vers.push_back(pt);
		}
		for (int i = 0; i < nFace; ++i) {
			Vec3d fs;
			Vec3f nm;
			fin >> tmpnum >> fs.x >> fs.y >> fs.z;
			faces.push_back(fs);
			Vec3f v1 = vers[fs.y] - vers[fs.x], v2 = vers[fs.z] - vers[fs.y];
			nm = v1.outdot(v2);
			nm.normalize();
			faceNorms.push_back(nm);
		}
		fin.close();
	}

	int intersect(const Vec3f &rayorig, const Vec3f &raydir, float &t) {
		for (int i = 0; i < nFace; ++i) {
			if (intersect(i, rayorig, raydir, t))
				return i;
		}
		return -1;
	}

private:
	bool intersect(int i, const Vec3f &rs, const Vec3f &rd, float &t) {
		Vec3f nm = faceNorms[i], pt1 = vers[faces[i].x], pt2 = vers[faces[i].y], pt3 = vers[faces[i].z];
		float tmp_t;
		float dir_dot_norm = rd.dot(nm);
		if (dir_dot_norm >= 0) {
			return false;
		}
		tmp_t = nm.dot(pt1 - rs) * (1 / dir_dot_norm);

		Vec3f P = rs + rd * tmp_t;
		float areaAll = ((pt2 - pt1).outdot(pt3 - pt2)).length();
		float area1 = ((pt2 - pt1).outdot(P - pt2)).length();
		float area2 = ((pt3 - pt2).outdot(P - pt3)).length();
		float area3 = ((pt1 - pt3).outdot(P - pt1)).length();
		if (area1 + area2 + area3 > areaAll + area1 / 1000) return false;
		t = tmp_t;
		return true;
	}
};

float mix(const float &a, const float &b, const float &mix) {return b * mix + a * (1 - mix);}
Vec3f trace(const Vec3f &rs, const Vec3f &rd, const std::vector<Sphere> &spheres, OFFModel &model, const int &depth) {
	float tnear = INFINITY;
	const Sphere* sphere = NULL;
	for (unsigned i = 0; i < spheres.size(); ++i) {
		float t0 = INFINITY, t1 = INFINITY;
		if (spheres[i].intersect(rs, rd, t0, t1)) {
			if (t0 < 0) t0 = t1;
			if (t0 < tnear) {
				tnear = t0;
				sphere = &spheres[i];
			}
		}
	}
	float t = INFINITY;
	int face_id = model.intersect(rs, rd, t);
	if (t < tnear) {
		tnear = t;
		sphere = NULL;
	}

	Vec3f surfColor = 0;
	if (face_id >= 0) {
		surfColor = model.surfColor * model.faceNorms[face_id].dot(-rd);
		return surfColor;
	}
	if (!sphere) { return Vec3f(2); }
	Vec3f P = rs + rd * tnear;
	Vec3f N = P - sphere->ct;
	N.normalize();
	float bias = 1e-4;
	bool inside = false;
	if (rs.dot(N) > 0) { N = -N; inside = true; }
	if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH) {
		float facingratio = -rd.dot(N);
		float fresneleffect = mix(pow(1 - facingratio, 3), 1, 0.1);
		Vec3f refldir = rd - N * 2 * rd.dot(N);
		refldir.normalize();
		Vec3f reflection = trace(P + N * bias, refldir, spheres, model, depth + 1);
		Vec3f refraction = 0;
		
		if (sphere->transparency) {
			float ior = 1.1, eta = (inside) ? ior : 1 / ior;
			float cosi = -N.dot(rd);
			float k = 1 - eta * eta * (1 - cosi * cosi);
			Vec3f refrdir = rd * eta + N * (eta *  cosi - sqrt(k));
			refrdir.normalize();
			refraction = trace(P - N * bias, refrdir, spheres, model, depth + 1);
		}
		
		surfColor = (reflection*fresneleffect + refraction*(1 - fresneleffect)*sphere->transparency)*sphere->surfColor;
	}
	else {
		
		for (unsigned i = 0; i < spheres.size(); ++i) {
			
				if (spheres[i].emisColor.x > 0) {
					
					Vec3f transmission = 1;
					Vec3f lightDirection = spheres[i].ct - P;
					lightDirection.normalize();
				
					for (unsigned j = 0; j < spheres.size(); ++j) {
						if (i != j) {
							float t0, t1;
							if (spheres[j].intersect(P + N * bias, lightDirection, t0, t1)) {
								transmission = 0;
								break;
							}
						}
					}
				
					float tmp_t;
					int tmp_face_id = model.intersect(P + N * bias, lightDirection, tmp_t);
					if (tmp_face_id > 0) transmission = 0;
					surfColor += sphere->surfColor * transmission * max(float(0), N.dot(lightDirection)) * spheres[i].emisColor;
				//}
			}
		}
	}

	return surfColor + sphere->emisColor;
}

void render(const std::vector<Sphere> &spheres, OFFModel model) {
	unsigned width = 640, height = 480;
	Vec3f *image = new Vec3f[width * height], *pixel = image;
	float invWidth = 1 / float(width), invHeight = 1 / float(height);
	float fov = 30, aspectratio = width / float(height);
	float angle = tan(PI * 0.5 * fov / 180.);

	for (unsigned y = 0; y < height; ++y) {
		for (unsigned x = 0; x < width; ++x, ++pixel) {
			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Vec3f raydir(xx, yy, -1);
			raydir.normalize();
			// trace rays
			*pixel = trace(Vec3f(0, 0, 0), raydir, spheres, model, 0);
		}
	}

	//store the data in the Image array
    float *LinePD = Image;
	for (int i = height - 1; i >= 0; --i) {
		for (int j = 0; j < width; ++j) {
			LinePD[0] = min(float(1), image[j + i * width].x);
			LinePD[1] = min(float(1), image[j + i * width].y);
			LinePD[2] = min(float(1), image[j + i * width].z);
			LinePD[3] = 0.02;
			LinePD += 4;
		}
	}
	
	std::ofstream ofs("./result.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(min(float(1), image[i].x) * 255) <<
			(unsigned char)(min(float(1), image[i].y) * 255) <<
			(unsigned char)(min(float(1), image[i].z) * 255);
	}
	ofs.close();
	delete[] image;
}

//display the rendered image using OpenGL
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(640, 480, GL_RGBA, GL_FLOAT, Image);
	glFlush();
}
vec3 color(const ray& r, hitable *world, hitable *light_shape, int depth) {
    hit_record hrec;
    if (world->hit(r, 0.001, MAXFLOAT, hrec)) {
        scatter_record srec;
        vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
        if (depth < 50 && hrec.mat_ptr->scatter(r, hrec, srec)) {
            if (srec.is_specular) {
                return srec.attenuation * color(srec.specular_ray, world, light_shape, depth+1);
            }
            else {
                hitable_pdf plight(light_shape, hrec.p);
                mixture_pdf p(&plight, srec.pdf_ptr);
                ray scattered = ray(hrec.p, p.generate(), r.time());
                float pdf_val = p.value(scattered.direction());
                delete srec.pdf_ptr;
                return emitted + srec.attenuation*hrec.mat_ptr->scattering_pdf(r, hrec, scattered)*color(scattered, world, light_shape, depth+1) / pdf_val;
            }
        }
        else
            return emitted;
    }
    else
        return vec3(0,0,0);
}

void cornell_box(hitable **scene, camera **cam, float aspect) {
    int i = 0;
    texture *pertext = new noise_texture(4);

    hitable **list = new hitable*[8];
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    material *glass = new dielectric(1.5);
    list[i++] = new sphere(vec3(160, 350, 170),70 , new lambertian(pertext));
    list[i++] = new translate(new rotate_y(
                    new box(vec3(0, 0, 0), vec3(160, 160, 160), new lambertian(pertext)),  15), vec3(265,0,295));
    list[i++] = new sphere(vec3(140,40,140), 40,glass);
    *scene = new hitable_list(list,i);
    vec3 lookfrom(300, 300, -800);
    vec3 lookat(300,300,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    *cam = new camera(lookfrom, lookat, vec3(0,1,0),
                      vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}

int main(int argc, char **argv) {
    int nx = 500;
    int ny = 500;
    int ns = 10000;
    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    hitable *world;
    //OFFModel bunny("bunny.off",2,120,130,40);
    camera *cam;
    float aspect = float(ny) / float(nx);
    cornell_box(&world, &cam, aspect);
    hitable *light_shape = new xz_rect(213, 343, 227, 332, 554, 0);
    hitable *glass_sphere = new sphere(vec3(190, 90, 190), 90, 0);
    hitable *a[2];
    a[0] = light_shape;
    a[1] = glass_sphere;
    hitable_list hlist(a,2);
    vector<Sphere> spheres;
    spheres.push_back(Sphere(Vec3f(0.0, -10004, -20), 10000, Vec3f(0.20, 0.20, 0.20), 0, 0.0));
    spheres.push_back(Sphere(Vec3f(7.0, -1, 20), 2, Vec3f(1.0, 0.76, 0.0), 1, 1.0));
    const char * name = "dragon_2048.off";
    OFFModel bunny(name,8,3,0,-10);
    spheres.push_back(Sphere(Vec3f(0.0, 20, -30), 3, Vec3f(0.00, 0.00, 0.00), 0, 0.0, Vec3f(3)));
    render(spheres, bunny);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(200, 200);
    //glutCreateWindow("rt");
    glClearColor(1, 1, 1, 1);
    glutDisplayFunc(display);
    glutMainLoop();
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = float(i+drand48())/ float(nx);
                float v = float(j+drand48())/ float(ny);
                ray r = cam->get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += de_nan(color(r, world, &hlist, 0));
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
    
	return 0;
}

