#pragma once
#include <vector>
#include <string>

#include "json.hpp"

#include "Vector2.hpp"

class Probe {
public:

	Probe(nlohmann::basic_json<> _json, Vector2 pos, Vector2 dest, 
		std::function<void(Probe&)> initFunction,
		std::function<void(Probe&, float)> updateFunction,
		std::function<void(Probe&)> unInitFunction);
	~Probe();

public: //TODO: make this private
	std::function<void(Probe&)> _init;
	std::function<void(Probe&, float)> _update;
	std::function<void(Probe&)> _unInit;
	
	nlohmann::basic_json<> _json;

	Vector2 _dest;
	Vector2 _pos;
	
	float _speed;
	bool _setuped = false;
	bool _remove = false;

	float _radius;
	sf::Color _color;
	sf::CircleShape _shape;

	std::vector<std::string> _keys;

	void update(float dt);
	void render(sf::RenderTarget& target);
};

