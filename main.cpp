#include <stdexcept>
#include <mutex>
#include <string>
#include <fstream>
#include <deque>
#include <chrono>
#include <thread>
extern "C"{
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <poll.h>
#include <errno.h>
#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
}
#include <iostream>
#include <vector>

using namespace std;

struct Color{
    unsigned char b, g, r, a;
    Color(unsigned char R = 0, unsigned char B = 0, unsigned char G = 0){
        b = B;
        r = R;
        g = G;
        a = 255;
    }
};

char const *path = "/dev/fb0";
class Framebuffer{
public:
    Color *data;
    size_t w, h, bpp, v_w, v_h;
    Framebuffer():
        data(nullptr),
        w(0),
        h(0),
        bpp(0),
        v_w(0),
        v_h(0)
    {

        int fb = open(path, O_RDONLY);
        if(fb < 0)
            throw runtime_error(string("can't open ") + path + string(": ") + strerror(errno));
        struct fb_fix_screeninfo fsinfo;
        if(ioctl(fb, FBIOGET_FSCREENINFO, &fsinfo) < 0){
            throw runtime_error(string("ioctl crash in fsinfo: ") + strerror(errno));
            close(fb);
        }
        struct fb_var_screeninfo vsinfo;
        if(ioctl(fb, FBIOGET_VSCREENINFO, &vsinfo) < 0){
            close(fb);
            throw runtime_error(string("ioctl crash in vsinfo: ") + strerror(errno));
        }
        bpp = vsinfo.bits_per_pixel;
        v_w = vsinfo.xres;
        v_h = vsinfo.yres;
        w = fsinfo.line_length * 8 / bpp;
        h = fsinfo.smem_len / fsinfo.line_length;
        close(fb);
        data = new Color[w * h];
    }
    int width() const noexcept{
        return v_w;
    }
    int heigh() const noexcept{
        return v_h;
    }
    Color *operator[](int i) noexcept{
        return data + w * (v_h - i - 1);
    }
    Color const *operator[](int i) const noexcept{
        return data + w * (v_h - i - 1);
    }

    void update(){
        int fb = open(path, O_WRONLY);
        if(fb < 0)
            throw runtime_error(string("can't open for write ") + path + string(": ") + strerror(errno));
        write(fb, data, w * v_h * sizeof(Color));
        close(fb);

    }
    void clear(){
        Color c(0, 0, 0);
        wchar_t wchar = *(wchar_t *)(&c);
        wmemset((wchar_t *)(data), wchar, w * v_h);
    }

    ~Framebuffer(){
        delete[] data;
    }
};
Framebuffer buffer;

using namespace std;

struct Vec2D{
protected:
    double pos[2];
public:

    Vec2D(double x = 0, double y = 0){
        pos[0] = x;
        pos[1] = y;
    }
    double& operator[](int i){
        return pos[i];
    }
    double abs()const{
        return sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
    }

    friend Vec2D operator+(Vec2D vec1, Vec2D vec2){
        return Vec2D(vec1[0] + vec2[0], vec1[1] + vec2[1]);
    }
    friend Vec2D operator-(Vec2D vec1, Vec2D vec2){
        return Vec2D(vec1[0] - vec2[0], vec1[1] - vec2[1]);
    }
    friend Vec2D operator*(double num, Vec2D vec1){
        return Vec2D(num * vec1[0], num * vec1[1]);
    }
    friend Vec2D operator*(Vec2D vec1, double num){
        return Vec2D(num * vec1[0], num * vec1[1]);
    }
    friend Vec2D operator/(Vec2D vec1, double num){
        return Vec2D(vec1[0] / num, vec1[1] / num);
    }
    friend double operator*(Vec2D vec1, Vec2D vec2){
        return vec1[0] * vec2[0] + vec1[1] * vec2[1];
    }

};

struct Atom{
protected:
    Vec2D Pos, V;
    double m, r;
public:
    Atom(Vec2D Pos, Vec2D V, double m = 1, double r = 1):
        Pos(Pos),
        V(V),
        m(m),
        r(r)
    {}
    Vec2D get_V(){
        return V;
    }
    Vec2D get_Pos(){
        return Pos;
    }
    void correct(double width, double heigh){
        if(Pos[0] - r < 0 || Pos[0] + r > width)
            V[0] *= -1;
        if(Pos[1] - r < 0 || Pos[1] + r > heigh)
            V[1] *= -1;
    }
    void move(double t){
        Pos[0] += t * V[0];
        Pos[1] += t * V[1];
    }
    void collide(Atom& another, double dT){
        Vec2D dist = another.Pos - Pos;
        if(dist.abs() < another.r + r){
            double F = another.r + r - dist.abs();
            F *= 5000;
            V = V - dist * (F * dT / m / dist.abs());
            another.V = another.V + dist * (F * dT / another.m / dist.abs());
        }
    }

};

void round(Vec2D pos, int R){
    for(int x = pos[0] - R; x < pos[0] + R; x++)
        for(int y = pos[1] - R; y < pos[1] + R; y++)
        {
            Vec2D buf(x - pos[0], y - pos[1]);
            if(x >= 0 && y >= 0 && buf.abs() <= R && x < buffer.width() && y < buffer.heigh())
                buffer[y][x] = Color(255, 0, int(255 * pow(buf.abs(), 3) / pow(R, 3)));
        }
}


int main()
{
    double heigh = buffer.heigh(), width = buffer.width();
    double speed_limit = 400;
    int n = 100;
    double r = 5, R = 40;
    double dT = r / (2 * speed_limit);
    double time_limit = 10;
    Vec2D Center(width / 2, heigh / 2);
    Vec2D Velocity(5, 5);
    Atom Bomb(Center, Velocity, 50, R);
    vector<Atom> atoms;
    for(int i = 0; i < n; ++i){
        double buf_v_x = rand() % int(speed_limit / 2) - speed_limit / 4;
        double buf_v_y = rand() % int(speed_limit / 2) - speed_limit / 4;
        double buf_x = rand() % int(width);
        double buf_y = rand() % int(heigh);
        Atom buf(Vec2D(buf_x, buf_y), Vec2D(buf_v_x, buf_v_y), 10, r);
        atoms.push_back(buf);
    }
    double t = 0;
    while(1){
        //t += dT;
        buffer.clear();
    for(int i = 0; i < n; ++i)
        round(atoms[i].get_Pos(), r);
    round(Bomb.get_Pos(), R);
    for(int i = 0; i < n; i++){
        atoms[i].correct(width, heigh);
        atoms[i].collide(Bomb, dT);
        atoms[i].move(dT);
    }
    Bomb.correct(width, heigh);
    Bomb.move(dT);

    buffer.update();
        //this_thread::sleep_for(chrono::milliseconds(1));
    }

    Color red(255);
    Color blue(0, 255);
    Color green(0, 0, 255);

    return 0;
}
