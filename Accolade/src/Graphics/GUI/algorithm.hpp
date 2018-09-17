#pragma once

#include "Widget.hpp"
#include "3rd/json.hpp"

extern void populate_widget_with_editable_json_object_form(
	Widget* w, const nlohmann::json& j
) noexcept;
extern void populate_widget_with_editable_json_array_form(
	Widget* w, const nlohmann::json& j
) noexcept;

extern nlohmann::json get_json_object_from_widget_tree(const Widget* w) noexcept;
extern nlohmann::json get_json_array_from_widget_tree(const Widget* w) noexcept;
