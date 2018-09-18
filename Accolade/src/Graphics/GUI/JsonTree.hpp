#pragma once
#include "Widget.hpp"

#include <unordered_map>
#include <string>
#include <functional>

#include "3rd/json.hpp"

#include "Utils/UUID.hpp"

class JsonTree : public Widget {
public:
	using ChangeCallback = std::function<void(const nlohmann::json&)>;

	static constexpr auto NAME = "JsonTree";

	JsonTree(const nlohmann::json& json) noexcept;

	void setJson(const nlohmann::json& json) noexcept;
	const nlohmann::json& getJson() const noexcept;
	nlohmann::json& getJson() noexcept;

	UUID listenChange(ChangeCallback&& f) noexcept;
	void stopListeningChange(UUID id) noexcept;

private:
	static constexpr auto INDENT_SIZE = 10;

private:
	void contructTree() noexcept;

private:
#pragma region members
	nlohmann::json structure;

	std::unordered_map<UUID, ChangeCallback> changeListeners;
#pragma endregion
};