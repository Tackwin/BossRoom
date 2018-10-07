#include "JsonTree.hpp"

#include "Panel.hpp"
#include "Label.hpp"
#include "PosPicker.hpp"
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
	auto size = Vector2f{ 0, 0 };

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

		auto offsetSize = Vector2f{ 0, 0 };

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
			
			size.x = std::max(label->getSize().x + label->getPosition().x, size.x);
			size.y = std::max(label->getSize().y + label->getPosition().y, size.y);

			offsetSize = label->getSize();
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
			size.x = std::max(
				valuePicker->getSize().x + valuePicker->getPosition().x, size.x
			);
			size.y = std::max(
				valuePicker->getSize().y + valuePicker->getPosition().y, size.y
			);
			offsetSize.y = std::max(offsetSize.y, valuePicker->getSize().y);
			valuePicker->listenChange(
				[&, copy_value = value, &ref_value = value]
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
			pos.y += offsetSize.y;
		}
		else if (value.is_object()) {
			auto jsonChild = makeChild<JsonTree>({
				//{"origin", {0, 0.5}},
				{"name", key},
				{"pos", Vector2f::saveJson(offsetLabel) },
				{"structure", value}
			});
			jsonChild->listenChange([&, key](auto j) mutable {
				value = j;

				//Here we propagate the changes
				for (auto&[_, f] : changeListeners) {
					f(structure);
				}
			});
			size.x = std::max(
				jsonChild->getSize().x + jsonChild->getPosition().x, size.x
			);
			size.y = std::max(
				jsonChild->getSize().y + jsonChild->getPosition().y, size.y
			);

			offsetSize.y = std::max(offsetSize.y, jsonChild->getSize().y);

			pos.y += offsetSize.y;
		}
		else if (value.is_array()) {
			pos.y += 20;
			// Hack
			// We assume that every 2 sized float array represents a vector. Eventually when
			// i'll have compile\run time reflection one way or another i will be able to
			// improve this.
			if (
				it.value().size() == 2 &&
				it.value().at(0).is_number_float() &&
				it.value().at(1).is_number_float()
				) {
				Vector2f x{ it.value().at(0).get<float>(), it.value().at(1).get<float>() };
				auto pos_picker = p->makeChild<PosPicker>({
					{"pos", offsetLabel},
					{"name", it.key() + "_value"},
					{"x", x},
					{"label", {
						{"font", "consola"},
						{"charSize", 13}
					}}
				});
				pos_picker->listenChange(
					[&, copy_value = value, &ref_value = value](const Vector2f& pos) {
						ref_value = pos;
						for (auto&[_, f] : changeListeners) {
							f(structure);
						}
					}
				);
			}
			else {
				int i = 0;
				for (auto x : value.get<nlohmann::json::array_t>()) {
					auto jsonChild = makeChild<JsonTree>({
						//{"origin", {0, 0.5}},
						{"name", key + std::to_string(i++)},
						{"pos", Vector2f::saveJson(pos) },
						{"structure", x}
					});
					jsonChild->listenChange([&, copy_i = (i - 1)](auto j) mutable {
						value[copy_i] = j;

						//Here we propagate the changes
						for (auto&[_, f] : changeListeners) {
							f(structure);
						}
					});
					size.x = std::max(
						jsonChild->getSize().x + jsonChild->getPosition().x, size.x
					);
					size.y = std::max(
						jsonChild->getSize().y + jsonChild->getPosition().y, size.y
					);
					offsetSize.y = std::max(offsetSize.y, jsonChild->getSize().y);

					pos.y += offsetSize.y;
				}
			}
		}
	}
	setSize(size);
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
