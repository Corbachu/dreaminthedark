#ifndef ALONEFLOOR_H
#define ALONEFLOOR_H


#include "alonefile.h"
extern "C" {
#include "../src/vars.h"
#include "../src/room.h"
#include "../src/floor.h"
#include "../src/hqr.h"
}


class AloneFloor
{
public:
    AloneFloor();
    ~AloneFloor();
    bool load(AloneFile *rooms,AloneFile *cams);
    void exportCollada();

    roomDataStruct* roomDataTable;
    AloneFile *mRooms;
    AloneFile *mCams;
    u32 expectedNumberOfRoom;
    u32 expectedNumberOfCamera;

    std::string hardCol2collada(ZVStruct* zvData, int index,float roomX,float roomY,float roomZ);
    std::string sceZone2collada(ZVStruct* zvData, int index,float roomX,float roomY,float roomZ);
    std::string cam2collada_lib(cameraDataStruct* cam, int index);
    std::string cam2collada_node(cameraDataStruct* cam, int index, float roomX, float roomY, float roomZ);


};

#endif // ALONEFLOOR_H
