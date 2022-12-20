#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "bazierface.h"
#include "shader.h"
#include "base/camera.h"

using namespace std;

//模型类
//一个模型由多个贝叶斯旋转面连接而成

class DIYtexture {
public:
    int type; //0 环绕 //1 顶部
    int repeat;//一圈的重复数 顶部忽略
    float l, r;//范围
    int reverse; //反转
    std::string name;
    GLuint map;
    DIYtexture(int t, int re, float l0, float r0, GLuint m) {
        type = t;
        repeat = re;
        l = l0;
        r = r0;
        map = m;
        reverse = 0;
    }


    DIYtexture() { type = repeat = l = map = 0; r = 1; };
    ~DIYtexture() {};
};

class DIYmodel
{
private:
    /* data */
    glm::vec3 offset;
    vector<glm::vec3> vertices;//锚点
    int active_point;
    int active_tex;
    int material_idx;
    DIYtexture material;
    vector<BezierFace> faces;
    vector<DIYtexture> textures;

    unsigned int VAO, VBO[3];
    unsigned int VAO2, VBO2;
    unsigned int VAO3, VBO3;
    unsigned int VAO4[2], VBO4[2];

    vector<float> pvalues;  //顶点坐标
    vector<float> tvalues;  //纹理坐标
    vector<float> nvalues;  //法线
    vector <float> fpvalues;//锚点坐标
    vector <float> tfpvalues;
    vector <float> apvalues;//锚点坐标

    int totalindex, findex;

    GLuint load_texture(string s, DIYtexture& diy);
    int load_model(vector<float>* pvalues, vector<float>* tvalues, vector<float>* nvalues);
    int load_frame(vector<float>* pvalues);
    int load_circle(vector<float>* pvalues, int tid, bool lor);
    bool load_active(vector<float>* pvalues);
    void makeFaces();
    void init();

public:


    int get_point(float x, float y, Camera camera);
    int get_line_start_point(float x, float y, Camera camera);
    int get_circle(float x, float y, Camera camera, bool& lor);
    int get_between_circle(float x, float y, Camera camera);
    void active(int index);

    void split_point(int pid);
    void remove_point(int pid);
    void modify_point(float dx, float dy, Camera camera);
    void modify_offset(float dx, float dy, Camera camera, int d);

    void remake();
    void switch_material();


    void add_texture();
    void modify_circle(float dx, float dy, Camera camera, bool lor);
    void remove_texture();
    void add_repeat(bool t);
    void trans_tex_type();
    void reverse_tex();

    void Draw(Camera c, Shader s, glm::vec3 lp);
    void DrawFrame(Camera c, Shader s, bool d);
    void DrawTexFrame(Camera c, Shader s, bool d);

    void save_file();
    void load_from_file();

    DIYmodel();
    ~DIYmodel();

};