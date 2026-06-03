
#include <math.h>

#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>

#include <Geode/modify/PlayerObject.hpp>

#include "databases.hpp"

using namespace geode::prelude;

float switchRotMultiplier = 8;
float switchRotTime = 0.05f;

bool modEnabled = true;
bool rotationEnabled = true;
std::string invertFlipSetting = "AUTO";

$on_mod(Loaded) {
	modEnabled = Mod::get()->getSettingValue<bool>("mod-enabled");
	rotationEnabled = Mod::get()->getSettingValue<bool>("subtle-rotation");
	invertFlipSetting = Mod::get()->getSettingValue<std::string>("invert-flip-v2");


	listenForSettingChanges<bool>("mod-enabled", [](bool value) {
		modEnabled = value;
	});
	listenForSettingChanges<bool>("subtle-rotation", [](bool value) {
		rotationEnabled = value;
	});
	listenForSettingChanges<std::string>("invert-flip-v2", [](std::string value) {
		invertFlipSetting = value;
	});
}

class $modify(PlayerObject) {
	struct Fields {
		float m_switchTimer = 0;
		float m_switchRotSpeed = 0;
		float m_flipDirection = 1;
	};

	void flipGravity(bool flipped, bool p1) {

		bool modEnabled = Mod::get()->getSettingValue<bool>("mod-enabled");

		if (modEnabled) {
			if (m_isSwing) {
				m_fields->m_switchTimer = 0;
				m_fields->m_flipDirection = (flipped ? -1 : 1);
				m_fields->m_switchRotSpeed = switchRotMultiplier;
			}
		}
		return PlayerObject::flipGravity(flipped, p1);
	}

	void flipSprites(bool flipped) {

		m_iconSprite->setFlipY(flipped);
		m_iconSpriteSecondary->setFlipY(flipped);
		m_iconSpriteWhitener->setFlipY(flipped);
		m_iconGlow->setFlipY(flipped);


		// Compatability for Alphalaneous's "Fine Outline" mod.
		CCNode* iconParent2nd = m_iconSpriteSecondary->getParent();

		CCNode* fineOutline = iconParent2nd->getChildByID("alphalaneous.fine_outline/black_outline");
		if (fineOutline) 
		{
			CCSprite* outlSprite = (CCSprite*)fineOutline;
			outlSprite->setFlipY(flipped);
		}

	}

	virtual void update(float dt) {
		PlayerObject::update(dt);
		
		float actualDeltaTime = CCDirector::get()->getActualDeltaTime();

		bool invertFlip = (invertFlipSetting == "ON");
		if (invertFlipSetting == "AUTO") {
			if (iconFlipDefaults.contains(GameManager::get()->getPlayerSwing())) {
				invertFlip = iconFlipDefaults[GameManager::get()->getPlayerSwing()];
			}
		}
		

		if (modEnabled) {
			m_fields->m_switchTimer += actualDeltaTime;
			float m_switchTimer = m_fields->m_switchTimer;
			
			if (m_isSwing) {

				bool flipped = m_isUpsideDown;
				if (invertFlip) flipped = !flipped;
				flipSprites(flipped);


				if (rotationEnabled) {
					CCNode* iconParent = m_iconSprite->getParent();

					float rotation = iconParent->getRotation();
					if (m_switchTimer < switchRotTime) {
						rotation = switchRotMultiplier * std::sin(m_switchTimer / switchRotTime * M_PI) * m_fields->m_flipDirection;
					}
					else {
						m_fields->m_switchRotSpeed = 0;
						rotation = 0;
					}
					iconParent->setRotation(rotation);
				}
			}
			else {
				
				flipSprites(false);

				m_fields->m_switchRotSpeed = 0;
			}
		}
	}
};