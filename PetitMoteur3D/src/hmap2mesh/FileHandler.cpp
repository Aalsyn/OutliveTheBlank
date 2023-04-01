#include <stdafx.h>
#include <FileHandler.h>
#include <tchar.h>
#include <algorithm>
#include <commdlg.h>
#include <fstream>
#include <iterator>
#include <MoteurWindows.h>
#include "TerrainHandler.h"

int FileHandler::retrieveImagePathFE(HWND hWnd, std::string& imagePath)
{

    LPSTR filebuff = new char[256];
    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd; //Handle to the parent window
    ofn.lpstrFilter = "Image files (*.jpg;*.png;*.bmp)\0*.jpg;*.png;*.bmp\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.lpstrFile = filebuff;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 256;
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Select An Image File\0";
    ofn.nMaxFileTitle = static_cast<DWORD>(strlen("Select an image file\0"));
    ofn.Flags = OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_EXPLORER;


    if (!GetOpenFileNameA(&ofn)) {
        return 0;
    }

    imagePath = ofn.lpstrFile;
    return 1;

}

int FileHandler::retrieveMeshSavingPathFE(HWND hWnd, std::string& imagePath)
{
    LPSTR filebuff = new char[256];
    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd; //Handle to the parent window
    ofn.lpstrFilter = "Image files (*.txt)\0*.txt\0";
    ofn.lpstrDefExt = ".txt";
    ofn.lpstrCustomFilter = NULL;
    ofn.lpstrFile = filebuff;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 256;
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Select where to save 3D model file\0";
    ofn.nMaxFileTitle = static_cast<DWORD>(strlen("Select where to save 3D model file\0"));
    ofn.Flags = OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_EXPLORER;


    if (!GetSaveFileNameA(&ofn)) {
        return 0;
    }
    imagePath = ofn.lpstrFile;
    return 1;
}

int FileHandler::retrieveMeshLoadingPathFE(HWND hWnd, std::string& imagePath)
{
    LPSTR filebuff = new char[256];
    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd; //Handle to the parent window
    ofn.lpstrFilter = "Image files (*.txt)\0*.txt\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.lpstrFile = filebuff;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 256;
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Select A 3D model File\0";
    ofn.nMaxFileTitle = static_cast<DWORD>(strlen("Select A 3D model File\0"));
    ofn.Flags = OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_EXPLORER;


    if (!GetOpenFileNameA(&ofn)) {
        return 0;
    }

    imagePath = ofn.lpstrFile;
    return 1;
}

void FileHandler::writeMeshToFileAsTXT( string path , CSommetTerrain* pVertices , int* index, int meshVertexArrSize, int indexSize) {
    ofstream out{ path,ios::binary };

    // write
    string meshVertexArrSizeIdentifier = "[meshVertexArrSize]:\n" + std::to_string(meshVertexArrSize) + "\n";
    copy(meshVertexArrSizeIdentifier.begin(),
        meshVertexArrSizeIdentifier.end(),
        ostream_iterator<char>{ out, "" }
    );
    // write
    string indexSizeIdentifier = "[indexSize]:\n"+ std::to_string(indexSize) + "\n";
    copy(indexSizeIdentifier.begin(),
        indexSizeIdentifier.end(),
        ostream_iterator<char>{ out, "" }
    );
    // write
    string vertexIdentifier = "[Vertex]:\n";
    copy(vertexIdentifier.begin(),
        vertexIdentifier.end(),
        ostream_iterator<char>{ out, "" }
    );
    copy(pVertices,
        pVertices+meshVertexArrSize,
        ostream_iterator<CSommetTerrain>{ out, "\n" }
    );
    string IndexIdentifier = "[Index]:\n";
    copy(IndexIdentifier.begin(),
        IndexIdentifier.end(),
        ostream_iterator<char>{ out, "" }
    );
    copy(index,
        index + indexSize,
        ostream_iterator<int>{ out, "\n" }
    );
}

int FileHandler::loadMeshFromTXTFile(string path)
{
    int& meshVertexArrSize = CTerrainHandler::get().verticesSize;
    int& indexSize = CTerrainHandler::get().indicesSize;

    CSommetTerrain*& pVertices = CTerrainHandler::get().pVertices;
    int*& index = CTerrainHandler::get().pIndices;

    ifstream in{ path };
    int sucessRate = 0;
    for (string line; getline(in, line);) {
        if (line == "[meshVertexArrSize]:") {
            std::vector<int> i = {};
            copy_n(istream_iterator<int>{in},
                1, back_inserter(i));
            meshVertexArrSize = i.back();
            pVertices = new CSommetTerrain[meshVertexArrSize]; // we could save one operation but it is more explicit that way
            if(meshVertexArrSize>0)
                sucessRate++;
        }
        if (line == "[indexSize]:") {
            std::vector<int> i;
            copy_n(istream_iterator<int>(in),
                1, back_inserter(i));
            indexSize = i.back();
            index = new int[indexSize];
            if (indexSize > 0)
                sucessRate++;
        }
        if (line == "[Vertex]:") {
            float x, y, z, nx, ny, nz , u ,v;
            int k = 0;
            while (k != meshVertexArrSize) {
                in >> x >> y >> z >> nx >> ny >> nz >> u >> v;
                pVertices[k] = CSommetTerrain({ x,y,z }, { nx,ny,nz }, {u,v});
                k++;
            }
            in.clear();
            sucessRate++;
        }
        if (line == "[Index]:") {
            std::vector<int> i;
            copy_n(istream_iterator<int>(in), indexSize, back_inserter(i));
            index = new int[indexSize];
            int k = 0;
            for (int ind : i) {
                index[k++] = ind;
            }
            in.clear();
            sucessRate++;
        }
    }
    if (sucessRate != 4)
        return 0;
    return 1;
}
