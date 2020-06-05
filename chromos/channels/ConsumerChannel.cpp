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
#include <chromos/channels/ConsumerChannel.h>

using namespace std;


ConsumerChannel::ConsumerChannel(string address)
{
    context = zmq_ctx_new();
    socket = zmq_socket(context, ZMQ_PULL);
    zmq_bind(socket, address.c_str());
}


string ConsumerChannel::receive()
{
    char buffer[1024*1024];
    int length;

    length = zmq_recv(socket, buffer, sizeof(buffer), 0);

    return move(string(buffer, length));
}


ConsumerChannel::~ConsumerChannel()
{
    zmq_close(socket);
    zmq_ctx_term(context);
}
