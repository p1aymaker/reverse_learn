#include "game_hack.h"


ssize_t process_v(pid_t __pid,   struct iovec* __local_iov, unsigned long __local_iov_count, struct iovec* __remote_iov, unsigned long __remote_iov_count, unsigned long __flags) {
	return syscall(process_vm_readv_syscall, __pid, __local_iov, __local_iov_count, __remote_iov, __remote_iov_count, __flags);
}


ssize_t process_w(pid_t __pid,   struct iovec* __local_iov, unsigned long __local_iov_count, struct iovec* __remote_iov, unsigned long __remote_iov_count, unsigned long __flags) {
	return syscall(process_vm_writev_syscall, __pid, __local_iov, __local_iov_count, __remote_iov, __remote_iov_count, __flags);
}
int pvm(unsigned int address, void* buffer,int size) {
	struct iovec local[1];
	struct iovec remote[1];

	local[0].iov_base = (void*)buffer;
	local[0].iov_len = size;
	remote[0].iov_base = (void*)address;
	remote[0].iov_len = size;

	ssize_t bytes = process_v(pid, local, 1, remote, 1, 0);
	return bytes == size;
}

int pvm_w(unsigned int address, void* buffer,int size) {
	struct iovec local[1];
	struct iovec remote[1];

	local[0].iov_base = (void*)buffer;
	local[0].iov_len = size;
	remote[0].iov_base = (void*)address;
	remote[0].iov_len = size;

	ssize_t bytes = process_w(pid, local, 1, remote, 1, 0);
	return bytes == size;
}





pid_t getPid(char * name){
	char text[69];
	pid_t pid = 0;
	sprintf(text,"pidof %s",name);
	FILE *chkRun = popen(text, "r");
	if (chkRun){
		char output[10];
		fgets(output ,10,chkRun);
		pclose(chkRun);
		pid= atoi(output);
		}
	if (pid < 10) {
		DIR* dir = NULL;
		struct dirent* ptr = NULL;
		FILE* fp = NULL;
		char filepath[256];
		char filetext[128];
		dir = opendir("/proc");
		if (NULL != dir)
		{
			while ((ptr = readdir(dir)) != NULL)
			{
				if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
					continue;
				if (ptr->d_type != DT_DIR)
					continue;
				sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);
				fp = fopen(filepath, "r");
				if (NULL != fp)
				{
					fgets(filetext, sizeof(filetext), fp);
					if (strcmp(filetext, name) == 0)
					{
						fclose(fp);
						break;
					}
					fclose(fp);
				}
			}
		}
		if (readdir(dir) == NULL)
		{
			closedir(dir);
			return 0;
		}
		closedir(dir);
		pid= atoi(ptr->d_name);
	}
		return pid;
}


unsigned int getBase(){
    FILE *fp;
    unsigned int addr = 0;
    char filename[40], buffer[1024];
    snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    fp = fopen(filename, "rt");
    if (fp != NULL) {
        while (fgets(buffer, sizeof(buffer), fp)) {
            if (strstr(buffer, "libUE4.so")) {
                addr = (unsigned int)strtoull(buffer, NULL, 16);
                break;
            }
        }
        fclose(fp);
    }
    return addr;
}

unsigned int getA(unsigned int address) {
	unsigned int buff;
    pvm(address, &buff, SIZE);
    return buff;
}

int getI(unsigned int address) {
	int buff;
	pvm(address, &buff, 4);
	return buff;
}

char* getText(unsigned int addr) {
	static char txt[42];
	memset(txt, 0, 42);
	char buff[41];
	pvm(addr + 4+SIZE, &buff, 41);
	int i;
	for (i = 0; i < 41; i++) {
		if (buff[i] == 0)
			break;
		txt[i] = buff[i];
	}
	txt[i + 1] = '\0';
	return txt;
}

float getDistance(struct Vec3* mxyz,struct Vec3* exyz){
return sqrt ((mxyz->X-exyz->X)*(mxyz->X-exyz->X)+(mxyz->Y-exyz->Y)*(mxyz->Y-exyz->Y)+(mxyz->Z-exyz->Z)*(mxyz->Z-exyz->Z))/100;
}

float Dot(struct Vector3 lhs, struct Vector3 rhs){
	return (((lhs.X * rhs.X) + (lhs.Y * rhs.Y)) + (lhs.Z * rhs.Z));
}




struct Vector3 World2Screen(struct FMatrix viewMatrix, struct Vector3 pos) {
	struct Vector3 screen;
	float screenW = (viewMatrix.M[0][3] * pos.X) + (viewMatrix.M[1][3] * pos.Y) + (viewMatrix.M[2][3] * pos.Z) + viewMatrix.M[3][3];

	
	if (screenW < 0.01f) //w 1.0
		screen.Z = 1;
	else
		screen.Z = 0;


	float screenX = (viewMatrix.M[0][0] * pos.X) + (viewMatrix.M[1][0] * pos.Y) + (viewMatrix.M[2][0] * pos.Z) + viewMatrix.M[3][0];
	float screenY = (viewMatrix.M[0][1] * pos.X) + (viewMatrix.M[1][1] * pos.Y) + (viewMatrix.M[2][1]* pos.Z) +viewMatrix.M[3][1];

	screen.Y = height/2 - height/2 * screenY / screenW;
	screen.X = width/2 + width/2 * screenX / screenW;


	//printf("\ninit screenX  %0.2f screenY %0.2f screenW %0.2f",screenX,screenY,screenW);
	if (screen.X > 0 && screen.Y > 0 && screen.X <= width && screen.Y <= height&&screen.Z==0)
         printf("view matriex   Screen Location is %0.2f %0.2f\n ",screen.X,screen.Y);

	return screen;
}

struct FMatrix RotatorToMatrix(struct FRotator rotation) {
	float radPitch = rotation.Pitch * ((float)M_PI / 180.0f);
	float radYaw = rotation.Yaw * ((float)M_PI / 180.0f);
	float radRoll = rotation.Roll * ((float)M_PI / 180.0f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	struct FMatrix matrix;

	matrix.M[0][0] = (CP * CY);
	matrix.M[0][1] = (CP * SY);
	matrix.M[0][2] = (SP);
	matrix.M[0][3] = 0;

	matrix.M[1][0] = (SR * SP * CY - CR * SY);
	matrix.M[1][1] = (SR * SP * SY + CR * CY);
	matrix.M[1][2] = (-SR * CP);
	matrix.M[1][3] = 0;

	matrix.M[2][0] = (-(CR * SP * CY + SR * SY));
	matrix.M[2][1] = (CY * SR - CR * SP * SY);
	matrix.M[2][2] = (CR * CP);
	matrix.M[2][3] = 0;

	matrix.M[3][0] = 0;
	matrix.M[3][1] = 0;
	matrix.M[3][2] = 0;
	matrix.M[3][3] = 1;

	return matrix;
}

struct Vector2 
{
	float X;
	float Y;
};

struct Vector2 WorldToScreen(struct Vector3 worldLocation, struct MinimalViewInfo camViewInfo)
{
	struct FMatrix tempMatrix = RotatorToMatrix(camViewInfo.Rotation);
	struct Vector3 vAxisX;
	struct Vector3 vAxisY;
	struct Vector3 vAxisZ;
	struct Vector3 vDelta;
	struct Vector3 vTransformed;
	struct Vector2 result;

	vAxisX.X=tempMatrix.M[0][0];
	vAxisX.Y=tempMatrix.M[0][1];
	vAxisX.Z=tempMatrix.M[0][2];

	vAxisY.X=tempMatrix.M[1][0];
	vAxisY.Y=tempMatrix.M[1][1];
	vAxisY.Z=tempMatrix.M[1][2];

	vAxisZ.X=tempMatrix.M[2][0];
	vAxisZ.Y=tempMatrix.M[2][1];
	vAxisZ.Z=tempMatrix.M[2][2];

	vDelta.X=worldLocation.X-camViewInfo.Location.X;
	vDelta.Y=worldLocation.Y-camViewInfo.Location.Y;
	vDelta.Z=worldLocation.Z-camViewInfo.Location.Z;

	vTransformed.X=Dot(vDelta, vAxisY);
	vTransformed.Y=Dot(vDelta, vAxisZ);
	vTransformed.Z=Dot(vDelta,vAxisX);
	if (vTransformed.Z < 1.0f) 
	{
		vTransformed.Z = 1.0f;
	}

	float fov = camViewInfo.FOV;
	//printf("%0.2f",fov);
	float screenCenterX = width/2;//(width / 2.0f);
	float screenCenterY = height/2;//(height / 2.0f);

	result.X=(screenCenterX + vTransformed.X * (screenCenterX / tanf(fov * ((float)M_PI / 360.0f))) / vTransformed.Z);
	result.Y=(screenCenterY - vTransformed.Y * (screenCenterX / tanf(fov * ((float)M_PI / 360.0f))) / vTransformed.Z);
	 if (result.X > 0 && result.Y > 0 && result.X <= width && result.Y <= height)
          printf("Screen Location is %0.2f %0.2f \n ",result.X,result.Y);
	return result;
}




