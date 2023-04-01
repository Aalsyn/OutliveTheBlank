#include "ImageHandler.h"
//#include "util.h"

using namespace PM3D;

int ImageHandler::retrieveImagePathFE(HWND hWnd, std::string& imagePath)
{
    
    OPENFILENAME ofn = { 0 };
    std::wstring path(MAX_PATH, '\0');
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = &path[0];
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 256;
    ofn.lpstrFilter = L"Image files (*.jpg;*.png;*.bmp)\0*.jpg;*.png;*.bmp\0"
        "All files\0*.*\0";
    ofn.nFilterIndex = 1;

    
    if (!GetOpenFileName(&ofn)) {
        return 0;
    }
    std::string str;
    std::transform(path.begin(), path.end(), std::back_inserter(str), [](wchar_t c) {
        return (char)c;
        });
    imagePath = str;

    return 1;

}
