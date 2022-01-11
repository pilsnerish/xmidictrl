//---------------------------------------------------------------------------------------------------------------------
//   XMidiCtrl - MIDI Controller plugin for X-Plane
//
//   Copyright (c) 2021-2022 Marco Auer
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

#include "inbound_worker.h"

// XMidiCtrl
#include "logger.h"

namespace xmidictrl {

//---------------------------------------------------------------------------------------------------------------------
//   CONSTRUCTOR / DESTRUCTOR
//---------------------------------------------------------------------------------------------------------------------

/**
 * Constructor
 */
inbound_worker::inbound_worker()
{
}


/**
 * Destructor
 */
inbound_worker::~inbound_worker()
{
}




//---------------------------------------------------------------------------------------------------------------------
//   PUBLIC
//---------------------------------------------------------------------------------------------------------------------


/**
 * Add a task to be executed
 */
void inbound_worker::add_task(const std::shared_ptr<inbound_task> &task)
{
    std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    LOG_DEBUG << " --> Task added to inbound queue" << LOG_END

    m_tasks.push(task);
}


/**
 * Process inbound tasks
 */
void inbound_worker::process(std::string_view sl_value)
{
    std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    std::queue<std::shared_ptr<inbound_task>> temp_list;

    // process the midi inbound queue for each midi device
    while (!m_tasks.empty()) {
        std::shared_ptr<inbound_task> t = m_tasks.front();

        if (t == nullptr || t->map == nullptr)
            continue;

        // perform the action related to the mapping
        if (!t->map->execute(*t->msg, sl_value)) {
            // store in temp list
            temp_list.push(t);
        }

        // delete entry from list
        m_tasks.pop();
    }

    // add temp tasks to queue again, will be executed in next flight loop
    while (!temp_list.empty()) {
        std::shared_ptr<inbound_task> t = temp_list.front();
        m_tasks.push(t);

        temp_list.pop();
    }
}




//---------------------------------------------------------------------------------------------------------------------
//   PRIVATE
//---------------------------------------------------------------------------------------------------------------------


} // Mamespace xmidictrl