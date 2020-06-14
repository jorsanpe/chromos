/*
 * Copyright (C) 2020  Jordi Sánchez
 * This file is part of Chromos
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <chromos/channels/PublisherChannel.h>
#include <zeromq/zmq.h>

using namespace std;


PublisherChannel::PublisherChannel(string address)
{
    context = zmq_ctx_new();
    socket = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(socket, address.c_str());
}


void PublisherChannel::publish(string envelope, string payload)
{
    zmq_send(socket, envelope.c_str(), envelope.size(), ZMQ_SNDMORE);
    zmq_send(socket, payload.c_str(), payload.size(), 0);
}


PublisherChannel::~PublisherChannel()
{
    zmq_close(socket);
    zmq_ctx_term(context);
}
