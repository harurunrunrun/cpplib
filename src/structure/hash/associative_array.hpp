#ifndef CPPLIB_SRC_STRUCTURE_HASH_ASSOCIATIVE_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HASH_ASSOCIATIVE_ARRAY_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <array>

template<
    class Key,
    class T,
    int MAX_SIZE,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>
>
class AssociativeArray{
    static_assert(MAX_SIZE > 0);

    static constexpr std::size_t table_capacity(){
        std::size_t capacity = 1;
        const std::size_t required = static_cast<std::size_t>(MAX_SIZE) * 2;
        while(capacity < required) capacity <<= 1;
        return capacity;
    }

    static constexpr std::size_t CAPACITY = table_capacity();
    static constexpr std::size_t MASK = CAPACITY - 1;

    struct Entry{
        Key key;
        T value;
    };

    struct Slot{
        std::optional<Entry> entry;
        std::size_t distance = 0;
    };

    struct State{
        std::array<Slot, CAPACITY> slots;
    };

    int size_ = 0;
    std::uint64_t seed_;
    Hash hash_;
    KeyEqual equal_;
    std::unique_ptr<State> state_;

    static std::uint64_t mix(std::uint64_t value){
        value += 0x9e3779b97f4a7c15ULL;
        value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
        value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }

    std::size_t bucket(const Key& key) const{
        return static_cast<std::size_t>(
            mix(static_cast<std::uint64_t>(hash_(key)) + seed_)
        ) & MASK;
    }

    std::size_t find_index(const Key& key) const{
        std::size_t index = bucket(key);
        std::size_t distance = 0;
        while(true){
            const Slot& slot = state_->slots[index];
            if(!slot.entry || slot.distance < distance) return CAPACITY;
            if(equal_(slot.entry->key, key)) return index;
            index = (index + 1) & MASK;
            ++distance;
        }
    }

    void insert_new(Key key, T value){
        if(size_ == MAX_SIZE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (AssociativeArray)."
            );
        }

        std::optional<Entry> incoming(
            std::in_place,
            Entry{std::move(key), std::move(value)}
        );
        std::size_t index = bucket(incoming->key);
        std::size_t distance = 0;
        while(true){
            Slot& slot = state_->slots[index];
            if(!slot.entry){
                slot.entry = std::move(incoming);
                slot.distance = distance;
                ++size_;
                return;
            }
            if(slot.distance < distance){
                std::swap(slot.entry, incoming);
                std::swap(slot.distance, distance);
            }
            index = (index + 1) & MASK;
            ++distance;
        }
    }

public:
    explicit AssociativeArray(
        std::uint64_t seed = 0x243f6a8885a308d3ULL,
        Hash hash = Hash(),
        KeyEqual equal = KeyEqual()
    ):
        seed_(seed),
        hash_(std::move(hash)),
        equal_(std::move(equal)),
        state_(std::make_unique<State>()){}

    AssociativeArray(const AssociativeArray&) = delete;
    AssociativeArray& operator=(const AssociativeArray&) = delete;
    AssociativeArray(AssociativeArray&&) noexcept = default;
    AssociativeArray& operator=(AssociativeArray&&) noexcept = default;

    int size() const noexcept{ return size_; }
    bool empty() const noexcept{ return size_ == 0; }
    static constexpr int max_size() noexcept{ return MAX_SIZE; }
    static constexpr std::size_t capacity() noexcept{ return CAPACITY; }

    T* find(const Key& key){
        const std::size_t index = find_index(key);
        return index == CAPACITY ? nullptr : &state_->slots[index].entry->value;
    }

    const T* find(const Key& key) const{
        const std::size_t index = find_index(key);
        return index == CAPACITY ? nullptr : &state_->slots[index].entry->value;
    }

    bool contains(const Key& key) const{ return find_index(key) != CAPACITY; }

    T& at(const Key& key){
        T* value = find(key);
        if(value == nullptr)[[unlikely]] throw std::out_of_range("AssociativeArray::at");
        return *value;
    }

    const T& at(const Key& key) const{
        const T* value = find(key);
        if(value == nullptr)[[unlikely]] throw std::out_of_range("AssociativeArray::at");
        return *value;
    }

    bool insert_or_assign(Key key, T value){
        const std::size_t index = find_index(key);
        if(index != CAPACITY){
            state_->slots[index].entry->value = std::move(value);
            return false;
        }
        insert_new(std::move(key), std::move(value));
        return true;
    }

    T& operator[](const Key& key){
        if(T* value = find(key); value != nullptr) return *value;
        insert_new(key, T());
        return *find(key);
    }

    T& operator[](Key&& key){
        if(T* value = find(key); value != nullptr) return *value;
        Key lookup = key;
        insert_new(std::move(key), T());
        return *find(lookup);
    }

    bool erase(const Key& key){
        std::size_t index = find_index(key);
        if(index == CAPACITY) return false;

        std::size_t next = (index + 1) & MASK;
        while(state_->slots[next].entry && state_->slots[next].distance > 0){
            state_->slots[index].entry = std::move(state_->slots[next].entry);
            state_->slots[index].distance = state_->slots[next].distance - 1;
            index = next;
            next = (next + 1) & MASK;
        }
        state_->slots[index].entry.reset();
        state_->slots[index].distance = 0;
        --size_;
        return true;
    }

    void clear(){
        for(Slot& slot: state_->slots){
            slot.entry.reset();
            slot.distance = 0;
        }
        size_ = 0;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HASH_ASSOCIATIVE_ARRAY_HPP_INCLUDED
