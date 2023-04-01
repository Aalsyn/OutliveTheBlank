#ifndef inputController_h
#define inputController_h


namespace PM3D {
	class InputController {
	private:
		bool holdingTab = false;
		bool holdingLight = false;
		bool holdingEsc = false;
		bool holdingDown = false;
		bool holdingUp = false;
		bool holdingSpace = false;
		bool isShadow = false;
		bool isShooting = false;
		bool holdingT = false;

		enum class MenuSelector {
			Reprendre,
			Reset,
			Options,
			Quitter
		};
		MenuSelector selection = MenuSelector::Reprendre;
	public :
		InputController() = default;
		void HandleKeyboardInput();
		void HandleMouseInput();
		void changeMenuSelection(bool down);
		void MenuEnterKeyPressed();
		void OptionButtonClicked(int nbr);
	};
}
#endif
