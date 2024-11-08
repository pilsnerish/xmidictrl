//---------------------------------------------------------------------------------------------------------------------
//   XMidiCtrl - MIDI Controller plugin for X-Plane
//
//   Copyright (c) 2021-2024 Marco Auer
//
//   XMidiCtrl is free software: you can redistribute it and/or modify it under the terms of the
//   GNU Affero General Public License as published by the Free Software Foundation, either version 3
//   of the License, or (at your option) any later version.
//
//   XMidiCtrl is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
//   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU Affero General Public License for more details.
//
//   You should have received a copy of the GNU Affero General Public License along with XMidiCtrl.
//   If not, see <https://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------------------------------------------------

#ifndef XMC_MAP_IN_CBV_H
#define XMC_MAP_IN_CBV_H

// Standard
#include <string>
#include <string_view>
#include <vector>

// toml11
#include <toml.hpp>

// XMidiCtrl
#include "label.h"
#include "map_in.h"
#include "midi_message.h"
#include "text_logger.h"

namespace xmidictrl {

//---------------------------------------------------------------------------------------------------------------------
//   CLASS
//---------------------------------------------------------------------------------------------------------------------

class map_in_cbv : public map_in {
public:
	explicit map_in_cbv(environment& in_env);
	~map_in_cbv() override = default;

	map_in_type type() override;

	void read_config(text_logger& in_log, toml::value& in_data, toml::value& in_config) override;
	bool check(text_logger& in_log, const midi_device_settings& in_dev_settings) override;

	std::unique_ptr<map_result> execute(map_param* in_param) override;

	std::string map_text_label() override;
	std::string map_text_cmd_drf() override;
	std::string map_text_parameter() override;

protected:
	std::string build_mapping_text(bool in_short) override;

private:
	// constants
	static constexpr std::string_view c_cfg_dataref {"dataref"};
	static constexpr std::string_view c_cfg_values {"values"};
	static constexpr std::string_view c_cfg_value {"value"};
	static constexpr std::string_view c_cfg_command {"command"};

	std::string m_dataref {};

	std::map<std::string, std::string> m_values {};
};

} // Namespace xmidictrl

#endif // XMC_MAP_IN_CBV_H