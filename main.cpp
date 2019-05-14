#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

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
    bool is_near(Atom another){
        Vec2D dist = another.Pos - Pos;
        if(dist.abs() < another.r + r)
            return true;
        else
            return false;
    }
    void correct(double width, double heigh){
        if(Pos[0] - r < 0 || Pos[0] + r > width)
            V[0] *= -1;
        if(Pos[1] - r < 0 || Pos[1] + r > heigh)
            V[1] += -1;
    }
};

int main()
{
    double heigh = 500, width = 500;
    double speed_limit = 400;
    int n = 50;
    double r = 1, R = 20;
    double dT = r / (2 * speed_limit);
    double time_limit = 10;
    Vec2D Center(width / 2, heigh / 2);
    Vec2D Velocity;
    Atom Bomb(Center, Velocity, 50, R);
    vector<Atom> atoms;
    for(int i = 0; i < n; ++i){
        double buf_v_x = rand() % int(speed_limit / 2);
        double buf_v_y = rand() % int(speed_limit / 2);
        double buf_x = rand() % int(width);
        double buf_y = rand() % int(heigh);
        Atom buf(Vec2D(buf_x, buf_y), Vec2D(buf_v_x, buf_v_y), 1, r);
        atoms.push_back(buf);
    }
    double t = 0;
    while(t < time_limit){
        t += dT;
    }
    return 0;
}
