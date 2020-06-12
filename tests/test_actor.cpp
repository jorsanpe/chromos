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
#include <unistd.h>
#include <iostream>
#include <cest/cest.h>

#include <chromos/actors/Actor.h>

using namespace cest;


class TestActor: public Actor {
public:
    int test_value;

    TestActor() {
        this->test_value = 0;
    }
    
protected:
    void execute() {
        test_value++;
    }
};


describe("Actor", []() {
    it("creates an Actor and waits for it to finish", []() {
        TestActor *actor = new TestActor();
        actor->join();
        expect(actor->test_value).toBe(1);
    });
});
