#ifndef NBODY_GLBALL_HPP
#define NBODY_GLBALL_HPP

class GLBall {
public:
    virtual ~GLBall() = default;
    virtual double getX() const = 0;
    virtual double getY() const = 0;
    virtual double getR() const = 0;
};

#endif //NBODY_GLBALL_HPP
