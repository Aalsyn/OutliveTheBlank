#pragma once
#include <DirectXMath.h>
#include <Texture.h>

using namespace DirectX;
struct ShadersParams
{
	XMMATRIX matWorldViewProj;	// la matrice totale 
	XMMATRIX matWorld;			// matrice de transformation dans le monde 
	XMVECTOR vLumiere; 			// la position de la source d'éclairage (Point)
	XMVECTOR vCamera; 			// la position de la caméra
	XMVECTOR vAEcl; 			// la valeur ambiante de l'éclairage
	XMVECTOR vAMat; 			// la valeur ambiante du matériau
	XMVECTOR vDEcl; 			// la valeur diffuse de l'éclairage 
	XMVECTOR vDMat; 			// la valeur diffuse du matériau 
	XMVECTOR vSEcl;
	XMVECTOR vSMat;
	float puissance;
	int bTex;
	XMFLOAT2 remplissage;
	XMVECTOR uvScaling;
};

struct MaterialBlock
{
	char NomFichierTexture[200];
	char NomMateriau[60];
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	float Puissance;
	bool transparent;
	float uScaling;
	float vScaling;
};

class CMaterialData
{
public:
	std::string NomFichierTexture;
	std::string NomMateriau;
	ID3D11ShaderResourceView* pTextureD3D;

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	float Puissance;
	bool transparent;
	float uScaling;
	float vScaling;

	CMaterialData()
		: pTextureD3D(nullptr)
		, Ambient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
		, Diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
		, Specular(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
		, Puissance(1.0f)
		, transparent(false)
		,uScaling(1.0f)
		,vScaling(1.0f)
	{
	}

	CMaterialData(std::string NomFichierTexture , std::string NomMateriau , float uScaling = 1.0f, float vScaling = 1.0f)
		: pTextureD3D(nullptr)
		, Ambient(XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f))
		, Diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
		, Specular(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f))
		, Puissance(1.0f)
		, transparent(false)
		, NomFichierTexture(NomFichierTexture)
		, NomMateriau(NomMateriau)
		, uScaling(uScaling)
		, vScaling(vScaling)
	{
	}

	void MatToBlock(MaterialBlock& mb)
	{
		strcpy_s(mb.NomFichierTexture, NomFichierTexture.c_str());
		strcpy_s(mb.NomMateriau, NomMateriau.c_str());
		mb.Ambient = Ambient;
		mb.Diffuse = Diffuse;
		mb.Specular = Specular;
		mb.Puissance = Puissance;
		mb.transparent = transparent;
		mb.uScaling = uScaling;
		mb.vScaling = vScaling;
	}

	void BlockToMat(MaterialBlock& mb)
	{
		NomFichierTexture.append(mb.NomFichierTexture);
		NomMateriau.append(mb.NomMateriau);
		Ambient = mb.Ambient;
		Diffuse = mb.Diffuse;
		Specular = mb.Specular;
		Puissance = mb.Puissance;
		transparent = mb.transparent;
		uScaling = mb.uScaling;
		vScaling = mb.vScaling;
	}
};

