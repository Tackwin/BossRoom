#include "Graphics/ParticleGenerator.hpp"
#include <functional>
#include <algorithm>

#include "Global/Const.hpp"
#include "Managers/TimerManager.hpp"
#include "Graphics/Particle.hpp"

ParticleGenerator::ParticleGenerator() {}

ParticleGenerator::ParticleGenerator(nlohmann::json json_, Vector2 pos_) :
	_json(json_),
	_pos(pos_) {

	_lambda = [&](auto)mutable -> bool {
		float timer = getJsonValue<float>(_json, "iTime");

		_particles.push_back(std::make_shared<Particle>(_json["particle"], _pos, Vector2::createUnitVector(unitaryRng(RNG) * 2 * PIf)));

		_lambdaKey = TimerManager::addFunction(timer, "", _lambda);
		return true;
	};
	_lambda(0);
}

void ParticleGenerator::update(double dt) {
	for (auto& p : _particles) {
		p->update(dt);
	}

	_particles.erase(
		std::remove_if(_particles.begin(), _particles.end(), [](const auto& A) {return A->toRemove(); }),
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
	resume();
};