#pragma once

#include "Widget.hpp"
#include "3rd/json.hpp"

extern void populate_widget_with_editable_json_object_form(
	Widget* w, const nlohmann::json& j
) noexcept;
extern void populate_widget_with_editable_json_array_form(
	Widget* w, const nlohmann::json& j
) noexcept;