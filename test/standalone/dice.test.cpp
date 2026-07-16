// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>

#include "../../src/structure/other/dice.hpp"

namespace{

using IntDice = Dice<int>;

void print(const IntDice& dice){
    const auto& labels = dice.labels();
    for(std::size_t index = 0; index < labels.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << labels[index];
    }
    std::cout << '\n';
}

void self_test(){
    const IntDice base(1, 2, 3, 4, 5, 6);
    IntDice dice = base;
    dice.roll_front();
    assert((dice.labels() == std::array<int, 6>{4, 1, 3, 6, 5, 2}));
    dice.roll_back();
    assert(dice == base);
    dice.roll_right().roll_left().spin_right().spin_left();
    assert(dice == base);

    std::set<std::array<int, 6>> distinct;
    for(const auto& orientation: base.orientations()){
        distinct.insert(orientation.labels());
        assert(orientation.rotationally_equivalent(base));
    }
    assert(distinct.size() == IntDice::orientation_count);

    for(int top = 0; top < 6; ++top){
        for(int front = 0; front < 6; ++front){
            constexpr std::array<int, 6> opposite{5, 3, 4, 1, 2, 0};
            const bool adjacent = top != front && opposite[top] != front;
            bool thrown = false;
            try{
                const auto oriented = base.oriented(
                    static_cast<DiceFace>(top),
                    static_cast<DiceFace>(front)
                );
                assert(adjacent);
                assert(oriented.top() == base.label(static_cast<DiceFace>(top)));
                assert(oriented.front() == base.label(static_cast<DiceFace>(front)));
            }catch(const std::invalid_argument&){
                thrown = true;
            }
            assert(thrown != adjacent);
        }
    }

    const auto by_label = base.oriented_by_labels(6, 4);
    assert(by_label.has_value());
    assert(by_label->top() == 6 && by_label->front() == 4);
    assert(!base.oriented_by_labels(10, 1).has_value());

    IntDice rotated = base;
    rotated.roll_front().spin_right();
    assert(base.rotationally_equivalent(rotated));
    assert(!base.rotationally_equivalent(IntDice(1, 2, 3, 4, 6, 5)));

    const IntDice repeated(1, 1, 2, 2, 3, 3);
    assert(repeated.orientations().size() == 24);

    bool thrown = false;
    try{
        (void)base.label(static_cast<DiceFace>(6));
    }catch(const std::out_of_range&){
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    std::array<int, 6> labels{};
    if(!(std::cin >> query_count)){
        self_test();
        return 0;
    }
    for(int& value: labels) std::cin >> value;
    IntDice dice(labels);

    while(query_count-- != 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "RF") dice.roll_front();
        else if(operation == "RB") dice.roll_back();
        else if(operation == "RR") dice.roll_right();
        else if(operation == "RL") dice.roll_left();
        else if(operation == "SR") dice.spin_right();
        else if(operation == "SL") dice.spin_left();
        else if(operation == "STATE") print(dice);
        else if(operation == "ORIENT"){
            int top, front;
            std::cin >> top >> front;
            dice = dice.oriented(
                static_cast<DiceFace>(top),
                static_cast<DiceFace>(front)
            );
        }else if(operation == "EQUIV"){
            std::array<int, 6> other_labels{};
            for(int& value: other_labels) std::cin >> value;
            std::cout << dice.rotationally_equivalent(IntDice(other_labels))
                      << '\n';
        }
    }
}
