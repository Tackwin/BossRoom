#include "SpellDirection.hpp"

#include "Components/Hitable.hpp"

#include "Managers/AssetsManager.hpp"

#include "Ruins/Section.hpp"

#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);
#define SAVE(x, y) json[#x] = y(info.x);

nlohmann::json SpellDirectionInfo::saveJson(SpellDirectionInfo info) noexcept {
	nlohmann::json json = nlohmann::json::object();

	SAVE(speed, );
	SAVE(damage, );
	SAVE(radius, );
	SAVE(lifeTime, );
	SAVE(capacity, );
	SAVE(particleGenerator, (std::string));

	return json;
}

SpellDirectionInfo SpellDirectionInfo::loadJson(nlohmann::json json) noexcept {
	SpellDirectionInfo info;

	LOAD(speed, (float));
	LOAD(damage, (float));
	LOAD(radius, (float));
	LOAD(lifeTime, (float));
	LOAD(capacity, (float));
	LOAD(particleGenerator, ([](auto x) { return x.get<std::string>(); }));

	return info;
}

#undef X

SpellDirection::SpellDirection(
	Section* section, std::weak_ptr<Object> sender, SpellDirectionInfo info
) noexcept : Spell(section), sender_(sender), info_(info) {
	senderUuid_ = sender.lock()->uuid;

	id_ = uuid;

	idMask.set(Object::MAGIC);
	idMask.set(Object::SPELL);

	capacity_ = info_.capacity;
	particleGenerator_ = ParticleGenerator(AM::getJson(info_.particleGenerator), pos);
	angleToSender_ = (float)(C::unitaryRng(C::RD) * 2 * C::PId);
}

void SpellDirection::setPos(Vector2f pos) noexcept {
	this->pos = pos;
	collider->setPos(pos);
	particleGenerator_.setPos(pos);
}

void SpellDirection::update(double dt) noexcept {
	lifeTimer_ -= (float)dt;

	if (launched_) {
		setPos(pos + dir_ * info_.speed * dt);

		if (lifeTimer_ < 0.f) remove();
		particleGenerator_.update(dt);
	}
}
void SpellDirection::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape mark{ 0.05f };
	mark.setFillColor(Vector4f{ 0.f, 0.f, 1.f, 1.f });
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(pos);

	target.draw(mark);

	particleGenerator_.render(target);
}

void SpellDirection::launch(Vector2f dir, std::bitset<Object::SIZE> target) noexcept {
	auto disk = std::make_unique<Disk>();
	disk->onEnter = std::bind(&SpellDirection::onEnter, this, std::placeholders::_1);
	disk->onExit = std::bind(&SpellDirection::onExit, this, std::placeholders::_1);
	disk->sensor = true;
	disk->r = info_.radius;

	collider = std::move(disk);

	dir_ = dir.normalize();

	collisionMask |= target;
	maskChanged = true;
	launched_ = true;
	lifeTimer_ = info_.lifeTime;
}

void SpellDirection::remove() noexcept {
	if (launched_) Spell::remove();
}

void SpellDirection::onEnter(Object* object) noexcept {
	if (object->uuid == senderUuid_) return;

	dealCapacity(info_.damage, dynamic_cast<Hitable*>(object));
}

void SpellDirection::onExit(Object*) noexcept {}

SpellDirectionInfo SpellDirection::getInfo() const noexcept {
	return info_;
}

void SpellDirection::dealCapacity(float damage, Hitable* hitable) noexcept {
	float dmg = std::fminf(damage, capacity_);

	assert(hitable != nullptr);
	hitable->hit(dmg);

	capacity_ -= dmg;
	if (capacity_ <= 0.f) remove();
}