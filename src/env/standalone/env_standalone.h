//---------------------------------------------------------------------------------------------------------------------
//   XMidiCtrl - MIDI Controller plugin for X-Plane
//
//   Copyright (c) 2021-2023 Marco Auer
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

#ifndef ENV_STANDALONE_H
#define ENV_STANDALONE_H

#include <string>

// xmidictrl
#include "cmd_standalone.h"
#include "data_standalone.h"
#include "environment.h"

namespace xmidictrl {

class env_standalone : public environment {
public:
    explicit env_standalone(text_logger &in_log);

    std::filesystem::path xplane_path() override;

    std::filesystem::path plugin_path() override;
    std::filesystem::path preferences_path() override;
    std::filesystem::path profiles_path() override;
    std::filesystem::path includes_path() override;

    std::filesystem::path current_aircraft_path() override;

    std::string current_aircraft_author() override;
    std::string current_aircraft_icao() override;
    std::string current_aircraft_acf_name() override;
    std::string current_aircraft_descr() override;

    data &drf() override;
    commands &cmd() override;

private:
    std::unique_ptr<cmd_standalone> m_cmd;
    std::unique_ptr<data_standalone> m_drf;
};

} // Namespace xmidictrl

#endif // ENV_STANDALONE_H