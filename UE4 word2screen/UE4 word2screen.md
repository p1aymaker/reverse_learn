# UE4 3D坐标转2D坐标

## 视角相机

ulocalplayer->playercontroller->cameramanager->POV

```c
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
```

## 投影矩阵

ulocalplayer->FsceneViewState->viewprojectionmartiex

矩阵特征 

```
0.86 -0.1 0.0  -0.49
-0.5 -0.17 0 -0.85
-2.6 1.6 0 -0.12
-1131.72 -5003.95 10.0 -285.13
```

```c
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
```

