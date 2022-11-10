#include "common.h"
#include "vars.h"

enum gameTypeEnum gameId;

u8* currentFoundBody;
int currentFoundBodyIdx;
int statusVar1;

hqrEntryStruct* hqrUnk;

int videoMode;
int musicConfigured;
int musicEnabled;

u8* aux;
u8* aux2;
u8* bufferAnim;

u8* screen;

int screenBufferSize;
int unkScreenVar2;

int numCVars;
short int* CVars;

u8* priority;

u8* fontData;

u8* aitdBoxGfx;

u8 palette[256*3];

//OSystem osystem;

u8 unkScreenVar[320*200];

u8 rgbaBuffer[320*200*4];

unsigned int timer;
volatile unsigned int timeGlobal;

int currentMenuLeft;
int currentMenuTop;
int currentMenuRight;
int currentMenuBottom;

textEntryStruct* tabTextes;
char* systemTextes;

char inputKey = 0;
char input1 = 0;
char input2 = 0;
char input3;
char input4;
char button;

char* languageNameTable[] =
{
  "FRANCAIS",
  "ITALIANO",
  "ENGLISH",
  "ESPAGNOL",
  "DEUTSCH",
  NULL,
};

char languageNameString[20] = "";

regularTextEntryStruct textTable[NUM_MAX_TEXT];

int readVar;

int hqrKeyGen = 0;

u8* screenSm1;
u8* screenSm2;
u8* screenSm3;
u8* screenSm4;
u8* screenSm5;

actorStruct actorTable[NUM_MAX_ACTOR];

short int currentCameraTarget;

int fileSize;

hqrEntryStruct* listBody;
hqrEntryStruct* listAnim;
hqrEntryStruct* listLife;
hqrEntryStruct* listTrack;
hqrEntryStruct* listMatrix;

short int maxObjects;

objectStruct* objectTable;

short int* vars;

int varSize;

messageStruct messageTable[NUM_MAX_MESSAGE];

short int currentMusic;
int action;

boxStruct genVar2[15]; // recheckSize
boxStruct genVar4[50];
boxStruct *genVar1;
boxStruct *genVar3;

int genVar5;
int genVar6;
int genVar7;
int nextMusic;
short int genVar9;
short int giveUp;
short int inHand;
short int lightVar1;
int lightVar2;
short int numObjInInventory;
int soundVar1;
int soundVar2;
short int statusScreenAllowed;

u8* etageVar0 = NULL;
u8* etageVar1 = NULL;

int changeFloor;
short int currentCamera;
short int currentEtage;
int needChangeRoom;

char* cameraPtr;
roomDefStruct* pCurrentRoomData;

short int currentDisplayedRoom;
int mainVar1;
int numCameraInRoom;
int numCameraZone;
u8* cameraZoneData;
int numRoomZone;
u8* roomZoneData;
u8* roomVar5[NUM_MAX_CAMERA_IN_ROOM];
int startGameVar1;

int transformX;
int transformY;
int transformZ;
int transformXCos;
int transformXSin;
int transformYCos;
int transformYSin;
int transformZCos;
int transformZSin;
bool transformUseX;
bool transformUseY;
bool transformUseZ;

int translateX;
int translateY;
int translateZ;

int cameraCenterX;
int cameraCenterY;
int cameraX;
int cameraY;
int cameraZ;

char currentCameraVisibilityList[30];

int actorTurnedToObj = 0;

int currentProcessedActorIdx;
actorStruct* currentProcessedActorPtr;

int currentLifeActorIdx;
actorStruct* currentLifeActorPtr;
int currentLifeNum;

u8* currentLifePtr;

int setupCameraVar1;

#ifdef USE_GL
float renderPointList[6400];
#else
short int renderPointList[6400];
#endif

s32 numActorInList;
s32 sortedActorTable[NUM_MAX_ACTOR];

s32 angleCompX;
s32 angleCompZ;
s32 angleCompBeta;

s32 bufferAnimCounter = 0;

s32 animCurrentTime;
s32 animKeyframeLength;
s32 animMoveX;
s32 animMoveY;
s32 animMoveZ;
s32 animRot1;
s32 animRot2;
s32 animRot3;
u8* animVar1;
u8* animVar3;
u8* animVar4;

short int newFloor;

s32 paletteVar;

u8 cameraBuffer[256];
u8 cameraBuffer2[256];
u8 cameraBuffer3[400];
u8 cameraBuffer4[400];

u8* cameraBufferPtr = cameraBuffer;
u8* cameraBuffer2Ptr = cameraBuffer2;
u8* cameraBuffer3Ptr = cameraBuffer3;

s32 overlaySize1;
s32 overlaySize2;

s32 bgOverlayVar1;

short int newRoom;

short int inventory[INVENTORY_SIZE];

short int shakeVar1;
short int shakingAmplitude;
u32 timerFreeze1;

hardColStruct* hardColTable[10];

short int hardColVar1;
short int hardColVar2;

ZVStruct hardClip;

char* listBodySelect[] = {
  "LISTBODY",
  "LISTBOD2",
};

char* listAnimSelect[] = {
  "LISTANIM",
  "LISTANI2",
};

saveEntry saveTable[40];

s32 hqrVar1 = 0;
s32 mainVar3 = 4000;
s32 mainVar2 = -2000;

s32 clipLeft = 0;
s32 clipTop = 0;
s32 clipRight = 319;
s32 clipBottom = 119;

hqrEntryStruct* listMus;
hqrEntryStruct* listSamp;

#ifdef INTERNAL_DEBUGGER
backgroundModeEnum backgroundMode = backgroundModeEnum_2D;
#endif

void freeAll(void)
{
/*  HQR_Free(hqrUnk);

  HQR_Free(listSamp);

  HQR_Free(listMus);

  free(languageData);

  free(tabTextes);

  free(priority);

  free(aitdBoxGfx);

  free(fontData);

  free(bufferAnim);

  if(aux != aux3)
  {
    free(aux);
  }

  free(aux2);*/

  //TODO: implement all the code that restore the interrupts & all
}
