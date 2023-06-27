#pragma once
#include "inventory_item_object.h"


class CActorBackpack : public CInventoryItemObject
{
private:
	typedef CInventoryItemObject inherited;
public:
	CActorBackpack();
	virtual ~CActorBackpack();

	virtual void Load(LPCSTR section);
	virtual void Hit(float P, ALife::EHitType hit_type);
 
	float m_additional_weight;
	float m_additional_weight2;

	float original_weight;
	float original_weight2;

	float m_fPowerRestoreSpeed;
	float m_fPowerLoss;

protected:
	virtual bool install_upgrade_impl(LPCSTR section, bool test);
};

