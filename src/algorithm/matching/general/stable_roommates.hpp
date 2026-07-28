#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_STABLE_ROOMMATES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_STABLE_ROOMMATES_HPP_INCLUDED

#include <cstddef>
#include <list>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

inline std::optional<std::vector<int>> stable_roommates(
    const std::vector<std::vector<int>>& preferences
){
    const int count = static_cast<int>(preferences.size());
    if((count & 1) != 0) return std::nullopt;
    if(count == 0) return std::vector<int>{};
    std::vector<std::vector<int>> rank(
        static_cast<std::size_t>(count),
        std::vector<int>(static_cast<std::size_t>(count), -1)
    );
    for(int person = 0; person < count; ++person){
        const auto& row = preferences[static_cast<std::size_t>(person)];
        if(static_cast<int>(row.size()) != count - 1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: incomplete preference list "
                "(stable_roommates)."
            );
        }
        for(int position = 0; position < count - 1; ++position){
            const int other = row[static_cast<std::size_t>(position)];
            if(other < 0 || count <= other || other == person
                || rank[static_cast<std::size_t>(person)]
                    [static_cast<std::size_t>(other)] != -1)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid preference list "
                    "(stable_roommates)."
                );
            }
            rank[static_cast<std::size_t>(person)]
                [static_cast<std::size_t>(other)] = position;
        }
    }

    using Iterator = std::list<int>::iterator;
    std::vector<std::list<int>> lists(static_cast<std::size_t>(count));
    std::vector<std::vector<Iterator>> iterator(
        static_cast<std::size_t>(count),
        std::vector<Iterator>(static_cast<std::size_t>(count))
    );
    std::vector<std::vector<unsigned char>> active(
        static_cast<std::size_t>(count),
        std::vector<unsigned char>(static_cast<std::size_t>(count), 0)
    );
    for(int person = 0; person < count; ++person){
        auto& list = lists[static_cast<std::size_t>(person)];
        for(int other: preferences[static_cast<std::size_t>(person)]){
            list.push_back(other);
            auto position = list.end();
            --position;
            iterator[static_cast<std::size_t>(person)]
                [static_cast<std::size_t>(other)] = position;
            active[static_cast<std::size_t>(person)]
                [static_cast<std::size_t>(other)] = 1;
        }
    }
    auto erase_pair = [&](int first, int second){
        if(active[static_cast<std::size_t>(first)]
            [static_cast<std::size_t>(second)] == 0) return;
        lists[static_cast<std::size_t>(first)].erase(
            iterator[static_cast<std::size_t>(first)]
                [static_cast<std::size_t>(second)]
        );
        lists[static_cast<std::size_t>(second)].erase(
            iterator[static_cast<std::size_t>(second)]
                [static_cast<std::size_t>(first)]
        );
        active[static_cast<std::size_t>(first)]
            [static_cast<std::size_t>(second)] = 0;
        active[static_cast<std::size_t>(second)]
            [static_cast<std::size_t>(first)] = 0;
    };

    std::vector<int> holder(static_cast<std::size_t>(count), -1);
    std::queue<int> free;
    for(int person = 0; person < count; ++person) free.push(person);
    while(!free.empty()){
        const int proposer = free.front();
        free.pop();
        while(true){
            if(lists[static_cast<std::size_t>(proposer)].empty()){
                return std::nullopt;
            }
            const int recipient =
                lists[static_cast<std::size_t>(proposer)].front();
            const int previous =
                holder[static_cast<std::size_t>(recipient)];
            if(previous == -1){
                holder[static_cast<std::size_t>(recipient)] = proposer;
                break;
            }
            if(rank[static_cast<std::size_t>(recipient)]
                    [static_cast<std::size_t>(proposer)]
                < rank[static_cast<std::size_t>(recipient)]
                    [static_cast<std::size_t>(previous)]){
                erase_pair(previous, recipient);
                holder[static_cast<std::size_t>(recipient)] = proposer;
                free.push(previous);
                break;
            }
            erase_pair(proposer, recipient);
        }
    }
    for(int recipient = 0; recipient < count; ++recipient){
        const int proposal = holder[static_cast<std::size_t>(recipient)];
        auto position = iterator[static_cast<std::size_t>(recipient)]
            [static_cast<std::size_t>(proposal)];
        ++position;
        while(position != lists[static_cast<std::size_t>(recipient)].end()){
            const int rejected = *position;
            ++position;
            erase_pair(recipient, rejected);
        }
    }

    while(true){
        int start_person = -1;
        for(int person = 0; person < count; ++person){
            if(lists[static_cast<std::size_t>(person)].empty()){
                return std::nullopt;
            }
            if(lists[static_cast<std::size_t>(person)].size() > 1){
                start_person = person;
                break;
            }
        }
        if(start_person == -1) break;
        std::vector<int> position(static_cast<std::size_t>(count), -1);
        std::vector<int> people;
        std::vector<int> second_choices;
        int person = start_person;
        while(position[static_cast<std::size_t>(person)] == -1){
            position[static_cast<std::size_t>(person)] =
                static_cast<int>(people.size());
            people.push_back(person);
            const auto& list = lists[static_cast<std::size_t>(person)];
            if(list.size() < 2) return std::nullopt;
            auto choice = list.begin();
            ++choice;
            const int second = *choice;
            second_choices.push_back(second);
            if(lists[static_cast<std::size_t>(second)].empty()){
                return std::nullopt;
            }
            person = lists[static_cast<std::size_t>(second)].back();
        }
        const int cycle_start =
            position[static_cast<std::size_t>(person)];
        const int cycle_end = static_cast<int>(people.size());
        std::vector<std::pair<int, int>> deletions;
        for(int index = cycle_start; index < cycle_end; ++index){
            const int proposer = people[static_cast<std::size_t>(index)];
            const int recipient =
                second_choices[static_cast<std::size_t>(index)];
            auto rejected = iterator[static_cast<std::size_t>(recipient)]
                [static_cast<std::size_t>(proposer)];
            ++rejected;
            while(rejected
                != lists[static_cast<std::size_t>(recipient)].end()){
                const int other = *rejected;
                ++rejected;
                deletions.emplace_back(recipient, other);
            }
        }
        for(auto [first, second]: deletions){
            erase_pair(first, second);
        }
    }

    std::vector<int> match(static_cast<std::size_t>(count), -1);
    for(int person = 0; person < count; ++person){
        if(lists[static_cast<std::size_t>(person)].size() != 1){
            return std::nullopt;
        }
        match[static_cast<std::size_t>(person)] =
            lists[static_cast<std::size_t>(person)].front();
    }
    for(int person = 0; person < count; ++person){
        const int other = match[static_cast<std::size_t>(person)];
        if(other < 0 || count <= other
            || match[static_cast<std::size_t>(other)] != person){
            return std::nullopt;
        }
    }
    return match;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_STABLE_ROOMMATES_HPP_INCLUDED
