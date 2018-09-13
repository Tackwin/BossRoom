#include "algorithm.hpp"

#include "ValuePicker.hpp"
#include "Panel.hpp"

constexpr auto indent_size = 10;

void populate_widget_with_editable_json_object_form(
	Widget* w, const nlohmann::json& j
) noexcept {
	Panel* p = new Panel{ nlohmann::json{
		{"name", "main_panel"},
		{"sprite", "transparent"},
		{"draggable", false},
		{"collapsable", true},
		{"resizable", false},
		{"collapsed", false},
	}};
	p->setVisible(true);

	Vector2f pos = { (float)indent_size, 0 };
	for (auto it = j.begin(); it != j.end(); ++it) {
		if (it.value().is_primitive()) {
			auto str = std::string{};
			if (it.value().is_number()) str = std::to_string(it.value().get<long double>());
			if (it.value().is_null()) str = "null";
			if (it.value().is_boolean()) str = it.value().get<bool>() ? "true" : "false";
			if (it.value().is_string()) str = it.value().get<std::string>();

			Label* label = new Label{nlohmann::json{
				{"pos", Vector2f::saveJson(pos) },
				{"name", it.key() + "_label"},
				{"origin", {0, 0.5}},
				{"font", "consola"},
				{"charSize", 14},
				{"text", it.key()},
				{"textColor", {1, 1, 1, 1}}
			}};
			auto offsetLabel = Vector2f{
				pos.x + label->getGlobalBoundingBox().w + 2, pos.y
			};
			ValuePicker* picker = new ValuePicker({ nlohmann::json{
				{"pos", Vector2f::saveJson(offsetLabel)},
				{"name", it.key() + "_value"},
				{"origin", {0, 0.5}},
				{"unfocusedColor", {0.2, 0.2, 0.2, 0.2}},
				{"focusedColor", {0.5, 0.5, 0.5, 0.5}},
				{"font", "consola"},
				{"size", {100, 15}},
				{"charSize", 10},
				{"text", str}
			}});

			// Now _you_ deal with this.
			p->addChild(label);
			p->addChild(picker);
		}
		else if (it.value().is_object()) {
			Label* label = new Label{ nlohmann::json{
				{"pos", Vector2f::saveJson(pos) },
				{"name", it.key() + "_label"},
				{"font", "consola"},
				{"charSize", 14},
				{"text", it.key()},
				{"textColor", {1, 1, 1, 1}}
			} };
			auto offsetLabel = Vector2f{
				pos.x + label->getGlobalBoundingBox().w + 2, pos.y
			};
			Widget* deep_w = new Widget{ nlohmann::json{
				{"name", it.key()},
				{"pos", Vector2f::saveJson(offsetLabel)}
			}};

			populate_widget_with_editable_json_object_form(deep_w, it.value());
			
			p->addChild(label);
			p->addChild(deep_w);
		}
		else if (it.value().is_array()) {
			int i = 0;
			Label* label = new Label{ nlohmann::json{
				{"pos", Vector2f::saveJson(pos) },
				{"name", it.key() + "_label"},
				{"font", "consola"},
				{"charSize", 14},
				{"text", it.key()},
				{"textColor", {1, 1, 1, 1}}
			} };
			pos.y += 11;
			for (auto x : it.value().get<nlohmann::json::array_t>()) {
				Widget* deep_w = new Widget{ nlohmann::json{
					{"pos", Vector2f::saveJson(pos)},
					{"name", it.key() + std::to_string(i++)}
				}};

				populate_widget_with_editable_json_array_form(deep_w, x);

				p->addChild(deep_w);

				pos.y += 11;
			}
			p->addChild(label);
		}
		pos.y += 14;
	}

	w->addChild(p);
}


void populate_widget_with_editable_json_array_form(
	Widget* w, const nlohmann::json& j
) noexcept {
	Panel* p = new Panel{ nlohmann::json{
		{"name", "main_panel"},
		{"sprite", "transparent"},
		{"draggable", false},
		{"collapsable", true},
		{"resizable", false},
		{"collapsed", false},
	} };
	p->setVisible(true);

	Vector2f pos = { (float)indent_size, 0 };
	for (auto it = j.begin(); it != j.end(); ++it) {
		if (it.value().is_primitive()) {
			auto str = std::string{};
			if (it.value().is_number()) str = std::to_string(it.value().get<long double>());
			if (it.value().is_null()) str = "null";
			if (it.value().is_boolean()) str = it.value().get<bool>() ? "true" : "false";
			if (it.value().is_string()) str = it.value().get<std::string>();

			ValuePicker* picker = new ValuePicker({ nlohmann::json{
				{"pos", Vector2f::saveJson(pos)},
				{"name", std::to_string(it - j.begin()) +"_value"},
				{"origin", {0, 0.5}},
				{"unfocusedColor", {0.2, 0.2, 0.2, 0.2}},
				{"focusedColor", {0.5, 0.5, 0.5, 0.5}},
				{"font", "consola"},
				{"size", {100, 15}},
				{"charSize", 10},
				{"text", str}
			} });

			// Now _you_ deal with this.
			p->addChild(picker);
		}
		else if (it.value().is_object()) {
			Widget* deep_w = new Widget{ nlohmann::json{
				{"name", std::to_string(it - j.begin())},
				{"pos", Vector2f::saveJson(pos)}
			} };

			populate_widget_with_editable_json_object_form(deep_w, it.value());

			p->addChild(deep_w);
		}
		else if (it.value().is_array()) {
			int i = 0;
			for (auto x : it.value().get<nlohmann::json::array_t>()) {
				Widget* deep_w = new Widget{ nlohmann::json{
					{"pos", Vector2f::saveJson(pos)},
					{"name", std::to_string(i++)}
				} };

				populate_widget_with_editable_json_array_form(deep_w, x);

				p->addChild(deep_w);

				pos.y += 11;
			}
		}
		pos.y += 14;
	}

	w->addChild(p);
}