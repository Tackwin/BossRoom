#include "JsonTree.hpp"

#include "Panel.hpp"
#include "Label.hpp"
#include "ValuePicker.hpp"

#include "Math/algorithms.hpp"
#include "Utils/string_algorithms.hpp"

JsonTree::JsonTree(const nlohmann::json& json) noexcept : Widget(json) {
	if (auto it = json.find("structure"); it != std::end(json)) {
		structure = *it;
		contructTree();
	}
}

void JsonTree::setJson(const nlohmann::json& json) noexcept {
	structure = json;
}
const nlohmann::json& JsonTree::getJson() const noexcept {
	return structure;
}
nlohmann::json& JsonTree::getJson() noexcept {
	return structure;
}

void JsonTree::contructTree() noexcept {
	auto p = makeChild<Panel>({
		{"name", "main_panel" },
		{"sprite", "transparent"},
		{"draggable", false},
		{"collapsable", true},
		{"resizable", false},
		{"collapsed", false},
	});
	p->setVisible(true);

	Vector2f pos = { (float)INDENT_SIZE, INDENT_SIZE / 2.f };
	int it_index = 0;
	for (auto it = structure.begin(); it != structure.end(); ++it, ++it_index) {
		auto key =		(structure.is_object() ? it.key()	: std::to_string(it_index)	);
		auto& value =	(structure.is_object() ? it.value()	: *it						);

		auto offsetLabel = Vector2f{ 0, 0 };
		if (structure.is_object()) {
			auto label = makeChild<Label>({
				{"pos", Vector2f::saveJson(pos) },
				{"name", key + "_label"},
				//{"origin", {0, 0.5}},
				{"font", "consola"},
				{"charSize", 14},
				{"text", key},
				{"textColor", {1, 1, 1, 1}}
				});

			offsetLabel = {
				pos.x + label->getGlobalBoundingBox().w + 5, pos.y
			};
		}

		if (value.is_primitive()) {
			auto str = std::string{};
			if (value.is_number_float()) str = std::to_string(value.get<float>());
			if (value.is_number_integer()) str = std::to_string(value.get<int>());
			if (value.is_number_unsigned()) str = std::to_string(value.get<unsigned>());
			if (value.is_null()) str = "null";
			if (value.is_boolean()) str = value.get<bool>() ? "true" : "false";
			if (value.is_string()) str = value.get<std::string>();

			auto valuePicker = makeChild<ValuePicker>({
				{"pos", Vector2f::saveJson(offsetLabel)},
				{"name", key + "_value"},
				//{"origin", {0, 0.5}},
				{"unfocusedColor", {0.2, 0.2, 0.2, 0.2}},
				{"focusedColor", {0.5, 0.5, 0.5, 0.5}},
				{"font", "consola"},
				{"size", {100, 15}},
				{"charSize", 14},
				{"text", str}
			});
			valuePicker->listenChange(
				[&, copy_value = value, &ref_value = value, valuePicker]
				(const std::string& new_str) mutable {
					if (copy_value.is_number()) {
						if (str::represent_number(new_str)) {
							if (copy_value.is_number_float())
								ref_value = std::stof(new_str);
							if (copy_value.is_number_integer())
								ref_value = std::stoi(new_str);
							if (copy_value.is_number_integer())
								ref_value = (unsigned)std::stoul(new_str);
						}
					}
					if (copy_value.is_string()) {
						ref_value = new_str;
					}
					if (
						copy_value.is_boolean() && new_str == "true" || new_str == "false"
					) {
						ref_value = (new_str == "true");
					}

					for (auto& [_, f] : changeListeners) {
						f(structure);
					}
				}
			);
		}
		else if (value.is_object()) {
			makeChild<JsonTree>({
				//{"origin", {0, 0.5}},
				{"name", key},
				{"pos", Vector2f::saveJson(offsetLabel) },
				{"structure", value}
			})->listenChange([&, key](auto j) mutable {
				value = j;

				//Here we propagate the changes
				for (auto&[_, f] : changeListeners) {
					f(structure);
				}
			});
		}
		else if (value.is_array()) {
			int i = 0;
			pos.y += 20;
			for (auto x : value.get<nlohmann::json::array_t>()) {
				makeChild<JsonTree>({
					//{"origin", {0, 0.5}},
					{"name", key + std::to_string(i++)},
					{"pos", Vector2f::saveJson(pos) },
					{"structure", x}
				})->listenChange([&, copy_i = (i - 1)](auto j) mutable {
					value[copy_i] = j;

					//Here we propagate the changes
					for (auto&[_, f] : changeListeners) {
						f(structure);
					}
				});

				pos.y += 15;
			}
			pos.y -= 15;
		}
		pos.y += 20;
	}
}


UUID JsonTree::listenChange(JsonTree::ChangeCallback&& f) noexcept {
	UUID i;
	changeListeners.emplace(i, f);
	return i;
}

void JsonTree::stopListeningChange(UUID id) noexcept {
	assert(changeListeners.count(id) > 0);
	changeListeners.erase(id);
}
