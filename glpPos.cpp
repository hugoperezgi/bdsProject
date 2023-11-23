#include <stdint.h>

class glpPos{
private:

    void calcDist();

public:

    uint8_t gX,gY;
    uint8_t fX,fY;
    uint8_t dX,dY;
    uint8_t distance;

    void setFood(uint8_t,uint8_t);

    glpPos();
    glpPos(uint8_t,uint8_t);
    glpPos(const glpPos&);
    ~glpPos();
};

glpPos::glpPos(uint8_t x, uint8_t y){
    this->gX=x;
    this->gY=y;
    this->distance=0;
    this->fX=0;
    this->fY=0;
    this->dX=0;
    this->dY=0;
}

glpPos::glpPos(const glpPos& pos){

    this->gX=pos.gX;
    this->gY=pos.gY;

    this->fX=pos.fX;
    this->fY=pos.fY;

    this->dX=pos.dX;
    this->dY=pos.dY;

    this->distance=pos.distance;
}

glpPos::~glpPos(){}
glpPos::glpPos(){
    this->gX=0;
    this->gY=0;
    this->distance=0;
    this->fX=0;
    this->fY=0;
    this->dX=0;
    this->dY=0;
}

void glpPos::setFood(uint8_t x, uint8_t y){
    this->fX=x;
    this->fY=y;
    this->calcDist();
}

void glpPos::calcDist(){

    this->fX>=this->gX ? this->dX = this->fX-this->gX : this->dX= this->gX-this->fX;
    this->fY>=this->gY ? this->dY = this->fY-this->gY : this->dY= this->gY-this->fY;

    this->distance = this->dX + this->dY;
}
