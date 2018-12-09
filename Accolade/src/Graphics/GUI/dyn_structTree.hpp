#pragma once
#include "Widget.hpp"

#include <unordered_map>
#include <string>
#include <functional>

#include "3rd/json.hpp"
#include "RTTI/dyn_struct.hpp"

#include "Utils/UUID.hpp"
#include "Entity/EntityStore.hpp"
#include "Entity/Eid.hpp"
#include "Entity/OwnId.hpp"

class dyn_structTree : public Widget {
public:
	using ChangeCallback = std::function<void(const dyn_struct&)>;

	static constexpr auto NAME = "dyn_structTree";

	dyn_structTree(const nlohmann::json& json) noexcept;

	void set_dyn_struct(const dyn_struct& d_struct) noexcept;

	UUID listen_change(ChangeCallback&& f) noexcept;
	void stop_listening_change(UUID id) noexcept;

	bool is_editable() const noexcept;
	void set_editable(bool v) noexcept;

	bool is_expendable() const noexcept;
	void set_expendable(bool v) noexcept;

private:
	static constexpr auto INDENT_SIZE = 10;

private:
	void contruct_tree() noexcept;

private:
	dyn_struct structure;

	bool editable{ false };
	bool expendable{ false };
	std::unordered_map<UUID, ChangeCallback> change_listeners;
};