#include "dyn_structTree.hpp"

#include "Panel.hpp"
#include "Label.hpp"
#include "PosPicker.hpp"
#include "BoolPicker.hpp"
#include "ValuePicker.hpp"

#include "Math/algorithms.hpp"
#include "Utils/string_algorithms.hpp"

#include "Common.hpp"

dyn_structTree::dyn_structTree(const nlohmann::json& json) noexcept : Widget(json) {
	if (auto it = json.find("structure"); it != std::end(json)) {
		structure = *it;
		contruct_tree();
	}
}

void dyn_structTree::set_dyn_struct(const dyn_struct& d_struct) noexcept {
	structure = d_struct;
}

void dyn_structTree::contruct_tree() noexcept {
	if (structure.type_tag == "Vector2<T>"_id) {
		auto pos_picker = makeChild<PosPicker>({
			{"pos", _pos},
			{"name", "value"},
			{"x", (Vector2f)structure},
			{"label", {
				{"font", "consola"},
				{"charSize", 13}
			}}
		});
		pos_picker->listenChange(
			[&, &ref_value = structure](const Vector2f& pos) {
				ref_value = pos;
				for (auto&[_, f] : change_listeners) {
					f(structure);
				}
			}
		);

		setSize(pos_picker->getSize());
		return;
	}
	if (holds_object(structure)) {
		Vector2f size;
		Vector2f p = _pos;

		for (auto&[k, v] : iterate_structure(structure)) {
			auto label = makeChild<Label>({
				{"pos", p },
				{"name", k + "_label"},
				//{"origin", {0, 0.5}},
				{"font", "consola"},
				{"charSize", 14},
				{"text", k},
				{"textColor", {1, 1, 1, 1}}
			});

			auto child = makeChild<dyn_structTree>({
				{"name", k + "_value"},
				{"pos", {p.x + INDENT_SIZE, p.y + label->getSize().y}},
				{"structure", v}
			});
			child->listen_change([&, &ref_value = v](const dyn_struct& s) {
				ref_value = s;
				for (auto&[_, f] : change_listeners) {
					f(structure);
				}
			});

			size.x = std::max(size.x, INDENT_SIZE + child->getSize().x);
			p.y += std::max(label->getSize().y, child->getSize().y);
			p.y += 5;
		}

		size.y = p.y - _pos.y;
		setSize(size);
		return;
	}
	if (holds_array(structure)) {
		Vector2f size;
		Vector2f p = _pos;

		size_t i = 0;
		for (auto& v : iterate_array(structure)) {
			auto child = makeChild<dyn_structTree>({
				{"name", i + "_value"},
				{"pos", {p.x + INDENT_SIZE, p.y}},
				{"structure", v}
			});

			size.x = std::max(size.x, INDENT_SIZE + child->getSize().x);
			p.y += child->getSize().y;
			p.y += 5;
			++i;
		}
		size.y = p.y - _pos.y;

		setSize(size);
		return;
	}
	if (holds_primitive(structure)) {
		auto str = std::string{};
		if (holds_real(structure)) str = std::to_string((long double)structure);
		if (holds_integer(structure)) str = std::to_string((long long)structure);
		if (holds_null(structure)) str = "null";
		if (holds_string(structure)) str = std::get<std::string>(structure.value);

		if (holds_bool(structure)) {
			auto boolPicker = makeChild<BoolPicker>({
				{"pos", _pos},
				{"name", "value"},
				{"size", {15, 15}},
				{"x", (bool)structure}
			});

			boolPicker->listenChange([&, &ref_value = structure] (bool x) mutable {
				ref_value = x;

				for (auto&[_, f] : change_listeners) {
					f(structure);
				}
			});

			setSize(boolPicker->getSize());
			return;
		}

		auto valuePicker = makeChild<ValuePicker>({
			{"pos", _pos },
			{"name", "value"},
			//{"origin", {0, 0.5}},
			{"unfocusedColor", {0.2, 0.2, 0.2, 0.2}},
			{"focusedColor", {0.5, 0.5, 0.5, 0.5}},
			{"font", "consola"},
			{"size", {100, 15}},
			{"charSize", 14},
			{"text", str}
			});

		valuePicker->listenChange(
			[&, copy_value = structure, &ref_value = structure]
			(const std::string& new_str) mutable {
				if (holds_number(copy_value)) {
					if (str::represent_number(new_str)) {
						if (holds_real(copy_value))
							ref_value = std::stof(new_str);
						if (holds_integer(copy_value))
							ref_value = std::stoi(new_str);
					}
				}

				if (holds_string(copy_value)) {
					ref_value = new_str;
				}
				if (holds_bool(copy_value)) {
					ref_value = (new_str == "true");
				}

				for (auto&[_, f] : change_listeners) {
					f(structure);
				}
			}
		);

		setSize(valuePicker->getSize());
	}
}


UUID dyn_structTree::listen_change(dyn_structTree::ChangeCallback&& f) noexcept {
	UUID id;
	change_listeners[id] = std::move(f);
	return id;
}

void dyn_structTree::stop_listening_change(UUID id) noexcept {
	assert(change_listeners.count(id) != 0);
	change_listeners.erase(id);
}
