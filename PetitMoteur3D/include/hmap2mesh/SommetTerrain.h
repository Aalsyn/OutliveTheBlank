#pragma once
#ifndef VERTEX_H
#define VERTEX_H

using namespace DirectX;
using namespace std;


class CSommetTerrain
{
public:

	CSommetTerrain() = default;
	CSommetTerrain(const XMFLOAT3& position, const XMFLOAT3& normal);
	CSommetTerrain(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& coordTex);

	static UINT numElements;
	static D3D11_INPUT_ELEMENT_DESC layout[];

	const XMFLOAT3& getPosition() const { return m_Position; }
protected:

	// set as friend so it that the override can be used instead to write vertex
	friend ostream& operator<<(ostream& os, const CSommetTerrain& v);

public :
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Normal;
	XMFLOAT2 m_CoordTex = XMFLOAT2(0.0f, 0.0f);
};

#endif