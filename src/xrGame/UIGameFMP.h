#pragma once

#include "UIGameMP.h"
#include "GameCraft.h"

class game_cl_freemp;
class CUITextWnd;
class CUIAMode;

class CUIGameFMP : public UIGameMP
{
private:
	game_cl_freemp *	m_game;
	typedef UIGameMP inherited;

protected:
	CUITextWnd*			m_stats;
	CUIAMode* m_anims;
	CUIGameCraft* craft_menu;

public:
				CUIGameFMP();
	virtual		~CUIGameFMP();

	virtual	void Init(int stage);

	virtual void SetClGame(game_cl_GameState* g);

	virtual void HideShownDialogs();

	virtual void	_BCL OnFrame();
	
	CUIGameCraft* getCraftMenu() { return craft_menu; };

	virtual bool IR_UIOnKeyboardPress(int dik);
};
