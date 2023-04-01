#pragma once

namespace PM3D
{

class CDIManipulateur
{
public:
	CDIManipulateur();
	~CDIManipulateur();

	bool CDIManipulateur::Init(HINSTANCE hInstance, HWND hWnd);
	void StatutClavier();
	bool ToucheAppuyee(UINT touche) const;
	void SaisirEtatSouris();

	const DIMOUSESTATE& EtatSouris() {
		return mouseState; }
	POINT CDIManipulateur::GetMousePosition()
	{
		POINT p{};
		GetCursorPos(&p);
		ScreenToClient(myHwnd, &p);
		return p;
	}
	////may help get back control when focus on the window is lost then regained
	//void Acquire() {
	//	pClavier->Acquire();
	//	pClavier->Acquire();
	//}

private:
	IDirectInput8 * pDirectInput;
	IDirectInputDevice8* pClavier;
	IDirectInputDevice8* pSouris;
	IDirectInputDevice8* pJoystick;
	HWND myHwnd;

	static bool bDejaInit;

	char tamponClavier[256];
	DIMOUSESTATE mouseState;
	DIMOUSESTATE previousMouseState;
};

} // namespace PM3D
