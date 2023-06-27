#pragma once
#include "ui\UIDialogWnd.h"
#include "ui\UIStatic.h"
#include "ui\UI3tButton.h"
#include "ui\UIInventoryUtilities.h"
#include "ui\xrUIXmlParser.h"
#include "ui\UIXmlInit.h"
#include "ui\UIHelper.h"

#include "ui\UIWndCallback.h"
   
#define MAX_CATEGORIES 6

class CUIScrollView;
class CUITabControl;
class CUIXml;

void SectionToIcon(CUIStatic* icon, LPCSTR section);



struct CraftSystemItem
{
	xr_vector<shared_str> ingridients;
	shared_str section;
};

#define MAX_INGRIDIENT 3
class ItemCraftPannel : public CUIWindow
{
	char* array_items_current[MAX_INGRIDIENT] = { "static_craft_item_1", "static_craft_item_2","static_craft_item_3" };


public:
	LPCSTR item_to_craft;
	xr_vector<shared_str> reqvest;

	ItemCraftPannel(CUIXml& xml_doc, CraftSystemItem* item)
	{
		
		CUIXmlInit::InitWindow(xml_doc, "static_window_craft", 0, this );
		
		//CUIStatic* wnd = UIHelper::CreateStatic(xml_doc, "static_window_craft", this);
 
		//CUITextWnd* wnd_text = UIHelper::CreateTextWnd(xml_doc, "static_text_craft_result", this);  //wnd
		//wnd_text->SetText(item->section.c_str());

		for (int i = 0; i < item->ingridients.size(); i++)
		{
			CUIStatic* item_icon = UIHelper::CreateStatic(xml_doc, array_items_current[i], this);   //wnd
			SectionToIcon(item_icon, item->ingridients[i].c_str());
			reqvest.push_back(item->ingridients[i]);
		}

		CUIStatic* item_icon = UIHelper::CreateStatic(xml_doc, "static_craft_item_4", this);		///wnd
		SectionToIcon(item_icon, item->section.c_str());
		
		item_to_craft = item->section.c_str();
		
	}


};
 
class CUIGameCraft	 : public  CUIDialogWnd, public CUIWndCallback
{
	typedef CUIDialogWnd m_window;
 
	CUIFrameWindow* main_panel;
	CUIFrameWindow* main_panel_craft;


	CUI3tButton* button_Craft;
 	CUI3tButton* button_exit;  

	CUITabControl* tabcontrol_menu;
 	CUIScrollView* listbox_crafts;
	
	CUITextWnd* label_craft_info;
 
	xr_map<shared_str, xr_vector<CraftSystemItem>> loaded_items;
	void LoadCrafts();

public: 
	void InitWindow();
	void Show(bool status);
  
	void SetActiveSubdialog(LPCSTR tab);
	virtual void 		SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	~CUIGameCraft();
	CUIGameCraft();


	virtual bool OnKeyboardAction(int dik, EUIMessages keyboard_action);
	virtual bool OnMouseAction(float x, float y, EUIMessages mouse_action);

	virtual void CraftSelect(ItemCraftPannel* panel);

	void xr_stdcall     button_click_craft(CUIWindow* w, void* d);
	void xr_stdcall     button_click_exit(CUIWindow* w, void* d);

	void ReciveResult(NET_Packet& packet);

	LPCSTR last_selected;
	xr_vector<shared_str> reqvest;

};

