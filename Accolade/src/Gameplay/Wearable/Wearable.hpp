#pragma once
#include <array>
#include <vector>
#include <bitset>
#include <memory>
#include <functional>
#include <unordered_set>
#include <any>

#include "./../../3rd/json.hpp"

#include "./../../Common.hpp"
#include "./../../Utils/UUID.hpp"

#include "WearableInfo.hpp"

#include "../../Events/Event.hpp"

class Player;
class Wearable {
public:
	static constexpr const char* EVERY_WEARABLE[] = {
		"Fire ball", "Sword"
	};
	static constexpr auto FIRE_BALL = EVERY_WEARABLE[0];
	static constexpr auto SWORD = EVERY_WEARABLE[1];


	Wearable() noexcept;
	Wearable(WearableInfo info) noexcept;

	Wearable(const Wearable&) = delete;
	Wearable& operator=(const Wearable&) = delete;

	Wearable(Wearable&&) = default;
	Wearable& operator=(Wearable&&) = default;

	void mount(std::weak_ptr<Player> player) noexcept;
	void unmount() noexcept;

	void update(double dt) noexcept;

	WearableInfo getInfo() const noexcept;

	const UUID& getUUID() const noexcept;

	static void InitWearable();
	static WearableInfo GetWearableinfo(std::string weapon) noexcept;
private: 

	static std::unordered_map<std::string, WearableInfo> _wearables;

	template<typename T>
	void setProp(std::string name, const T& v) noexcept {
		_properties[name] = v;
	}
	bool hasProp(const std::string& name) const noexcept {
		return _properties.count(name) != 0;
	}
	void delProp(const std::string& name) noexcept {
		_properties.erase(name);
	}
	template<typename T>
	T& getPropsRef(const std::string& name) noexcept {
		return std::any_cast<T&>(_properties.at(name));
	}

	std::weak_ptr<Player> _player{};
	WearableInfo _info{};

	UUID _uuid{ UUID::null };
	std::unordered_set<UUID> _keys{};
	std::unordered_map<std::string, std::any> _properties;
};

