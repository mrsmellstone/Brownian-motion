#ifndef NBODY_NBODYSCENE_HPP
#define NBODY_NBODYSCENE_HPP

#include "GLBall.h"

class NBodyScene {
public:
    virtual ~NBodyScene() = default;
    virtual unsigned int getNumberOfBodies() const = 0;
    virtual const GLBall& getBody(unsigned int number) const = 0;
    virtual void doTimeStep() = 0;
};

#endif //NBODY_NBODYSCENE_HPP
