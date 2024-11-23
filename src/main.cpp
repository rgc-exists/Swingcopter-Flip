
#include <math.h>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/PlayerObject.hpp>


float switchRotMultiplier = 8;
float switchRotTime = 0.05f;

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
	}

	virtual void update(float dt) {
		PlayerObject::update(dt);

		bool modEnabled = Mod::get()->getSettingValue<bool>("mod-enabled");
		bool rotationEnabled = Mod::get()->getSettingValue<bool>("subtle-rotation");
		
		float actualDeltaTime = CCDirector::get()->getActualDeltaTime();

		if (modEnabled) {
			m_fields->m_switchTimer += actualDeltaTime;
			float m_switchTimer = m_fields->m_switchTimer;
			
			if (m_isSwing) {

				bool flipped = m_isUpsideDown;
				bool invertFlip = Mod::get()->getSettingValue<bool>("invert-flip");
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