#include "GLBall.h"
#include "NBodyScene.h"

#include <cmath>
#include <vector>


class SampleBall : public GLBall {
protected:
    double vx;
    double vy;
    double x;
    double y;
    double r;

public:
    // Конструктор *ваш* - что нужно, то в нём и получаете.
    // Копировать этот конструктор *не* надо.
    SampleBall(double x, double y, double r, double vx, double vy)
            : x(x), y(y), r(r), vx(vx), vy(vy) {}

    // Ваши методы, никак не связанные с GLBall
    void move(double dt) {
        x += vx * dt;
        y += vy * dt;
    }

    double getX() const override {
        return x;
    }
    double getY() const override {
        return y;
    }
    double getR() const override {
        return r;
    }
    double& getVX(){
        return vx;
    }
    double& getVY(){
        return vy;
    }
    double getV() {
        return sqrt(vx * vx + vy * vy);
    }
    friend double abs(SampleBall const one, SampleBall const another){
        return sqrt((one.x - another.x) * (one.x - another.x)  + (one.y - another.y) * (one.y - another.y));
    }
};

class SampleScene : public NBodyScene {
protected:
    std::vector<SampleBall> bodies;
    double width, heigh;
    double dt;

public:
    
    SampleScene(double width, double heigh):
        width(width),
        heigh(heigh)
    {}

    unsigned int getNumberOfBodies() const override {
        return bodies.size();
    }

    const GLBall& getBody(unsigned int number) const override {
        return bodies.at(number);
    }
    
    void add_ball(SampleBall add){
        bodies.push_back(add);
    }
    
    void correct_time(){
        double max_v = 0;
        double min_r = bodies[0].getR();
        for(SampleBall& b : bodies){
            if(b.getV() > max_v)
                max_v = b.getV();
            if(b.getR() < min_r)
                min_r = b.getR();
        }
        dt = min_r / (5 * max_v);
    }

    void doTimeStep() override {
        for(SampleBall& b : bodies)
            b.move(dt);
    }

    double get_dt(){
        return dt;
    }

    void initScene() {
        bodies.push_back(SampleBall(0, 0, 10, 1, 0));
        bodies.push_back(SampleBall(15, 15, 1, 0, 1));
    }
    
    void correct_edge(){
        for(SampleBall& b : bodies){
            if(b.getX() - b.getR() < 0 || b.getX() + b.getR() > width)
                b.getVX() *= -1;
            if(b.getY() - b.getR() < 0 || b.getY() + b.getR() > heigh)
                b.getVY() *= -1;
        }
    }
    
    void correct_fat(){
        for(SampleBall& b : bodies){
            if(&b != &bodies[0]){
                double dist = abs(bodies[0], b);
                double rad_dist = bodies[0].getR() + b.getR();
                if(dist < rad_dist){
            // псевдосила
                    double F = rad_dist - dist;
                    F *= 5000;
                    double vec_x = b.getX() - bodies[0].getX();
                    double vec_y = b.getY() - bodies[0].getY();
            /* направление * (сила / масса(радиус)) * dt */
                    b.getVX() += (vec_x / dist) * (F / b.getR()) * dt;
                    b.getVY() += (vec_y / dist) * (F / b.getR()) * dt;
                    bodies[0].getVX() += (-vec_x / dist) * (F / bodies[0].getR()) * dt;
                    bodies[0].getVY() += (-vec_y / dist) * (F / bodies[0].getR()) * dt;
                }
            }
        }
    }
};

NBodyScene* getScene(double width, double heigh)
{
    SampleScene* s = new SampleScene(width, heigh);
    s->initScene();
    return s;
}

int main()
{
    SampleScene* scene = (SampleScene*)getScene(800, 600);
    double t = 0, Timelimit = 10;
    while(t < Timelimit) {
        scene->correct_time();
        scene->correct_edge();
        scene->correct_fat();
        scene->doTimeStep();
        t += scene->get_dt();
    }
    delete scene;
    return 0;
};
