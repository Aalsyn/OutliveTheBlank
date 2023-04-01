#pragma once

#include <string>
#include <Terrain.h>
#include <SommetTerrain.h>


using namespace std;

class FileHandler {
    FileHandler(){};

public:

    FileHandler(const FileHandler&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;
    
    static FileHandler& get() {
        static FileHandler singleton;
        return singleton;
    }

public:

    // TODO :: should be changed to static 
    int retrieveImagePathFE(HWND hWnd, std::string& imagePath);
    int retrieveMeshSavingPathFE(HWND hWnd, std::string& imagePath);
    int retrieveMeshLoadingPathFE(HWND hWnd, std::string& imagePath);

    static void writeMeshToFileAsTXT(
        string path, 
        CSommetTerrain* pVertices,
        int* index,
        int meshVertexArrSize,
        int indexSize
    );

    static int loadMeshFromTXTFile( string path );


};