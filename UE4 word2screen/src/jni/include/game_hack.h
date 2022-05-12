#include <sys/uio.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include<math.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <sys/un.h>
#include <time.h>
#include <ctype.h>
//#include <iostream>


#define M_PI 3.14159265358979323846
#define SIZE 4
#if defined(__arm__)
int process_vm_readv_syscall = 376;
int process_vm_writev_syscall = 377;
#elif defined(__aarch64__)
int process_vm_readv_syscall = 270;
int process_vm_writev_syscall = 271;
#elif defined(__i386__)
int process_vm_readv_syscall = 347;
int process_vm_writev_syscall = 348;
#else
int process_vm_readv_syscall = 310;
int process_vm_writev_syscall = 311;
#endif

#define LEN sizeof(struct MAPS)



int pid = 0;


struct Vec4 {
    float  X, Y, Z, W;
};
struct Vec3 {
    float X, Y, Z;
};
struct Vec2 {
    float X, Y;
};

struct Ulevel {
    uintptr_t addr;
    int size;
};

struct D3DMatrix
{

    float _11, _12, _13, _14;

    float _21, _22, _23, _24;

    float _31, _32, _33, _34;

    float _41, _42, _43, _44;

};


struct FMatrix 
{
    float M[4][4];
};




struct Vector3 
{
    float X;
    float Y;
    float Z;

};

struct FRotator
{
    float Pitch;
    float Yaw;
    float Roll;
};

struct MinimalViewInfo
{
    struct Vector3 Location;
    struct FRotator Rotation;
    float FOV;
};


int width=2392;
int height=1440;
struct cube_data{
    struct Vec3* pos;
    float Distance;

};


struct data{
    int cubecount;
    struct cube_data* cube[30];
};