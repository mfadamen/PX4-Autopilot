/****************************************************************************
 *
 *   Copyright (c) 2020 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef COLLISION_HPP
#define COLLISION_HPP

#include <uORB/topics/collision_report.h>

class MavlinkStreamCollision : public MavlinkStream
{
public:
	const char *get_name() const override
	{
		return MavlinkStreamCollision::get_name_static();
	}

	static constexpr const char *get_name_static()
	{
		return "COLLISION";
	}

	static constexpr uint16_t get_id_static()
	{
		return MAVLINK_MSG_ID_COLLISION;
	}

	uint16_t get_id() override
	{
		return get_id_static();
	}

	static MavlinkStream *new_instance(Mavlink *mavlink)
	{
		return new MavlinkStreamCollision(mavlink);
	}

	unsigned get_size() override
	{
		return _collision_sub.advertised() ? MAVLINK_MSG_ID_COLLISION_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
	}

private:
	uORB::Subscription _collision_sub{ORB_ID(collision_report)};

	/* do not allow top copying this class */
	MavlinkStreamCollision(MavlinkStreamCollision &) = delete;
	MavlinkStreamCollision &operator = (const MavlinkStreamCollision &) = delete;

protected:
	explicit MavlinkStreamCollision(Mavlink *mavlink) : MavlinkStream(mavlink)
	{}

	bool send() override
	{
		collision_report_s report;
		bool sent = false;

		while (_collision_sub.update(&report)) {
			mavlink_collision_t msg = {};

			msg.src = report.src;
			msg.id = report.id;
			msg.action = report.action;
			msg.threat_level = report.threat_level;
			msg.time_to_minimum_delta = report.time_to_minimum_delta;
			msg.altitude_minimum_delta = report.altitude_minimum_delta;
			msg.horizontal_minimum_delta = report.horizontal_minimum_delta;

			mavlink_msg_collision_send_struct(_mavlink->get_channel(), &msg);
			sent = true;
		}

		return sent;
	}
};

#endif // COLLISION_HPP
