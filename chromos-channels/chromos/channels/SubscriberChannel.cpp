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
#include <zeromq/zmq.h>
#include <chromos/channels/SubscriberChannel.h>

using namespace std;


SubscriberChannel::SubscriberChannel(string address)
{
    context = zmq_ctx_new();
    socket = zmq_socket(context, ZMQ_PULL);
    zmq_bind(socket, address.c_str());
}


SubscriberNotification SubscriberChannel::receive()
{
    char envelope[1024*1024];
    int envelope_length;
    char payload[1024*1024];
    int payload_length;

    envelope_length = zmq_recv(socket, envelope, sizeof(envelope), 0);
    payload_length = zmq_recv(socket, payload, sizeof(payload), 0);

    return SubscriberNotification{string(envelope, envelope_length), string(payload, payload_length)};
}


SubscriberChannel::~SubscriberChannel()
{
    zmq_close(socket);
    zmq_ctx_term(context);
}


void SubscriberChannel::subscribe(std::string envelope)
{
    zmq_setsockopt(socket, ZMQ_SUBSCRIBE, envelope.c_str(), envelope.size());
}

