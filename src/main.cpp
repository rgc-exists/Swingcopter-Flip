
#include <math.h>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/PlayerObject.hpp>


float switchRotMultiplier = 5;
float switchRotTime = 0.25f;

class $modify(PlayerObject) {
	struct Fields {
		float m_switchTimer = 0;
		float m_switchRotSpeed = 0;
		float m_flipDirection = 1;
	};

	void flipGravity(bool flipped, bool p1) {
		switchRotMultiplier = Mod::get()->getSettingValue<double>("rotation-intensity");
		switchRotTime = Mod::get()->getSettingValue<double>("rotation-duration");

		bool modEnabled = Mod::get()->getSettingValue<bool>("mod-enabled");

		if (modEnabled) {
			if (m_isSwing) {

				m_fields->m_switchTimer = 0;
				m_fields->m_flipDirection = (flipped ? -1 : 1);
				m_fields->m_switchRotSpeed = switchRotMultiplier;

				flipSprites(flipped);

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

	virtual void update(float p0) {
		PlayerObject::update(p0);

		bool modEnabled = Mod::get()->getSettingValue<bool>("mod-enabled");

		if (modEnabled) {
			m_fields->m_switchTimer += 1;
			float m_switchTimer = m_fields->m_switchTimer;

			float secondsPerFrame = CCDirector::get()->getSecondsPerFrame();
			float rotTimeFrameAdjusted = switchRotTime * secondsPerFrame;

			if (m_isSwing) {
				CCNode* iconParent = m_iconSprite->getParent();

				float rotation = iconParent->getRotation();
				if (m_switchTimer < rotTimeFrameAdjusted) {

					float speedToChange = (switchRotMultiplier / rotTimeFrameAdjusted * 2);
					m_fields->m_switchRotSpeed -= speedToChange;

					rotation += m_fields->m_switchRotSpeed * m_fields->m_flipDirection;

					/*
					if (m_switchTimer == std::ceilf(rotTimeFrameAdjusted / 4)) {
						flipSprites(m_fields->m_flipDirection < 0);
					}
					*/
				}
				else {
					rotation = 0;
				}
				iconParent->setRotation(rotation);
			}
		}

	}
};