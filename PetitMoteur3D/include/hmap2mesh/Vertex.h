#pragma once
#ifndef VERTEX_H
#define VERTEX_H

using namespace DirectX;
using namespace std;


class CVertex
{
public:

	CVertex() = default;
	CVertex(const XMFLOAT3& position, const XMFLOAT3& normal);

	static UINT numElements;
	static D3D11_INPUT_ELEMENT_DESC layout[];

	const XMFLOAT3& getPosition() const { return m_Position; }
protected:

	// set as friend so it that the override can be used instead to write vertex
	friend ostream& operator<<(ostream& os, const CVertex& v);

	XMFLOAT3 m_Position;
	XMFLOAT3 m_Normal;
};

#endif