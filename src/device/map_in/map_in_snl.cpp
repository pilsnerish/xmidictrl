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

#include "map_in_snl.h"

// fmt
#include "fmt/format.h"

// XMidiCtrl
#include "toml_utils.h"

namespace xmidictrl {

//---------------------------------------------------------------------------------------------------------------------
//   CONSTRUCTOR / DESTRUCTOR
//---------------------------------------------------------------------------------------------------------------------

/**
 * Constructor
 */
map_in_snl::map_in_snl(environment& in_env, bool in_legacy_mode)
	: map_in(in_env)
	, m_legacy_mode(in_legacy_mode)
{
	// create label objects
	m_label_short = std::make_unique<label>(in_env);
	m_label_long = std::make_unique<label>(in_env);
}




//---------------------------------------------------------------------------------------------------------------------
//   PUBLIC
//---------------------------------------------------------------------------------------------------------------------

/**
 * Return the mapping type
 */
map_in_type map_in_snl::type()
{
	if (m_legacy_mode)
		return map_in_type::push_pull;
	else
		return map_in_type::short_and_long;
}


/**
 * Set the time point when the initial message was received
 */
void map_in_snl::set_time_received()
{
	// reset all times first
	reset();

	m_time_received.store(std::chrono::system_clock::now());
}


/**
 * Set the time point when the initial message was released
 */
void map_in_snl::set_time_released()
{
	// only set the released time, if a received time is present
	if (m_time_received.load() > time_point::min())
		m_time_released.store(std::chrono::system_clock::now());
}


/**
 * Read settings from config
 */
void map_in_snl::read_config(text_logger& in_log, toml::value& in_data, toml::value& in_config)
{
	if (m_legacy_mode) {
		in_log.debug_line(in_data.location().line(), "Read settings for type 'pnp'");
		in_log.warn_line(in_data.location().line(), "Obsolete mapping type, please use 'snl' instead!");
	} else {
		in_log.debug_line(in_data.location().line(), "Read settings for type 'snl'");
	}

	map_in::read_config(in_log, in_data, in_config);

	if (m_legacy_mode) {
		if (toml_utils::contains(in_log, in_data, c_cfg_dataref_push)) {
			// read dataref push
			m_dataref_short = toml_utils::read_string(in_log, in_data, c_cfg_dataref_push);
			m_values_short = toml_utils::read_str_vector_array(in_log, in_data, c_cfg_values_push);
		} else {
			// read command push
			m_command_short = toml_utils::read_string(in_log, in_data, c_cfg_command_push);
		}
	} else {
		if (toml_utils::contains(in_log, in_data, c_cfg_dataref_short)) {
			// read dataref short
			m_dataref_short = toml_utils::read_string(in_log, in_data, c_cfg_dataref_short);
			m_values_short = toml_utils::read_str_vector_array(in_log, in_data, c_cfg_values_short);
		} else {
			// read command short
			m_command_short = toml_utils::read_string(in_log, in_data, c_cfg_command_short);
		}
	}

	if (m_legacy_mode) {
		if (toml_utils::contains(in_log, in_data, c_cfg_dataref_pull)) {
			// read dataref pull
			m_dataref_long = toml_utils::read_string(in_log, in_data, c_cfg_dataref_pull);
			m_values_long = toml_utils::read_str_vector_array(in_log, in_data, c_cfg_values_pull);
		} else {
			// read command pull
			m_command_long = toml_utils::read_string(in_log, in_data, c_cfg_command_pull);
		}
	} else {
		if (toml_utils::contains(in_log, in_data, c_cfg_dataref_long)) {
			// read dataref long
			m_dataref_long = toml_utils::read_string(in_log, in_data, c_cfg_dataref_long);
			m_values_long = toml_utils::read_str_vector_array(in_log, in_data, c_cfg_values_long);
		} else {
			// read command long
			m_command_long = toml_utils::read_string(in_log, in_data, c_cfg_command_long);
		}
	}

	// read config for labels
	m_label_short->read_config(in_log, in_data, in_config, m_dataref_short, c_cfg_label_short);
	m_label_long->read_config(in_log, in_data, in_config, m_dataref_long, c_cfg_label_long);
}


/**
 * Check the mapping
 */
bool map_in_snl::check(text_logger& in_log, const midi_device_settings& in_dev_settings)
{
	bool result = true;

	if (!map::check(in_log, in_dev_settings))
		result = false;

	// short
	if (!m_dataref_short.empty()) {
		if (!env().drf().check(m_dataref_short)) {
			in_log.error(source_line());
			in_log.error(fmt::format(" --> Dataref '{}' not found", m_dataref_short));
			result = false;
		}

		if (m_values_short.empty()) {
			in_log.error(source_line());
			if (m_legacy_mode)
				in_log.error(fmt::format(" --> Parameter '{}' is not defined", c_cfg_values_push));
			else
				in_log.error(fmt::format(" --> Parameter '{}' is not defined", c_cfg_values_short));

			result = false;
		}
	} else {
		if (m_command_short.empty()) {
			in_log.error(source_line());
			if (m_legacy_mode)
				in_log.error(fmt::format(" --> Parameter '{}' is empty", c_cfg_command_push));
			else
				in_log.error(fmt::format(" --> Parameter '{}' is empty", c_cfg_command_short));
			result = false;
		}
	}

	// long
	if (!m_dataref_long.empty()) {
		if (!env().drf().check(m_dataref_long)) {
			in_log.error(source_line());
			in_log.error(fmt::format(" --> Dataref '{}' not found", m_dataref_long));
			result = false;
		}

		if (m_values_long.empty()) {
			in_log.error(source_line());
			if (m_legacy_mode)
				in_log.error(fmt::format(" --> Parameter '{}' is not defined", c_cfg_values_pull));
			else
				in_log.error(fmt::format(" --> Parameter '{}' is not defined", c_cfg_values_long));

			result = false;
		}
	} else {
		if (m_command_long.empty()) {
			in_log.error(source_line());
			if (m_legacy_mode)
				in_log.error(fmt::format(" --> Parameter '{}' is empty", c_cfg_command_pull));
			else
				in_log.error(fmt::format(" --> Parameter '{}' is empty", c_cfg_command_long));

			result = false;
		}
	}

	// check labels
	if (!m_label_short->check(in_log, source_line()))
		result = false;

	if (!m_label_long->check(in_log, source_line()))
		result = false;

	return result;
}


/**
 * Execute the action in X-Plane
 */
std::unique_ptr<map_result> map_in_snl::execute(map_param* in_param)
{
	auto result = std::make_unique<map_result>();

	auto param_in = get_param_in(in_param);
	if (param_in == nullptr)
		return result;

	if (!check_sublayer(param_in->sl_value()) || m_time_received.load() == time_point::min()) {
		// wrong sublayer (or received time is missing)
		reset();

		result->completed = true;
		return result;
	}

	// check if the command has been already executed
	if (m_time_command > time_point::min()) {
		std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - m_time_command;

		if (elapsed.count() > 0.3f) {
			switch (m_command_type) {
				case command_type::cmd_short:
					if (!m_command_short.empty()) {
						if (m_legacy_mode)
							param_in->msg().log().debug(fmt::format(" --> End push command '{}'", m_command_short));
						else
							param_in->msg().log().debug(fmt::format(" --> End short command '{}'", m_command_short));

						env().cmd().end(param_in->msg().log(), m_command_short);

						m_label_short->display_label(param_in->msg().log());
					}
					break;

				case command_type::cmd_long:
					if (!m_command_long.empty()) {
						if (m_legacy_mode)
							param_in->msg().log().debug(fmt::format(" --> End pull command '{}'", m_command_long));
						else
							param_in->msg().log().debug(fmt::format(" --> End long command '{}'", m_command_long));

						env().cmd().end(param_in->msg().log(), m_command_long);

						m_label_long->display_label(param_in->msg().log());
					}
					break;

				case command_type::none:
					break;
			}

			reset();

			result->completed = true;
			return result;
		} else {
			// let's wait a bit longer...
			result->completed = false;
			return result;
		}
	}

	if (m_time_released.load() == time_point::min()) {
		std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - m_time_received.load();

		if (elapsed_seconds.count() < 0.5f) {
			result->completed = false;
			return result;
		}
	}

	std::chrono::duration<double> elapsed_seconds = m_time_released.load() - m_time_received.load();
	m_time_command = std::chrono::system_clock::now();

	if (elapsed_seconds.count() > 0.5f || m_time_released.load() == time_point::min()) {
		if (!m_dataref_long.empty()) {
			toggle_dataref(param_in->msg().log(), m_dataref_long, m_values_long, true);
		} else if (!m_command_long.empty()) {
			if (m_legacy_mode)
				param_in->msg().log().debug(fmt::format(" --> Begin pull command '{}'", m_command_long));
			else
				param_in->msg().log().debug(fmt::format(" --> Begin long command '{}'", m_command_long));

			m_command_type = command_type::cmd_long;
			env().cmd().begin(param_in->msg().log(), m_command_long);

			// keep the task in the queue to end the command
			result->completed = false;
			return result;
		}
	} else {
		if (!m_dataref_short.empty()) {
			toggle_dataref(param_in->msg().log(), m_dataref_short, m_values_short, true);
		} else if (!m_command_short.empty()) {
			if (m_legacy_mode)
				param_in->msg().log().debug(fmt::format(" --> Begin push command '{}'", m_command_short));
			else
				param_in->msg().log().debug(fmt::format(" --> Begin short command '{}'", m_command_short));

			m_command_type = command_type::cmd_short;
			env().cmd().begin(param_in->msg().log(), m_command_short);

			// keep the task in the queue to end the command
			result->completed = false;
			return result;
		}
	}

	// don't keep the task in the queue
	result->completed = true;
	return result;
}


/*
 * Return the label name(s)
 */
std::string map_in_snl::map_text_label()
{
	return {};
}


/**
 * Return mapped command
 */
std::string map_in_snl::map_text_cmd_drf()
{
	std::string map_str {};

	if (m_legacy_mode) {
		if (!m_dataref_short.empty())
			map_str.append(m_dataref_short + "   (dataref push)");
		else
			map_str.append(m_command_short + "   (command push)");

		if (!m_dataref_long.empty())
			map_str.append("\n" + m_dataref_long + "   (dataref pull)");
		else
			map_str.append("\n" + m_command_long + "   (command pull)");
	} else {
		if (!m_dataref_short.empty())
			map_str.append(m_dataref_short + "   (dataref short)");
		else
			map_str.append(m_command_short + "   (command short)");

		if (!m_dataref_long.empty())
			map_str.append("\n" + m_dataref_long + "   (dataref long)");
		else
			map_str.append("\n" + m_command_long + "   (command long)");
	}

	return map_str;
}


/**
 * Return mapped parameter
 */
std::string map_in_snl::map_text_parameter()
{
	std::string map_str {};

	return map_str;
}




//---------------------------------------------------------------------------------------------------------------------
//   PROTECTED
//---------------------------------------------------------------------------------------------------------------------

/**
 * Return the mapping as string
 */
std::string map_in_snl::build_mapping_text(bool in_short)
{
	std::string map_str;

	if (!in_short) {
		if (m_legacy_mode)
			map_str = " ====== Push & Pull ======\n";
		else
			map_str = " ====== Short & Long ======\n";

		if (!sl().empty())
			map_str.append("Sublayer = '" + std::string(sl()) + "'\n");
	}

	// TODO: Add dataref option

	//map_str.append("Cmd Push: " + m_command_push + "\n");
	//map_str.append("Cmd Pull: " + m_command_pull);

	return map_str;
}




//---------------------------------------------------------------------------------------------------------------------
//   PRIVATE
//---------------------------------------------------------------------------------------------------------------------

/**
 * Reset all data
 */
void map_in_snl::reset()
{
	m_command_type = command_type::none;
	m_time_command = time_point::min();

	m_time_received.store(time_point::min());
	m_time_released.store(time_point::min());
}

} // Namespace xmidictrl