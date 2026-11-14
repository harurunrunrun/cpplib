// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <vector>

#include "../../src/algorithm/range/offline_query/hilbert_mo.hpp"
#include "../../src/algorithm/range/offline_query/mo_with_modifications.hpp"
#include "../../src/algorithm/range/offline_query/rollback_mo.hpp"

int main(){
    const std::vector<int> sequence{1, 2, 3, 4};
    {
        HilbertMo mo(4);
        mo.add_query(0, 3);
        mo.add_query(1, 4);
        int sum = 0;
        std::vector<int> answers(2);
        mo.run([&](int p){ sum += sequence[p]; }, [&](int p){ sum += sequence[p]; },
               [&](int p){ sum -= sequence[p]; }, [&](int p){ sum -= sequence[p]; },
               [&](int id){ answers[id] = sum; });
        assert((answers == std::vector<int>{6, 9}));
    }
    {
        // n is a power of two: queries with right == n must stay inside the
        // Hilbert grid, otherwise the order degrades (281 moves before the fix).
        const int n = 16;
        HilbertMo mo(n);
        for(int left = 0; left <= n; ++left){
            for(int right = left; right <= n; ++right) mo.add_query(left, right);
        }
        long long moves = 0;
        int answered = 0;
        auto step = [&](int){ ++moves; };
        mo.run(step, step, step, step, [&](int){ ++answered; });
        assert(answered == (n + 1) * (n + 2) / 2);
        assert(moves <= 200);
    }
    {
        RollbackMo mo(4);
        mo.add_query(0, 3);
        mo.add_query(1, 4);
        int sum = 0;
        std::vector<int> inserted, answers(2);
        mo.run([&](int p){ sum += sequence[p]; inserted.push_back(p); },
               [&]{ return inserted.size(); },
               [&](std::size_t size){
                   while(inserted.size() > size){
                       sum -= sequence[inserted.back()];
                       inserted.pop_back();
                   }
               },
               [&](int id){ answers[id] = sum; });
        assert((answers == std::vector<int>{6, 9}));
    }
    {
        MoWithModifications mo(3);
        mo.add_query(0, 3, 0);
        mo.add_query(0, 3, 1);
        std::vector<int> values{1, 2, 3}, answers(2);
        int sum = 0;
        auto update = [&](int, int left, int right){
            if(left <= 1 && 1 < right) sum -= values[1];
            values[1] = values[1] == 2 ? 5 : 2;
            if(left <= 1 && 1 < right) sum += values[1];
        };
        mo.run([&](int p){ sum += values[p]; }, [&](int p){ sum += values[p]; },
               [&](int p){ sum -= values[p]; }, [&](int p){ sum -= values[p]; },
               update, update, [&](int id){ answers[id] = sum; });
        assert((answers == std::vector<int>{6, 9}));
    }
}
