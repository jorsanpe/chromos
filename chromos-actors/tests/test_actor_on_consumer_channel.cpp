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
#include <cest/cest.h>

#include <chromos/channels/ProducerChannel.h>
#include <chromos/actors/ActorOnConsumerChannel.h>

using namespace cest;

class TestActor: public ActorOnConsumerChannel {
public:
    std::string message_received;

    TestActor(ConsumerChannel *channel): ActorOnConsumerChannel(channel) {
        message_received = "";
    }

protected:
    void execute(std::string message) {
        message_received = message;
    }
};


describe("ActorOnConsumerChannel", []() {
    it("creates an actor and executes function upon reception of message", []() {
        ConsumerChannel consumer_channel("tcp://127.0.0.1:9876");
        ProducerChannel producer_channel("tcp://127.0.0.1:9876");
        TestActor *actor = new TestActor(&consumer_channel);

        producer_channel.send("Hello");
        actor->join();

        expect(actor->message_received).toBe("Hello");
    });
});
