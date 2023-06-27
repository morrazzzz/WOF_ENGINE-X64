#include "stdafx.h"
#include "GameCraft.h"
 
#include "ui\UIScrollView.h"
#include "ui\UITabControl.h"
#include "UICursor.h"
#include "Level.h"

void SectionToIcon(CUIStatic* icon, LPCSTR section)
{
	if (pSettings->section_exist(section))
	{
		icon->SetShader(InventoryUtilities::GetEquipmentIconsShader());

		Frect				tex_rect;
		tex_rect.x1 = float(pSettings->r_u32(section, "inv_grid_x") * INV_GRID_WIDTH);
		tex_rect.y1 = float(pSettings->r_u32(section, "inv_grid_y") * INV_GRID_HEIGHT);
		tex_rect.x2 = float(pSettings->r_u32(section, "inv_grid_width") * INV_GRID_WIDTH);
		tex_rect.y2 = float(pSettings->r_u32(section, "inv_grid_height") * INV_GRID_HEIGHT);
		tex_rect.rb.add(tex_rect.lt);


		icon->GetUIStaticItem().SetTextureRect(tex_rect);
	}
	else
	{
		Msg("Check Valid Item Name: %s", section);
	}

}

void CUIGameCraft::LoadCrafts()
{
	string_path p;
	FS.update_path(p, "$game_config$", "actor_craftmenu.ltx");

	CInifile* file = xr_new<CInifile>(p, true, true, true);
	
	if (!file)
	{
 		return;
	}

	loaded_items.clear();

 	int section_id = 0;
	for (auto data_ini : file->sections())
	{
		for (auto items : data_ini->Data)
		{
 			int cnt = _GetItemCount(items.second.c_str());
			CraftSystemItem data;
			data.section = items.first.c_str();
 			for (int z = 0; z < cnt; z++)
			{
				string256 item;
				_GetItem(items.second.c_str(), z, item);
				data.ingridients.push_back(item);
 			}
  			loaded_items[data_ini->Name.c_str()].push_back(data);
		}
		section_id++;
	}
  
}



void CUIGameCraft::InitWindow()
{	
	CUIXml xml_doc;
 	xml_doc.Load(CONFIG_PATH, UI_PATH, "actor_craftmenu.xml");
	
	main_panel = UIHelper::CreateFrameWindow(xml_doc, "craft_menu_frame", this);	  
	main_panel_craft = UIHelper::CreateFrameWindow(xml_doc, "craft_menu_frame_craft", this);;

	// TabControls
	tabcontrol_menu = xr_new<CUITabControl>();
	tabcontrol_menu->SetAutoDelete(true);
	AttachChild(tabcontrol_menu);
	CUIXmlInit::InitTabControl(xml_doc, "craft_menutab", 0, tabcontrol_menu);
	tabcontrol_menu->SetMessageTarget(this);
	
	// Scrool Crafts
	listbox_crafts = xr_new<CUIScrollView>();
	CUIXmlInit::InitScrollView(xml_doc, "craft_menulistbox", 0, listbox_crafts);
	listbox_crafts->SetAutoDelete(true);
	main_panel->AttachChild(listbox_crafts);   
	

	// Buttons
	button_exit = UIHelper::Create3tButton(xml_doc, "craft_menu_exit_btn", main_panel);
	button_Craft = UIHelper::Create3tButton(xml_doc, "craft_menu_craft_btn", main_panel);
	
	label_craft_info = UIHelper::CreateTextWnd(xml_doc, "craft_menu_info", main_panel);
	label_craft_info->SetText("Выберите Крафт!!!");

	Register(button_exit);
	Register(button_Craft);

	AddCallback(button_exit, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIGameCraft::button_click_exit));
	AddCallback(button_Craft, BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIGameCraft::button_click_craft));

}

void CUIGameCraft::Show(bool status)
{
	m_window::Show(status);
	//InitWindow();	Активировать для бистрой настройки UI )) (Можно из игры настраивать)
}
 
CUIGameCraft::CUIGameCraft()
{
	LoadCrafts();
	InitWindow();
}

void CUIGameCraft::SetActiveSubdialog(LPCSTR tab)
{
	char* array_menu_buttons[MAX_CATEGORIES] = { "eptMedic", "eptMaterial", "eptElectronic", "eptFood", "eptAmmo", "eptOutfits" };
	char* array_menu[MAX_CATEGORIES] = { "craft_medicine", "craft_materials", "craft_electric", "craft_food", "craft_ammo", "craft_amunition" };


	listbox_crafts->Clear();
	CUIXml xml_doc;
	xml_doc.Load(CONFIG_PATH, UI_PATH, "actor_craftmenu.xml");

	LPCSTR _id = 0;
	for (int i = 0; i < MAX_CATEGORIES; i++)
	{
		if (xr_strcmp(array_menu_buttons[i], tab) == 0)
		{
			_id = array_menu[i];
			break;
		}
	}

	for (auto section : loaded_items)
	{
		if (section.first.equal(_id))
		{
			for (auto item : section.second)
			{
				ItemCraftPannel* wnd = new ItemCraftPannel(xml_doc, &item);
				listbox_crafts->AddWindow(wnd, true);
			}
		}
	}
}

void CUIGameCraft::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	switch (msg)
	{
		case TAB_CHANGED:
		{
			if (pWnd == tabcontrol_menu)
			{
				SetActiveSubdialog(tabcontrol_menu->GetActiveId().c_str());
			}
		}
		break;
		default:
		{
			CUIWndCallback::OnEvent(pWnd, msg, pData);
			m_window::SendMessage(pWnd, msg, pData);
		}
		break;
	}
 
}

CUIGameCraft::~CUIGameCraft()
{
}
  
 
bool CUIGameCraft::OnKeyboardAction(int dik, EUIMessages keyboard_action)
{
 	if (is_binded(kCraftMode, dik))
	{
		if (WINDOW_KEY_PRESSED == keyboard_action)
			HideDialog();

		return true;
	}

	return m_window::OnKeyboardAction(dik, keyboard_action);
}

bool CUIGameCraft::OnMouseAction(float x, float y, EUIMessages mouse_action)
{
	if (mouse_action == WINDOW_LBUTTON_DOWN)
	{
		auto child = listbox_crafts->Items();

		for (auto iter = child.rbegin(); iter != child.rend(); iter++)
		{
			CUIWindow* wind = (*iter);
			Frect wndRect;
			wind->GetAbsoluteRect(wndRect);
			Fvector2 pos = GetUICursor().GetCursorPosition();

			ItemCraftPannel* Craft = smart_cast<ItemCraftPannel*>(wind);

			if (Craft && wndRect.in(pos))
			{
				CraftSelect(Craft);
			}
		}
	}

	return m_window::OnMouseAction(x, y, mouse_action);
}

void CUIGameCraft::CraftSelect(ItemCraftPannel* panel)
{
	string128 Text;
	sprintf(Text, "Выбран Крафт: %s", panel->item_to_craft);
	label_craft_info->SetText(Text);

	last_selected = panel->item_to_craft;
	reqvest = panel->reqvest;
}

void xr_stdcall CUIGameCraft::button_click_craft(CUIWindow* w, void* d)
{
	NET_Packet p;
	Game().u_EventGen(p, GE_CRAFT_ITEM, -1);
	p.w_stringZ(last_selected);
	p.w_u8(reqvest.size());
	for (auto item : reqvest)
	{
		p.w_stringZ(item);
	}
	Game().u_EventSend(p);
}

void xr_stdcall CUIGameCraft::button_click_exit(CUIWindow* w, void* d)
{
	this->HideDialog();
}

void CUIGameCraft::ReciveResult(NET_Packet& packet)
{
	shared_str result;
	packet.r_stringZ(result);
	label_craft_info->SetText(result.c_str());
}
