#include "StdAfx.h"
#include "Vertex.h"
#include <iostream>

// stream operator to write the vertex 
ostream& operator<<(ostream& os, const CVertex& v)
{
	os << v.m_Position.x << ' ' << v.m_Position.y << ' ' << v.m_Position.z;
	os << ' ';
	os << v.m_Normal.x << ' ' << v.m_Normal.y << ' ' << v.m_Normal.z;
	return os;
}

D3D11_INPUT_ELEMENT_DESC CVertex::layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	 D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
	 D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

UINT CVertex::numElements = ARRAYSIZE(CVertex::layout);

CVertex::CVertex(const XMFLOAT3& position, const XMFLOAT3& normal) :
	m_Position(position), 
	m_Normal(normal) 
{}

