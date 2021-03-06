#pragma once
#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <unordered_set>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Common.hpp"

#include "3rd/json.hpp"

#include "Math/Vector.hpp"

#include "Physics/Object.hpp"

#include "Graphics/AnimatedSprite.hpp"

#include "Managers/MemoryManager.hpp"

#include "Gameplay/Magic/Spells/SpellDirection.hpp"
#include "Gameplay/Magic/Spells/SpellTarget.hpp"
#include "Gameplay/Wearable/Wearable.hpp"

#include "Utils/UUID.hpp"

#include "Components/Removable.hpp"
#include "Components/Hitable.hpp"

#include "Entity/EntityStore.hpp"

#include "Memory/ValuePtr.hpp"

struct PlayerInfo {
	static constexpr auto BOLOSS = "char_boloss";

	float maxLife{ 0.f };
	float speed{ 0.f };
	float dashRange{ 0.f };
	float specialSpeed{ 0.f };
	float invincibilityTime{ 0.f };
	float jumpBoostMultiplier{ NAN };
	float jumpHeight{ 0.f };

	Vector2f hitBox{0.f, 0.f};

	std::string name{ "" };
	std::string sprite{ "" };

	std::optional<std::string> weapon{};
	std::vector<ValuePtr<ItemInfo>> items;

	PlayerInfo();
	PlayerInfo(nlohmann::json json);
};

class Item;
class Zone;
class Game;
class Level;
class Section;
class Projectile;
class Plateforme;
class Player : 
	public Object,
	public Removable,
	public Hitable,
	public std::enable_shared_from_this<Player>
{
public:
	Player() noexcept;
	Player(const PlayerInfo& info) noexcept;

	void enterLevel(Level* level) noexcept;
	void exitLevel() noexcept;

	void enter(Section* section)	noexcept;
	void leave()					noexcept;

	void input() noexcept;
	void update(double dt);
	void render(sf::RenderTarget& target);

	void heal(float hp) noexcept;
	virtual void hit(float damage) noexcept override;

	bool isAlive() const;
	float getLife() const;

	std::optional<std::string> getWeapon() const noexcept;
	void swapWeapon(std::string weapon);
	bool isEquiped() const noexcept;

	bool canMount(const OwnId<Item>& item) noexcept;
	void mountItem(OwnId<Item>&& item) noexcept;
	const std::vector<OwnId<Item>>& getItems() noexcept;

	template<class... Args>
	void addProjectile(Args... args) {
		(addProjectile(args), ...);
	}
	void addProjectile(const std::shared_ptr<Projectile>& projectile);

	Vector2f getPos();
	void setPos(const Vector2f& v);

	const std::vector<std::shared_ptr<Projectile>>&
		getProtectilesToShoot() const;

	void clearProtectilesToShoot();

	template<class... Args>
	void addZone(Args... args) {
		(addZone(args), ...);
	}
	void addZone(const std::shared_ptr<Zone>& zone);
	const std::vector<std::shared_ptr<Zone>>& getZonesToApply() const;
	void clearZonesToApply();

	bool isInvicible() const;

	void floored();
	void ceiled() noexcept;
	bool isFloored() const;

	void clearJump() noexcept;
	void clearKnockBack() noexcept;

	void setFocus(bool focus = true);
	bool getFocus() const;

	void unmount();

	void applyVelocity(Vector2f v);

	PlayerInfo getPlayerInfo() const;
	PlayerInfo& getPlayerInfo();

	void knockBack(Vector2f recoil, float time) noexcept;

	Vector2f getFacingDir() const noexcept;
	void setFacingDir(float angle) noexcept;

	void keyPressed(sf::Keyboard::Key key);
	void keyReleased(sf::Keyboard::Key key);

	Vector2f support(float a, float d) const noexcept;

	virtual void remove() noexcept override;
	virtual bool toRemove() const noexcept override;

	Rectangle2f getBoundingBox() const noexcept;

private: //TODO: Make private

	void jumpKeyPressed();
	bool filterCollision(Object& obj) noexcept;

	void onEnter(Object* obj) noexcept;
	void onExit(Object* obj) noexcept;

	u32 _nJumpsLeft{ 2 };

	std::optional<Vector2f> _knockBack{};
	float _knockBackTime{ 0.f };

	std::optional<Vector2f> jumpVelocity_{};

	float _life;
	Vector2f _dir;
	sf::Color _color;

	PlayerInfo _info;

	Wearable _weapon;

	Box* _hitBox;

	nlohmann::basic_json<> _json;

	bool _floored{ false };
	bool _freeze{ false };
	bool remove_{ false };
	bool continueToJump_{ false };
	bool _boostingJump{ false };
	bool downed_jump{ false };

	bool _focus{ true };

	float _facingDir{ 0.f };

	bool _invincible{ false };
	double _invincibleTime{ 0.f };

	bool wanting_to_pass_semiPlatforme{ false };
	bool pass_though_semiPlateforme{ false };

	//Todo clean up level.
	Level* _level{ nullptr };
	Section* section_{ nullptr };

	AnimatedSprite _sprite;
	sf::Sound _hitSound;

	std::unordered_set<UUID> plateforme_colliding;

	std::vector<OwnId<Item>> own_items;

	std::vector<std::shared_ptr<Projectile>> _projectilesToShoot;
	std::vector<std::shared_ptr<Zone>> _zonesToApply;

	Vector2f origin;
	// Now there is stuff for the magic spells, i need to figure out
	// a way to make that generics, i'll wait for a number of spell to be implemented
	// before i do that. Right now i don't see pattern to generalize, maybe there isn't,
	// we'll see
public:

	void giveSpell(SpellTargetInfo info) noexcept;
	void giveSpell(SpellDirectionInfo info) noexcept;

	bool isBoomerangSpellAvailable() const noexcept;
	bool isDirectionSpellAvailable() const noexcept;
private:

	std::shared_ptr<SpellTarget> spellBoomerang_;
	std::shared_ptr<SpellDirection> spellDirection_;

	void updateBoomerangSpell(double dt) noexcept;
	void updateDirectionSpell(double dt) noexcept;
};