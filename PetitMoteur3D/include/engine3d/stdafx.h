// stdafx.h�: fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s
//

#pragma once
#define DIRECTINPUT_VERSION 0x0800
#define WIN32_LEAN_AND_MEAN		// Exclure les en-t�tes Windows rarement utilis�s
//#define _HAS_STD_BYTE 0
// Fichiers d'en-t�te Windows�:
#include <windows.h>

// Fichiers d'en-t�te C RunTime
#include <cassert>
#include <cstdint>
#include <cstring>
#include <tchar.h>

// Fichiers d'en-t�te C++ RunTime
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <codecvt>
#include <locale>


// TODO�: faites r�f�rence ici aux en-t�tes suppl�mentaires n�cessaires au programme
#define _XM_NO_INTRINSICS_
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "d3dx11effect.h"
#include <dinput.h>
#include <PxPhysicsAPI.h>
#include <PxSimulationEventCallback.h>
#include <opencv2/opencv.hpp>
#include <xaudio2.h>
