#include "Graphics/AnimatedSprite.hpp"

#include "Managers/TimerManager.hpp"

AnimatedSprite::AnimatedSprite() {

}
AnimatedSprite::AnimatedSprite(const AnimatedSprite& sprite) :
	_sprite(sprite._sprite),
	_frameRectangle(sprite._frameRectangle),
	_keyAnim(sprite._keyAnim),
	_nFrames(sprite._nFrames)
{

}

AnimatedSprite::AnimatedSprite(std::vector<uint32_t> nFrames_, Vector2 frameRectangle_) :
	_frameRectangle(Vector2::ZERO, frameRectangle_),
	_nFrames(nFrames_)
{
	_frameRectangle = _frameRectangle == Rectangle::ZERO ? _sprite.getTextureRect() : _frameRectangle;
	_sprite.setTextureRect(_frameRectangle);
}
AnimatedSprite::AnimatedSprite(const sf::Texture& texture_, std::vector<uint32_t> nFrames_, Vector2 frameRectangle_) :
	_frameRectangle(Vector2::ZERO, frameRectangle_),
	_nFrames(nFrames_),
	_sprite(texture_)
{
	_frameRectangle = _frameRectangle == Rectangle::ZERO ? _sprite.getTextureRect() : _frameRectangle;
	_sprite.setTextureRect(_frameRectangle);
}

void AnimatedSprite::startAnim(uint32_t i_, float speed_) {
	if (!_keyAnim.empty() && TimerManager::functionsExist(_keyAnim)) {
		TimerManager::removeFunction(_keyAnim);
	}
	_keyAnim = TimerManager::addFunction(speed_ / _nFrames[i_], "anim", [&, i_, n = 0u](float)mutable->bool{
		_sprite.setTextureRect(static_cast<sf::IntRect>(Rectangle{
			(n % _nFrames[i_]) * _frameRectangle.w, 
			i_ * _frameRectangle.h, 
			_frameRectangle.w, 
			_frameRectangle.h
		}));
		return n++ >= _nFrames[i_];
	});
	TimerManager::callFunction(_keyAnim);
}
void AnimatedSprite::render(sf::RenderTarget& target) {
	target.draw(_sprite);
}
const Vector2& AnimatedSprite::getSize() {
	return _frameRectangle.size;
}
sf::Sprite& AnimatedSprite::getSprite() {
	return _sprite;
}
AnimatedSprite& AnimatedSprite::operator=(const AnimatedSprite& other) {
	_sprite = other._sprite;
	_frameRectangle = other._frameRectangle;
	_keyAnim = other._keyAnim;
	_nFrames = other._nFrames;
	return *this;
}
