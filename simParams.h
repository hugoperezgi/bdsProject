/*
RRegex : Report.[0-9]* of sim.[0-9]*: Generated Species->.[0-9]* Species Alive->.[0-9]* GloopCount->.[0-9]*
*/

//Field size, uint8_t max(255)

#define PLAYGROUND_SIZE_X 60
#define PLAYGROUND_SIZE_Y 60
#define MAX_GLOOPS (PLAYGROUND_SIZE_X*PLAYGROUND_SIZE_Y)/2
#define DEFAULT_FOODCHANCE 10
#define DEFAULT_BONUSFOOD 5
#define precGLOOP_THRESHOLD 20
#define REPORT_PERIOD 1000
#define DETAILED_REPORT true

//Gloop Stuff

#define DEFAULT_FOOD 75
#define DEFAULT_FOODCONSUMPTION 25
#define FOOD_THRESHOLD 75
#define DEFAULT_LIFESPAN 50
#define DEFAULT_MAXLIFESPAN 75
#define DEFAULT_OCROWDCOEF_FIXED 120
#define ENABLE_FIXED_ocCoef false

//Gloop Replication Params

#define DEFAULT_RCHANCE 20
#define MIN_RCHANCE 10
#define MAX_RCHANCE 30

#define DEFAULT_DCHANCE 15
#define MIN_DCHANCE 5

#define DEFAULT_MCHANCE 10
#define MIN_MCHANCE 5
#define MAX_MCHANCE 15

//Gloop Trait Params

#define DEFAULT_MOVRANGE 10
#define MIN_MOVRANGE 5
#define MAX_MOVRANGE 15

#define DEFAULT_RANGEOFVISION 5
#define MIN_RANGEOFVISION 0
#define MAX_RANGEOFVISION 15

#define DEFAULT_FOODBONUS 5
#define MIN_FOODBONUS 0
#define MAX_FOODBONUS 15

#define DEFAULT_HUNGERDEF 5
#define MIN_HUNGERDEF 0
#define MAX_HUNGERDEF 15
