#include "include/support.h"
uintptr_t getEntityAddr(uintptr_t base);


int GetActorLocation(uintptr_t base,struct Vector3* position){
	//SceneComponent* RootComponent;//[Offset: 0x120, Size: 4]
	return pvm(getA(base+0x120)+0x140,position,12);
}

uintptr_t getEntityAddr(uintptr_t base){
	return getA(getA(base + 0x491e6f0) + 0x20) + 0x70;
}



uintptr_t getViewMatriex(uintptr_t base,struct FMatrix* vMatrix){
	return pvm(getA(getA(base+0x4907EA0)+0x68)+0x590,vMatrix,64);
}


void  getMagicCoord(struct Vec2* result,struct Vec3* myposition,struct Vec3* enenmy){
	float v10=enenmy->Y-myposition->Y;
	float v11=enenmy->X-myposition->X;
	result->Y=atan2f(v10,v11)*180.0 /M_PI;
	result->X=atan2f((enenmy->Z)-myposition->Z,sqrtf(v10*v10+v11*v11))*180.0/M_PI;//+((distance / 88000.0) * ((distance / 88000.0) * 360.0)
	return;

}






int  main(int argc, char const *argv[]){
	/* code */


	uintptr_t enAddrPntr;
	//0x491e6f0
	char name[100];
	int LocationCuber,LocationPerson;
	pid=getPid("com.YourCompany.ThirdPerson");
	if (pid < 10){
		printf("\nGame is not running");
		return 0;
	}
	printf("\n Game Pid: %d", pid);
	uintptr_t base = getBase();
	printf("\n Base: %lX", base);

	enAddrPntr = getEntityAddr(base);

	struct Ulevel ulevel;
	pvm(enAddrPntr, &ulevel, sizeof(ulevel));
	if (ulevel.size < 1) {
		printf("\nWrong Entity Address");
	}
	//printf("\nEntity Address: %lX | Size: %d", enAddrPntr, ulevel.size);
	//printf("\nsize is %d",sizeof(uintptr_t));
	struct Vec3* xyz_person=(struct Vec3*)malloc(12);
	memset(xyz_person,0,12);

	//0x48711b4

	uintptr_t gname_buff[30];
	uintptr_t gname = getA(base + 0x48711b4);
	pvm(gname, &gname_buff, sizeof(gname_buff));
	//printf("\nUlevel addr %lx", ulevel.addr);
	uintptr_t Crontroller=0;


	struct Vector2 screen_location;

	uintptr_t ulocal_player=getA(base+0x4907EA0);//ULocalPlayer::Exec

	uintptr_t player_controller=getA(ulocal_player+0x20);

	printf("\n player_controller addr is %lx",player_controller);
	uintptr_t camera_manager=getA(player_controller+0x30c);


	while(1){
    	for (int i = 0; i < ulevel.size; i++) {
            uintptr_t  pBase = getA(ulevel.addr + i * 4);
            //printf("\nPbase is %lx",pBase);
          
            int ids = getI(pBase + 8 + 2 * SIZE);
            //printf("\nids is %x",ids);
            int page = ids / 0x4000;
            int index = ids % 0x4000;
            if (page < 0 || page>30){
            	//printf("\npage is over");
                continue;
            }
            	
            if (gname_buff[page] == 0){
            	//printf("\nis zero\n");
                gname_buff[page] = getA(gname + page * SIZE);
            }  	
            strcpy(name, getText(getA(gname_buff[page] + index * SIZE)));
            if (strlen(name) < 5){
            	//printf("\nname too short");
				continue;
            }
			if(!strcmp(name,"ThirdPersonCharacter")||!strcmp(name,"ThirdPersonCharacter2")||!strcmp(name,"ThirdPersonCharacter3")){
				printf("%s\n",name);
				struct Vector3 xyz_EditorCube;
				struct MinimalViewInfo POV;
				struct FMatrix viewMatrix;
				pvm(camera_manager+0x320,&POV,sizeof(struct MinimalViewInfo));
				GetActorLocation(pBase,&xyz_EditorCube);
				WorldToScreen(xyz_EditorCube,POV);
				getViewMatriex(base,&viewMatrix);
				World2Screen(viewMatrix,xyz_EditorCube);
				printf("=================================\n");
    		}
    	}
    
		sleep(2);
	}
	return 0;
}