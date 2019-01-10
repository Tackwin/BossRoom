#include "StructViewer.hpp"
#include "Math/Vector.hpp"
#include "Common.hpp"
#include "Panel.hpp"
#include "Label.hpp"


StructViewer::StructViewer(const StructViewerOpts& opts) noexcept :
	Widget(nlohmann::json{
		{"pos", opts.pos}
	}),
	d_struct(opts.d_struct)
{
	if (holds_integer(d_struct)) {
		makeChild<Label>({
			{"font", "consola"},
			{"charSize", opts.font_size},
			{"text", std::to_string((long long int)d_struct)}
		});
		return;
	}
	if (holds_bool(d_struct)) {
		auto b = (bool)d_struct;

		makeChild<Panel>({
			{"draggable", false},
			{"resizable", false},
			{"sprite", b ? "on_on_off" : "off_on_off"},
			{"size", Vector2f{opts.font_size * 1.5f, opts.font_size * 1.5f}},
		});
		return;
	}
	if (holds_real(d_struct)) {
		makeChild<Label>({
			{"font", "consola"},
			{"charSize", opts.font_size},
			{"text", std::to_string((long double)d_struct)}
		});
		return;
	}
	if (holds_null(d_struct)) {
		makeChild<Label>({
			{"font", "consola"},
			{"charSize", opts.font_size},
			{"text", "null"}
		});
		return;
	}
	if (holds_string(d_struct)) {
		makeChild<Label>({
			{"font", "consola"},
			{"charSize", opts.font_size},
			{"text", (std::string)d_struct}
		});
		return;
	}
	if (d_struct.type_tag == Vector2_Type_Tag) {
		auto label_x = makeChild<Label>({
			{"font", "consola"},
			{"charSize", opts.font_size},
			{"text", std::to_string((float)*at(d_struct, "x"))}
		});
		makeChild<Label>({
			{"pos", Vector2f{label_x->getSize().x + opts.font_size / 2.f, 0}},
			{"font", "consola"},
			{"charSize", opts.font_size},
			{"text", std::to_string((float)*at(d_struct, "y"))}
		});

		return;
	}
	if (holds_array(d_struct)) {
		auto plus_panel = makeChild<Panel>({
			{"size", Vector2f{opts.font_size * 1.5f, opts.font_size * 1.5f}},
			{"sprite", "minus_button"},
			{"draggable", false},
			{"resizable", false}
		});

		makeChild<Label>({
			{"font", "consola"},
			{"charSize", opts.font_size},
			{"text", ":"}
		});



		Vector2f offset{ opts.font_size * 4.f, opts.font_size * 1.5f };
		for (const auto& x : iterate_array(d_struct)) {
			StructViewerOpts new_opts;
			new_opts.d_struct = x;
			new_opts.font_size = opts.font_size;
			new_opts.pos = offset;

			auto added = new StructViewer{ new_opts };
			addChild((Widget*)added);

			offset.y += added->getSize().y;
			offset.y += opts.font_size;
		}

		Callback on_click;
		on_click.ended = [plus_panel, plus = false]() mutable {
			plus = !plus;
			plus_panel->setTexture(plus ? "plus_button" : "minus_button");
			return true;
		};
		plus_panel->setOnClick(on_click);
	}
	if (holds_object(d_struct)) {
		auto plus_panel = makeChild<Panel>({
			{"size", Vector2f{opts.font_size * 1.5f, opts.font_size * 1.5f}},
			{"sprite", "minus_button"},
			{"draggable", false},
			{"resizable", false}
		});

		auto label_colon = makeChild<Label>({
			{"font", "consola"},
			{"pos", Vector2f{plus_panel->getSize().x, 0}},
			{"charSize", opts.font_size},
			{"text", ": "}
		});

		Vector2f offset{ label_colon->getPosition().x + label_colon->getSize().x, 0 };
		for (const auto& [name, x] : iterate_structure(d_struct)) {
			auto label_name = makeChild<Label>({
				{"font", "consola"},
				{"charSize", opts.font_size},
				{"pos", offset},
				{"text", name + ": "} 
			});

			StructViewerOpts new_opts;
			new_opts.d_struct = x;
			new_opts.font_size = opts.font_size;
			new_opts.pos = offset + Vector2f{label_name->getSize().x, 0};

			auto added = new StructViewer{ new_opts };
			addChild((Widget*)added);

			offset.y += std::max({ added->getSize().y, label_name->getSize().y });
			offset.y += opts.font_size;
		}

		Callback on_click;
		on_click.ended = [plus_panel, plus = false]() mutable {
			plus = !plus;
			plus_panel->setTexture(plus ? "plus_button" : "minus_button");
			return true;
		};
		plus_panel->setOnClick(on_click);
	}
}
