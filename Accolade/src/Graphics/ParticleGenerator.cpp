#include "Graphics/ParticleGenerator.hpp"
#include <functional>
#include <algorithm>

#include "Const.hpp"

#include "Managers/TimerManager.hpp"

#include "Graphics/Particle.hpp"

ParticleGenerator::ParticleGenerator() {}

ParticleGenerator::ParticleGenerator(nlohmann::json json_, Vector2f pos_) {
	loadJson(json_);
	setPos(pos_);
}

ParticleGenerator::~ParticleGenerator() {
	if (TimerManager::functionsExist(_lambdaKey)) {
		TimerManager::removeFunction(_lambdaKey);
	}
}


void ParticleGenerator::update(double dt) {
	for (auto& p : _particles) {
		p->update(dt);
	}

	_particles.erase(
		std::remove_if(_particles.begin(), _particles.end(), [](const std::shared_ptr<Particle>& A) { return A->toRemove(); }),
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
	TimerManager::pauseFunction(_lambdaKey);
};
void ParticleGenerator::resume() {
	_paused = false;
	TimerManager::resumeFunction(_lambdaKey);
};
void ParticleGenerator::restart() {
	_particles.clear();
	if (TimerManager::functionsExist(_lambdaKey)) {
		TimerManager::removeFunction(_lambdaKey);
	}
	start();
};

void ParticleGenerator::start() {
	_remain = _nParticles;
	_ended = false;
	_lambda(0);
}

void ParticleGenerator::loadJson(const nlohmann::json& json) {
	_json = json;

	_remain = getJsonValue<u32>(_json, "nParticles");
	_nParticles = _remain;

	_lambda = [&](double)mutable -> bool {
		float timer = getJsonValue<float>(_json, "iTime");

		//_particles.push_back(std::make_shared<Particle>(_json["particle"], _pos, Vector2f::createUnitVector(unitaryRng(RD) * 2 * PIf)));

		if (_remain != 0) {
			_lambdaKey = TimerManager::addFunction(timer, "", _lambda);
			_remain--;
			_ended = true;
		}
		return true;
	};
}

void ParticleGenerator::setPos(const Vector2f& pos) {
	_pos = pos;
}
Vector2f ParticleGenerator::getPos() const {
	return _pos;
}

bool ParticleGenerator::isEnded() const {
	return _ended;
}
