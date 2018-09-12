#include "algorithm.hpp"

#include "ValuePicker.hpp"
#include "Panel.hpp"

constexpr auto indent_size = 10;

void populate_widget_with_editable_json_form(
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
				{"name", it.key()},
				{"unfocusedColor", {0.2, 0.2, 0.2, 0.2}},
				{"focusedColor", {0.5, 0.5, 0.5, 0.5}},
				{"font", "consola"},
				{"charSize", 10},
				{"text", str}
			}});

			// Now _you_ deal with this.
			p->addChild(picker);
		}
		else if (it.value().is_object()) {
			Widget* deep_w = new Widget{ nlohmann::json{
				{"name", it.key()},
				{"pos", Vector2f::saveJson(pos)}
			}};

			populate_widget_with_editable_json_form(deep_w, it.key());
			
			p->addChild(deep_w);
		}
		else if (it.value().is_array()) {
			int i = 0;
			for (auto x : it.value().get<nlohmann::json::array_t>()) {
				Widget* deep_w = new Widget{ nlohmann::json{
					{"pos", Vector2f::saveJson(pos)},
					{"name", it.key() + std::to_string(i++)}
				}};

				//populate_widget_with_editable_json_form(deep_w, x);

				p->addChild(deep_w);

				pos.y += 11;
			}
		}
		pos.y += 14;
	}

	w->addChild(p);
}