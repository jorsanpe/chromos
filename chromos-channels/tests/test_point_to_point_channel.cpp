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
#include <chromos/channels/ConsumerChannel.h>

using namespace cest;


describe("PointToPointChannel", []() {
    it("creates the producer/consumers channels and allows sending messages", []() {
        ConsumerChannel consumer("tcp://127.0.0.1:9876");
        ProducerChannel sender("tcp://127.0.0.1:9876");

        sender.send("Message Payload");

        expect(consumer.receive()).toBe("Message Payload");
    });

    it("can send message payloads of up to 1 MB", []() {
        ConsumerChannel consumer("tcp://127.0.0.1:9876");
        ProducerChannel producer("tcp://127.0.0.1:9876");
        std::string payload(1024*1024, 'A');

        producer.send(payload);

        expect(consumer.receive()).toBe(payload);
    });

    it("allows multiple producers and one consumer on a channel", []() {
        ConsumerChannel consumer("tcp://127.0.0.1:9876");
        ProducerChannel producer1("tcp://127.0.0.1:9876");
        ProducerChannel producer2("tcp://127.0.0.1:9876");

        producer1.send("Producer Payload");
        producer2.send("Producer Payload");

        expect(consumer.receive()).toBe("Producer Payload");
        expect(consumer.receive()).toBe("Producer Payload");
    });
});
