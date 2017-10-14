#include "Graphics/AnimatedSprite.hpp"

#include "Const.hpp"

#include "Managers/TimerManager.hpp"
#include "Managers/AssetsManager.hpp"

AnimatedSprite::AnimatedSprite() {

}
AnimatedSprite::AnimatedSprite(const AnimatedSprite& sprite_) :
	_sprite(sprite_._sprite),
	_json(sprite_._json)
{

}
AnimatedSprite::AnimatedSprite(const std::string& json_) :
	_json(AssetsManager::getJson(JSON_KEY)["sprites"][json_]),
	_sprite(AssetsManager::getTexture(json_)) {
}


void AnimatedSprite::pushAnim(const std::string& key_, uint32_t offset_) {
	if (!_stackAnim.empty()) {
		const auto& anim = _stackAnim.top();
		TimerManager::pauseFunction(anim.keyCallback);
	}

	assert(!_json[key_].is_null());
	auto& json = _json[key_];

	const bool loop = json["loop"].is_null() ? false : json["loop"].get<bool>();
	const uint32_t frames = json["frames"];
	const float time = json["time"];
	
	Animation anim;
	anim.w = json["rect"].is_null() ? _json["rect"][0] : json["rect"][0];
	anim.h = json["rect"].is_null() ? _json["rect"][1] : json["rect"][1];
	anim.row = json["row"].is_null() ? 0 : json["row"].get<uint32_t>();
	anim.col = json["col"].is_null() ? 0 : json["col"].get<uint32_t>();
	anim.keyCallback = TimerManager::addFunction(time / frames, key_, 
		[&, row = anim.row, col = anim.col, frames, w = anim.w, h = anim.h, loop, n = offset_](double)mutable->bool{
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
AnimatedSprite& AnimatedSprite::operator=(const AnimatedSprite& other) {
	_sprite = other._sprite;
	_json = other._json;
	//we don't copy the anim Key
	return *this;
}
