#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>

template<class T, std::size_t MAX_SIZE, bool Minimum = true>
class KineticTournament{
    static_assert(std::is_integral_v<T> && std::is_signed_v<T>,
        "KineticTournament requires a signed integral type");
#if defined(__SIZEOF_INT128__)
    __extension__ using Wide = __int128;
    using UnsignedWide = __uint128_t;
#else
    using Wide = long long;
    using UnsignedWide = unsigned long long;
    static_assert(sizeof(T) < sizeof(long long),
        "this compiler needs a wider integer type");
#endif

public:
    struct Line{
        T slope{};
        T intercept{};
    };

    struct Result{
        T value{};
        Line line{};
        std::size_t index{};
    };

private:
    static constexpr std::size_t TREE_SIZE = 4 * MAX_SIZE + 1;

    struct State{
        std::array<Line, MAX_SIZE> lines{};
        std::array<unsigned char, MAX_SIZE> active{};
        std::array<int, TREE_SIZE> winner{};
        std::array<T, TREE_SIZE> event_time{};
        std::array<unsigned char, TREE_SIZE> has_event{};
    };

    std::size_t size_ = 0;
    T time_{};
    std::unique_ptr<State> state_;

    [[nodiscard]] Wide evaluate_wide(int index) const{
        const auto& line = state_->lines[static_cast<std::size_t>(index)];
        return static_cast<Wide>(line.slope) * static_cast<Wide>(time_)
            + static_cast<Wide>(line.intercept);
    }

    [[nodiscard]] T evaluate_checked(int index) const{
        const Wide value = evaluate_wide(index);
        if(value < static_cast<Wide>(std::numeric_limits<T>::lowest())
            || value > static_cast<Wide>(std::numeric_limits<T>::max())){
            throw std::overflow_error("kinetic line evaluation overflow");
        }
        return static_cast<T>(value);
    }

    [[nodiscard]] bool better(int left, int right) const{
        if(left == -1) return false;
        if(right == -1) return true;
        const Wide left_value = evaluate_wide(left);
        const Wide right_value = evaluate_wide(right);
        if(left_value != right_value){
            if constexpr(Minimum) return left_value < right_value;
            else return left_value > right_value;
        }
        const T left_slope = state_->lines[static_cast<std::size_t>(left)].slope;
        const T right_slope = state_->lines[static_cast<std::size_t>(right)].slope;
        if(left_slope != right_slope){
            if constexpr(Minimum) return left_slope < right_slope;
            else return left_slope > right_slope;
        }
        return left < right;
    }

    void take_event(std::size_t node, T event){
        if(!state_->has_event[node] || event < state_->event_time[node]){
            state_->event_time[node] = event;
            state_->has_event[node] = 1;
        }
    }

    void combine(std::size_t node){
        const std::size_t left_node = node * 2;
        const std::size_t right_node = left_node + 1;
        if(right_node >= TREE_SIZE){
            throw std::logic_error("kinetic tournament tree capacity");
        }
        const int left = state_->winner[left_node];
        const int right = state_->winner[right_node];
        const int winner = better(left, right) ? left : right;
        const int loser = winner == left ? right : left;
        state_->winner[node] = winner;
        state_->has_event[node] = 0;
        if(state_->has_event[left_node]){
            take_event(node, state_->event_time[left_node]);
        }
        if(state_->has_event[right_node]){
            take_event(node, state_->event_time[right_node]);
        }
        if(winner == -1 || loser == -1) return;

        const Wide winner_slope = static_cast<Wide>(
            state_->lines[static_cast<std::size_t>(winner)].slope
        );
        const Wide loser_slope = static_cast<Wide>(
            state_->lines[static_cast<std::size_t>(loser)].slope
        );
        Wide slope_difference;
        Wide high_value;
        Wide low_value;
        if constexpr(Minimum){
            if(winner_slope <= loser_slope) return;
            slope_difference = winner_slope - loser_slope;
            high_value = evaluate_wide(loser);
            low_value = evaluate_wide(winner);
        }else{
            if(winner_slope >= loser_slope) return;
            slope_difference = loser_slope - winner_slope;
            high_value = evaluate_wide(winner);
            low_value = evaluate_wide(loser);
        }
        if(high_value < low_value){
            throw std::logic_error("invalid kinetic tournament certificate");
        }
        const UnsignedWide value_difference =
            static_cast<UnsignedWide>(high_value)
            - static_cast<UnsignedWide>(low_value);
        const UnsignedWide divisor =
            static_cast<UnsignedWide>(slope_difference);
        const UnsignedWide step = value_difference / divisor
            + static_cast<UnsignedWide>(value_difference % divisor != 0);
        const Wide remaining_time =
            static_cast<Wide>(std::numeric_limits<T>::max())
            - static_cast<Wide>(time_);
        if(step == 0 || step > static_cast<UnsignedWide>(remaining_time)){
            return;
        }
        const Wide event =
            static_cast<Wide>(time_) + static_cast<Wide>(step);
        take_event(node, static_cast<T>(event));
    }

    void set_impl(
        std::size_t index,
        std::size_t left,
        std::size_t right,
        std::size_t node
    ){
        if(node >= TREE_SIZE){
            throw std::logic_error("kinetic tournament tree capacity");
        }
        if(right <= left + 1){
            state_->winner[node] = state_->active[index]
                ? static_cast<int>(index)
                : -1;
            state_->has_event[node] = 0;
            return;
        }
        const std::size_t middle = left + (right - left) / 2;
        if(index < middle) set_impl(index, left, middle, node * 2);
        else set_impl(index, middle, right, node * 2 + 1);
        combine(node);
    }

    void repair(
        std::size_t left,
        std::size_t right,
        std::size_t node
    ){
        if(node >= TREE_SIZE){
            throw std::logic_error("kinetic tournament tree capacity");
        }
        if(!state_->has_event[node] || time_ < state_->event_time[node]) return;
        if(right <= left + 1){
            state_->has_event[node] = 0;
            return;
        }
        const std::size_t middle = left + (right - left) / 2;
        repair(left, middle, node * 2);
        repair(middle, right, node * 2 + 1);
        combine(node);
    }

    [[nodiscard]] int query_impl(
        std::size_t query_left,
        std::size_t query_right,
        std::size_t left,
        std::size_t right,
        std::size_t node
    ) const{
        if(node >= TREE_SIZE){
            throw std::logic_error("kinetic tournament tree capacity");
        }
        if(query_right <= left || right <= query_left) return -1;
        if(query_left <= left && right <= query_right){
            return state_->winner[node];
        }
        const std::size_t middle = left + (right - left) / 2;
        const int first = query_impl(
            query_left, query_right, left, middle, node * 2
        );
        const int second = query_impl(
            query_left, query_right, middle, right, node * 2 + 1
        );
        return better(first, second) ? first : second;
    }

    void check_index(std::size_t index) const{
        if(index >= size_) throw std::out_of_range("kinetic tournament index");
    }

    void check_range(std::size_t left, std::size_t right) const{
        if(left > right || right > size_){
            throw std::out_of_range("kinetic tournament range");
        }
    }

public:
    explicit KineticTournament(std::size_t size = MAX_SIZE, T initial_time = T{}):
        size_(size), time_(initial_time), state_(std::make_unique<State>()){
        if(size > MAX_SIZE) throw std::length_error("kinetic tournament capacity");
        state_->winner.fill(-1);
    }

    KineticTournament(const KineticTournament&) = delete;
    KineticTournament& operator=(const KineticTournament&) = delete;
    KineticTournament(KineticTournament&&) noexcept = default;
    KineticTournament& operator=(KineticTournament&&) noexcept = default;

    [[nodiscard]] std::size_t size() const noexcept{
        return size_;
    }

    [[nodiscard]] T current_time() const noexcept{
        return time_;
    }

    [[nodiscard]] bool active(std::size_t index) const{
        check_index(index);
        return state_->active[index] != 0;
    }

    [[nodiscard]] std::optional<Line> line(std::size_t index) const{
        check_index(index);
        if(!state_->active[index]) return std::nullopt;
        return state_->lines[index];
    }

    void set(std::size_t index, T slope, T intercept){
        check_index(index);
        state_->lines[index] = Line{slope, intercept};
        state_->active[index] = 1;
        set_impl(index, 0, size_, 1);
    }

    void erase(std::size_t index){
        check_index(index);
        if(!state_->active[index]) return;
        state_->active[index] = 0;
        set_impl(index, 0, size_, 1);
    }

    void advance_time(T new_time){
        if(new_time < time_){
            throw std::invalid_argument("kinetic tournament time must not decrease");
        }
        time_ = new_time;
        if(size_ != 0) repair(0, size_, 1);
    }

    [[nodiscard]] std::optional<Result> query(
        std::size_t left,
        std::size_t right
    ) const{
        check_range(left, right);
        if(left == right) return std::nullopt;
        const int winner = query_impl(left, right, 0, size_, 1);
        if(winner == -1) return std::nullopt;
        const std::size_t index = static_cast<std::size_t>(winner);
        return Result{evaluate_checked(winner), state_->lines[index], index};
    }

    [[nodiscard]] std::optional<Result> best() const{
        return query(0, size_);
    }
};
