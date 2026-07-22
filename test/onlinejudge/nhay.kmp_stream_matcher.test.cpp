// competitive-verifier: PROBLEM https://www.spoj.com/problems/NHAY/

#include <iostream>
#include <limits>
#include <string>
#include <utility>

#include "../../src/algorithm/string/automata/kmp_stream_matcher.hpp"

template<class Consumer>
void consume_line(Consumer&& consume){
    char c;
    while(std::cin.get(c)){
        if(c == '\n') break;
        if(c == '\r' && std::cin.peek() == '\n'){
            std::cin.get();
            break;
        }
        consume(c);
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int declared_length;
    bool first_case = true;
    while(std::cin >> declared_length){
        (void)declared_length;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string pattern;
        std::getline(std::cin, pattern);
        if(!pattern.empty() && pattern.back() == '\r') pattern.pop_back();

        if(!first_case) std::cout << '\n';
        first_case = false;

        if(pattern.empty()){
            long long position = 0;
            std::cout << position << '\n';
            consume_line([&](char){
                std::cout << ++position << '\n';
            });
            continue;
        }

        KmpStreamMatcher matcher(std::move(pattern));
        consume_line([&](char c){
            const long long position = matcher.feed(c);
            if(position != -1) std::cout << position << '\n';
        });
    }
}
