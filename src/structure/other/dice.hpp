#ifndef CPPLIB_SRC_STRUCTURE_OTHER_DICE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_DICE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>

enum class DiceFace : unsigned char{
    top,
    front,
    right,
    back,
    left,
    bottom,
};

template<class T>
class Dice{
public:
    using value_type = T;
    using labels_type = std::array<value_type, 6>;
    static constexpr std::size_t orientation_count = 24;

private:
    using permutation_type = std::array<unsigned char, 6>;

    struct OrientationTable{
        std::array<permutation_type, orientation_count> values{};
        std::size_t count = 0;
    };

    labels_type labels_;

    static constexpr std::size_t face_index(DiceFace face){
        const auto index = static_cast<unsigned int>(face);
        if(index >= 6)[[unlikely]]{
            throw std::out_of_range("invalid DiceFace");
        }
        return index;
    }

    static constexpr permutation_type compose(
        const permutation_type& current,
        const permutation_type& operation
    ) noexcept{
        permutation_type result{};
        for(std::size_t position = 0; position < 6; ++position){
            result[position] = current[operation[position]];
        }
        return result;
    }

    static constexpr bool contains(
        const OrientationTable& table,
        const permutation_type& permutation
    ) noexcept{
        for(std::size_t index = 0; index < table.count; ++index){
            if(table.values[index] == permutation) return true;
        }
        return false;
    }

    static constexpr OrientationTable make_orientation_table() noexcept{
        constexpr permutation_type roll_front_permutation{3, 0, 2, 5, 4, 1};
        constexpr permutation_type roll_right_permutation{4, 1, 0, 3, 5, 2};
        constexpr permutation_type spin_right_permutation{0, 2, 3, 4, 1, 5};
        constexpr std::array<permutation_type, 3> generators{
            roll_front_permutation,
            roll_right_permutation,
            spin_right_permutation,
        };

        OrientationTable table;
        table.values[0] = {0, 1, 2, 3, 4, 5};
        table.count = 1;
        for(std::size_t head = 0; head < table.count; ++head){
            for(const auto& generator: generators){
                const auto next = compose(table.values[head], generator);
                if(!contains(table, next)){
                    table.values[table.count++] = next;
                }
            }
        }
        return table;
    }

    inline static constexpr OrientationTable orientation_table_ =
        make_orientation_table();

    constexpr Dice permuted(const permutation_type& permutation) const{
        return Dice(labels_type{
            labels_[permutation[0]],
            labels_[permutation[1]],
            labels_[permutation[2]],
            labels_[permutation[3]],
            labels_[permutation[4]],
            labels_[permutation[5]],
        });
    }

    template<std::size_t... Indices>
    constexpr std::array<Dice, orientation_count> make_orientations(
        std::index_sequence<Indices...>
    ) const{
        return {permuted(orientation_table_.values[Indices])...};
    }

    constexpr Dice& apply(const permutation_type& permutation){
        *this = permuted(permutation);
        return *this;
    }

public:
    static_assert(
        orientation_table_.count == orientation_count,
        "cube rotations must contain exactly 24 orientations"
    );

    constexpr explicit Dice(labels_type labels)
        : labels_(std::move(labels)){}

    constexpr Dice(
        T top,
        T front,
        T right,
        T back,
        T left,
        T bottom
    )
        : labels_{
            std::move(top),
            std::move(front),
            std::move(right),
            std::move(back),
            std::move(left),
            std::move(bottom),
        }{}

    constexpr const labels_type& labels() const noexcept{
        return labels_;
    }

    constexpr const T& label(DiceFace face) const{
        return labels_[face_index(face)];
    }

    constexpr const T& top() const noexcept{ return labels_[0]; }
    constexpr const T& front() const noexcept{ return labels_[1]; }
    constexpr const T& right() const noexcept{ return labels_[2]; }
    constexpr const T& back() const noexcept{ return labels_[3]; }
    constexpr const T& left() const noexcept{ return labels_[4]; }
    constexpr const T& bottom() const noexcept{ return labels_[5]; }

    constexpr Dice& roll_front(){
        return apply({3, 0, 2, 5, 4, 1});
    }

    constexpr Dice& roll_back(){
        return apply({1, 5, 2, 0, 4, 3});
    }

    constexpr Dice& roll_right(){
        return apply({4, 1, 0, 3, 5, 2});
    }

    constexpr Dice& roll_left(){
        return apply({2, 1, 5, 3, 0, 4});
    }

    constexpr Dice& spin_right(){
        return apply({0, 2, 3, 4, 1, 5});
    }

    constexpr Dice& spin_left(){
        return apply({0, 4, 1, 2, 3, 5});
    }

    constexpr std::array<Dice, orientation_count> orientations() const{
        return make_orientations(std::make_index_sequence<orientation_count>{});
    }

    constexpr Dice oriented(DiceFace top_face, DiceFace front_face) const{
        const std::size_t top_index = face_index(top_face);
        const std::size_t front_index = face_index(front_face);
        for(const auto& permutation: orientation_table_.values){
            if(permutation[0] == top_index && permutation[1] == front_index){
                return permuted(permutation);
            }
        }
        throw std::invalid_argument(
            "Dice top and front faces must be distinct adjacent faces"
        );
    }

    constexpr std::optional<Dice> oriented_by_labels(
        const T& top_label,
        const T& front_label
    ) const{
        for(const auto& permutation: orientation_table_.values){
            if(labels_[permutation[0]] == top_label
               && labels_[permutation[1]] == front_label){
                return permuted(permutation);
            }
        }
        return std::nullopt;
    }

    constexpr bool rotationally_equivalent(const Dice& other) const{
        for(const auto& permutation: orientation_table_.values){
            if(permuted(permutation).labels_ == other.labels_) return true;
        }
        return false;
    }

    friend constexpr bool operator==(
        const Dice& left,
        const Dice& right
    ){
        return left.labels_ == right.labels_;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_DICE_HPP_INCLUDED
