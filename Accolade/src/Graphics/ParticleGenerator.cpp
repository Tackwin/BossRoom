#include "ParticleGenerator.hpp"
#include <functional>
#include <algorithm>

#include "Managers/AssetsManager.hpp"
#include "./../Common.hpp"

#include "./../Managers/TimerManager.hpp"

#include "./../Graphics/Particle.hpp"

ParticleGenerator::ParticleGenerator() {}

ParticleGenerator::ParticleGenerator(const dyn_struct& d_struct) noexcept {
	this->d_struct = d_struct;
	setPos((Vector2f)get("pos", d_struct));
}

ParticleGenerator::ParticleGenerator(nlohmann::json json_, Vector2f pos_) {
	loadJson(json_);
	setPos(pos_);
}

void ParticleGenerator::update(double dt) {
	if (!_paused && _remain != 0) {
		_timer -= dt;
		if (_timer < 0.0) {
			_timer = getJsonValue<double>(_json, "iTime");
	
			auto particule = std::make_shared<Particle>(
				std::normal_distribution{
					(double)_particule["size"]["mean"], (double)_particule["size"]["range"]
				},
				std::normal_distribution{ 
					(double)_particule["speed"]["mean"],
					(double)_particule["speed"]["range"]
				},
				std::normal_distribution{ 
					(double)_particule["lifetime"]["mean"],
					(double)_particule["lifetime"]["range"]
				},
				std::normal_distribution{ 
					(double)_particule["a"]["mean"], (double)_particule["a"]["range"]
				},
				AM::getTexture(_particule["sprite"].get<std::string>()),
				_pos,
				Vector2f::createUnitVector(unitaryRng(RD) * 2 * PIf)
			);
			_particles.push_back(std::move(particule));

			if (_remain != 0) {
				_remain--;
			}
		}
	}

	for (auto& p : _particles) {
		p->update(dt);
	}

	_particles.erase(
		std::remove_if(
			_particles.begin(), 
			_particles.end(), 
			[](const auto& A) { return A->toRemove(); }
		),
		_particles.end()
	);
}

void ParticleGenerator::render(sf::RenderTarget& target) {
	for (auto& p : _particles) {
		p->render(target);
	}
}

void ParticleGenerator::pause() {
	_paused = true;
};
void ParticleGenerator::resume() {
	_paused = false;
};
void ParticleGenerator::restart() {
	_particles.clear();
	start();
};

void ParticleGenerator::start() {
	_remain = _nParticles;
}

void ParticleGenerator::loadJson(const nlohmann::json& json) {
	_json = json;

	_remain = getJsonValue<u32>(_json, "nParticles");
	_iTime = getJsonValue<double>(_json, "iTime");
	_particule = _json.at("particle");
}

void ParticleGenerator::setPos(const Vector2f& pos) {
	_pos = pos;
}
Vector2f ParticleGenerator::getPos() const {
	return _pos;
}

bool ParticleGenerator::toRemove() const noexcept {
	return _remain == 0;
}