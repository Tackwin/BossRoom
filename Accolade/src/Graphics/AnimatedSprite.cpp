#include "AnimatedSprite.hpp"

#include "./../Common.hpp"

#include "./../Managers/TimerManager.hpp"
#include "./../Managers/AssetsManager.hpp"

AnimatedSprite::AnimatedSprite() noexcept {

}
AnimatedSprite::AnimatedSprite(AnimatedSprite&& other) noexcept {
	while (!_stackAnim.empty()) {
		TM::removeFunction(_stackAnim.top().keyCallback);
		_stackAnim.pop();
	}
	std::swap(_stackAnim, other._stackAnim);
	std::swap(_sprite, other._sprite);
	std::swap(_json, other._json);
}
AnimatedSprite::AnimatedSprite(const std::string& json_) noexcept :
	_json(AssetsManager::getJson(JSON_KEY)["sprites"][json_]),
	_sprite(AssetsManager::getTexture(json_)) {
}

AnimatedSprite::~AnimatedSprite() {
	while (!_stackAnim.empty()) {
		TM::removeFunction(_stackAnim.top().keyCallback);
		_stackAnim.pop();
	}
}

AnimatedSprite& AnimatedSprite::operator=(AnimatedSprite&& other) noexcept {
	while (!_stackAnim.empty()) {
		TM::removeFunction(_stackAnim.top().keyCallback);
		_stackAnim.pop();
	}
	std::swap(_stackAnim, other._stackAnim);
	std::swap(_sprite, other._sprite);
	std::swap(_json, other._json);
	return *this;
}

void AnimatedSprite::pushAnim(const std::string& key, u32 offset) noexcept {
	if (!_stackAnim.empty()) {
		const auto& anim = _stackAnim.top();
		TimerManager::pauseFunction(anim.keyCallback);
	}

	assert(!_json[key].is_null());
	auto& json = _json[key];

	const bool loop = json["loop"].is_null() ? false : json["loop"].get<bool>();
	const u32 frames = json["frames"];
	const float time = json["time"];
	
	Animation anim;
	anim.w = json["rect"].is_null() ? _json["rect"][0] : json["rect"][0];
	anim.h = json["rect"].is_null() ? _json["rect"][1] : json["rect"][1];
	anim.row = json["row"].is_null() ? 0 : json["row"].get<u32>();
	anim.col = json["col"].is_null() ? 0 : json["col"].get<u32>();

	anim.keyCallback = TimerManager::addFunction(time / frames,
		[
			&, row = anim.row, col = anim.col, frames,
			w = anim.w, h = anim.h, loop, n = offset
		]
	(double) mutable -> bool {
		if (!loop && n >= frames) {
			popAnim();
			return true;
		}
		n %= frames;
		_stackAnim.top().i = n;

		sf::IntRect intRect;
		intRect.left = (col + n) * w;
		intRect.top = row * h;
		intRect.width = w;
		intRect.height = h;

		_sprite.setTextureRect(intRect);
		n++;

		return false;
	}
	);

	_stackAnim.push(anim);
	
	TimerManager::callFunction(anim.keyCallback);
}
void AnimatedSprite::popAnim() {
	TimerManager::removeFunction(_stackAnim.top().keyCallback);
	_stackAnim.pop();
	if (!_stackAnim.empty()) {
		const auto& top = _stackAnim.top();
		sf::IntRect intRect;
		intRect.left = (top.col + top.i) * top.w;
		intRect.top = top.row * top.h;
		intRect.width = top.w;
		intRect.height = top.h;

		TimerManager::resumeFunction(top.keyCallback);
		_sprite.setTextureRect(intRect);
	}
}
void AnimatedSprite::render(sf::RenderTarget& target) {
	target.draw(_sprite);
}
const Vector2f AnimatedSprite::getSize() {
	return { _json["rect"][0], _json["rect"][1] };
}
sf::Sprite& AnimatedSprite::getSprite() {
	return _sprite;
}