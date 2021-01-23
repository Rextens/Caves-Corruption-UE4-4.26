// Fill out your copyright notice in the Description page of Project Settings.


#include "NarrowCorridors.h"

NarrowCorridors::NarrowCorridors()
{
	frequency = 2;
	meshID = 0;
}

float NarrowCorridors::generator(const FVoxelFastNoise &noise, const v_flt &X, const v_flt& Y, const v_flt& Z)
{
	float vX = cos(noise.GetPerlin_3D(X + 0.5f, Y + 0.5f, Z + 0.5f, 0.01f) * PI * 2);
	float vY = -sin(noise.GetPerlin_3D(X + 0.5f, Y + 0.5f, Z + 0.5f, 0.01f) * PI * 2);

	if (vX > 0 && vY > 0)
	{
		return vX * vY;
	}
	else
	{
		return -1;
	}
	//return noise.GetValueFractal_3D(vX * 0.01f + 0.5f, vY * 0.01f + 0.5f, vZ * 0.01f + 0.5f, 1.02f, 12);
}

