#include <stdafx.h>
#include "SommetTerrain.h"
#include "FileHandler.h"
#include "ChargeurTerrain.h"

using namespace DirectX;

void CChargeurTerrain::lireFichierHeightmap(std::string imagePath)
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

void CChargeurTerrain::construireTerrain(float echelleXZ_, float echelleY_)
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

XMFLOAT3 CChargeurTerrain::computeNormalAt(int x, int z)
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

void CChargeurTerrain::calculerNormales()
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

void CChargeurTerrain::construireIndex()
{
    indicesSize = (dx-1) * (dz-1) * 6;
    pIndices = new int32_t[indicesSize]; // déclarer unsigned int* pIndices; dans

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

// gestion de la memoire

void CChargeurTerrain::release() {
    delete[] pVertices;
    delete[] pIndices;
    pVertices = nullptr;
    pIndices = nullptr;
    verticesSize = 0;
    indicesSize = 0;
}

void CChargeurTerrain::releaseTempDataFromMemory() {
    delete[] verticesData;
    verticesData = new XMFLOAT3[1];//TODO:: probably should be a null pointer
}