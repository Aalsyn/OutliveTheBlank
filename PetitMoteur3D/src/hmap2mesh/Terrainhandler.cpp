#include <stdafx.h>
#include "SommetTerrain.h"
#include <FileHandler.h>
#include "TerrainHandler.h"

using namespace DirectX;

void CTerrainHandler::lireFichierHeightmap(std::string imagePath)
{
    // Read the image file as
    // imread("default.jpg");
    image = imread(imagePath, IMREAD_GRAYSCALE);

    // Error Handling
    if (image.empty()) {
        cout << "Image File "
            << "Not Found" << endl;

        // wait for any key press
        cin.get();
        return;
    }

    // save image size
    dx = image.cols;
    dz = image.rows;
    dy = 255;

}

void CTerrainHandler::construireTerrain(float echelleXZ_, float echelleY_)
{
    verticesSize = dx * dz;
    verticesData = new XMFLOAT3[verticesSize];
    const float xShift = ((float) - dx / 2);
    const float yShift = ((float) - dy / 2);
    const float zShift = ((float) - dz / 2);
    for (int i = 0; i < dz; ++i){
        int columnDelta = i * dx;
        for (int j = 0; j < dx; ++j){
            verticesData[columnDelta + j ] = 
                XMFLOAT3(
                    (xShift + j) * echelleXZ_,
                    ((yShift + image.at<uchar>(i, j)) * echelleY_), //z is a float value between 0 and 255 at vertex location
                     (zShift + i) * echelleXZ_)
                ;
        }
    }
    // release image matrix to free some memory
    image.release();
}

XMFLOAT3 CTerrainHandler::computeNormalAt(int x, int z)
{
    XMVECTOR n1, n2, n3, n4;

    XMVECTOR v1, v2, v3, v4;

    n1 = n2 = n3 = n4 = XMVectorSet(0, 0, 0, 0); // Le Z est le haut
    // v1 = p1 – p0, etc...

    if (z < dz - 1) v1 = obtenirPosition(x, z + 1) - obtenirPosition(x, z);
    if (x < dx - 1) v2 = obtenirPosition(x + 1, z) - obtenirPosition(x, z);
    if (z > 0) v3 = obtenirPosition(x, z - 1) - obtenirPosition(x, z);
    if (x > 0) v4 = obtenirPosition(x - 1, z) - obtenirPosition(x, z);
   
    // les produits vectoriels
    if (z < dz - 1 && x < dx - 1) n1 = XMVector3Cross(v1, v2);
    if (z > 0 && x < dx - 1) n2 = XMVector3Cross(v2, v3);
    if (z > 0 && x > 0) n3 = XMVector3Cross(v3, v4);
    if (z < dz - 1 && x > 0) n4 = XMVector3Cross(v4, v1);
    n1 = n1 + n2 + n3 + n4;
    n1 = XMVector3Normalize(n1);
    XMFLOAT3 resultat;
    XMStoreFloat3(&resultat, n1);
    return resultat;
}

void CTerrainHandler::calculerNormales()
{
    const float M = static_cast<float>((dx > dz) ? dx : dz);
    pVertices = new CSommetTerrain[dx * dz];
    for (int i = 0; i < dz; ++i) {
        int columnDelta = i * dx;
        for (int j = 0; j < dx; ++j) {
            pVertices[j + columnDelta] = CSommetTerrain(verticesData[j + columnDelta], computeNormalAt(j, i),{ (float)(j)/M ,(float)(i)/M });
        }
    }
}

void CTerrainHandler::construireIndex()
{
    indicesSize = (dx-1) * (dz-1) * 6;
    pIndices = new int[indicesSize]; // déclarer unsigned int* pIndices; dans

    int k = 0;
    for (int z = 0; z < dz - 1; ++z)
    {
        for (int x = 0; x < dx - 1; ++x)
        {
            // L'important ici est d'utiliser la même formule pour identifier
            // les sommets qu'au moment de leur création
            pIndices[k++] = z * dx + x;
            pIndices[k++] = (z + 1) * dx + (x + 1);
            pIndices[k++] = z * dx + (x + 1);
            pIndices[k++] = z * dx + x;
            pIndices[k++] = (z + 1) * dx + x;
            pIndices[k++] = (z + 1) * dx + (x + 1);
        }
    }

    releaseTempDataFromMemory();
}

int CTerrainHandler::enregistrerTout()
{
    // ask for a saving path, and save the 3dmodel to the file
    std::string meshFilePath;
    if (!FileHandler::get().retrieveMeshSavingPathFE(NULL, meshFilePath)) {
        return 0;
    }
    CTerrainHandler& tg = CTerrainHandler::get();
    FileHandler::writeMeshToFileAsTXT(
        meshFilePath,
        tg.pVertices,
        tg.pIndices,
        tg.verticesSize,
        tg.indicesSize);
    return 1;
}

//accesseurs
float computeDistance(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
    XMVECTOR vector1 = XMLoadFloat3(&v1);
    XMVECTOR vector2 = XMLoadFloat3(&v2);

    XMVECTOR vectorSub = XMVectorSubtract(vector1, vector2);
    XMVECTOR length = XMVector3Length(vectorSub);

    float distance = 0.0f;
    XMStoreFloat(&distance, length);
    return distance;
}

double CTerrainHandler::getTerrainY(const float x, const float z) const
{
    auto vShift = [](double val) {return (0.0 < val) - (val < 0.0); };
    const int xIndex = static_cast<int>(x / echelleXZ + (dx / 2));
    const double xScaled = x / echelleXZ + (dx / 2.0);
    const int zIndex = static_cast<int>(z / echelleXZ + (dz / 2));
    const double zScaled = z / echelleXZ + (dz / 2.0);

    if (xIndex > 0 && xIndex < dx && zIndex > 0 && zIndex < dz) {

       //for(CVertex& vert = pVertices[xIndex + zIndex * dx],int i = 0;
        
        
        const XMFLOAT3& v1 = pVertices[xIndex + vShift(xScaled - xIndex) + zIndex * dx].getPosition();
        const XMFLOAT3& v2 = pVertices[xIndex + ((zIndex+vShift(zScaled - zIndex))* dx)].getPosition();
        const XMFLOAT3& v3 = pVertices[xIndex + zIndex * dx].getPosition();
        
       /* float d1 = computeDistance( v1, { x,v1.y,z } );
        float d2 = computeDistance( v2, { x,v2.y,z } );
        float d0 = computeDistance( v3, { x,v3.y,z });*/

        // compute Barycentric weights
        const float u = 
            ((v2.z - v3.z)*(x - v3.x) + (v3.x - v2.x)*(z - v3.z))
            /
            ((v2.z - v3.z)*(v1.x - v3.x) + (v3.x-v2.x)*(v1.z-v3.z))
            ;
        const float v = 
            ((v3.z - v1.z) * (x - v3.x) + (v1.x - v3.x) * (z - v3.z))
            /
            ((v2.z - v3.z) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.z - v3.z))
            ;
        
        float y = (1 - u - v) * v3.y + u * v1.y + v * v2.y;
        return y;
    }
    return -1000;
}

// gestion de la memoire

void CTerrainHandler::release() {
    delete[] pVertices;
    delete[] pIndices;
    pVertices = nullptr;
    pIndices = nullptr;
    verticesSize = 0;
    indicesSize = 0;
}

void CTerrainHandler::releaseTempDataFromMemory() {
    delete[] verticesData;
    verticesData = new XMFLOAT3[1];//TODO:: probably should be a null pointer
}

void main_() { // ce main est factice
    // le code est "driven" par un UI donc l'entry point du code de generation terrain ce trouve dans la methode void generateTerrain(std::string imagePath) dans TerrainHandler.h
}