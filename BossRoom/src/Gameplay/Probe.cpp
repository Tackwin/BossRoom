#include "Gameplay/Probe.hpp"

#include "Managers/TimerManager.hpp"
#include "Global/Const.hpp"

using namespace nlohmann;

Probe::Probe(basic_json<> json, Vector2 pos, Vector2 dest, 
			 std::function<void(Probe&)> init, 
			 std::function<void(Probe&, float)> update, 
			 std::function<void(Probe&)> unInit) :
	_json(json), _pos(pos), _dest(dest), _init(init), _update(update), _unInit(unInit) {

	_speed = getJsonValue<float>(json, "SPEED");
	_radius = getJsonValue<float>(json, "RADIUS");
	_color = sf::Color(
		json["COLOR"]["R"].get<sf::Uint8>(),
		json["COLOR"]["G"].get<sf::Uint8>(),
		json["COLOR"]["B"].get<sf::Uint8>()
	);

	_shape = sf::CircleShape(_radius);
	_shape.setOrigin(_radius, _radius);
	_shape.setFillColor(_color);
}

Probe::~Probe() {
	_unInit(*this);

	for(auto &k : _keys) {
		TimerManager::removeFunction(k);
	}
}

void Probe::update(float dt) {
	if(_setuped) {
		_update(*this, dt);
		return;
	}

	auto d = _dest - _pos;
	d.normalize();

	_pos += d * _speed * dt;
	if(Vector2::equal(_pos, _dest)) {
		_pos = _dest;
		_setuped = true;
		_init(*this);
	}

	_shape.setPosition(_pos);
}

void Probe::render(sf::RenderTarget& target) {
	target.draw(_shape);
}
