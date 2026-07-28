#ifndef CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_DETAIL_STANDARD_FINGER_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_DETAIL_STANDARD_FINGER_TREE_HPP_INCLUDED

#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

template<auto Monoid>
class StandardFingerTree {
public:
    using value_type = typename decltype(Monoid)::S;

private:
    struct Item;
    using ItemPtr = std::shared_ptr<const Item>;

    struct Item {
        std::optional<value_type> value;
        std::vector<ItemPtr> children;
        value_type measure;
        std::size_t leaf_size;

        explicit Item(value_type value_)
            : value(std::move(value_)),
              measure(*value),
              leaf_size(1) {}

        explicit Item(std::vector<ItemPtr> children_)
            : children(std::move(children_)),
              measure(Monoid.e()),
              leaf_size(0) {
            if (children.size() != 2 && children.size() != 3) {
                throw std::logic_error("finger tree node arity");
            }
            for (const ItemPtr& child : children) {
                measure = Monoid.op(measure, child->measure);
                leaf_size += child->leaf_size;
            }
        }

        [[nodiscard]] bool is_leaf() const noexcept {
            return value.has_value();
        }
    };

    struct Level {
        enum class Kind { empty, single, deep };
        Kind kind = Kind::empty;
        ItemPtr single;
        std::vector<ItemPtr> prefix;
        std::unique_ptr<Level> middle;
        std::vector<ItemPtr> suffix;
        value_type measure = Monoid.e();
        std::size_t leaf_size = 0;
    };

    struct DigitSplit {
        std::vector<ItemPtr> left;
        ItemPtr pivot;
        std::vector<ItemPtr> right;
    };

    struct LevelSplit {
        std::unique_ptr<Level> left;
        ItemPtr pivot;
        std::unique_ptr<Level> right;
    };

    std::unique_ptr<Level> root_ = make_empty();

    static std::unique_ptr<Level> make_empty() {
        return std::make_unique<Level>();
    }

    static std::unique_ptr<Level> make_single(ItemPtr item) {
        auto result = std::make_unique<Level>();
        result->kind = Level::Kind::single;
        result->single = std::move(item);
        update(result.get());
        return result;
    }

    static std::unique_ptr<Level> make_deep(
        std::vector<ItemPtr> prefix,
        std::unique_ptr<Level> middle,
        std::vector<ItemPtr> suffix
    ) {
        if (prefix.empty() || prefix.size() > 4
            || suffix.empty() || suffix.size() > 4) {
            throw std::logic_error("finger tree digit size");
        }
        auto result = std::make_unique<Level>();
        result->kind = Level::Kind::deep;
        result->prefix = std::move(prefix);
        result->middle = std::move(middle);
        result->suffix = std::move(suffix);
        update(result.get());
        return result;
    }

    static value_type measure_items(const std::vector<ItemPtr>& items) {
        value_type result = Monoid.e();
        for (const ItemPtr& item : items) {
            result = Monoid.op(result, item->measure);
        }
        return result;
    }

    static std::size_t size_items(const std::vector<ItemPtr>& items) {
        std::size_t result = 0;
        for (const ItemPtr& item : items) result += item->leaf_size;
        return result;
    }

    static void update(Level* level) {
        level->measure = Monoid.e();
        level->leaf_size = 0;
        if (level->kind == Level::Kind::empty) return;
        if (level->kind == Level::Kind::single) {
            level->measure = level->single->measure;
            level->leaf_size = level->single->leaf_size;
            return;
        }
        level->measure = measure_items(level->prefix);
        level->leaf_size = size_items(level->prefix);
        level->measure = Monoid.op(level->measure, level->middle->measure);
        level->leaf_size += level->middle->leaf_size;
        level->measure = Monoid.op(
            level->measure, measure_items(level->suffix)
        );
        level->leaf_size += size_items(level->suffix);
    }

    static ItemPtr make_node(std::vector<ItemPtr> children) {
        return std::make_shared<Item>(std::move(children));
    }

    static std::unique_ptr<Level> push_front_item(
        std::unique_ptr<Level> tree, ItemPtr item
    ) {
        if (tree->kind == Level::Kind::empty) {
            return make_single(std::move(item));
        }
        if (tree->kind == Level::Kind::single) {
            std::vector<ItemPtr> prefix{std::move(item)};
            std::vector<ItemPtr> suffix{tree->single};
            return make_deep(
                std::move(prefix), make_empty(), std::move(suffix)
            );
        }
        if (tree->prefix.size() < 4) {
            tree->prefix.insert(tree->prefix.begin(), std::move(item));
            update(tree.get());
            return tree;
        }
        std::vector<ItemPtr> promoted{
            tree->prefix[1], tree->prefix[2], tree->prefix[3]
        };
        tree->middle = push_front_item(
            std::move(tree->middle), make_node(std::move(promoted))
        );
        tree->prefix = {std::move(item), tree->prefix[0]};
        update(tree.get());
        return tree;
    }

    static std::unique_ptr<Level> push_back_item(
        std::unique_ptr<Level> tree, ItemPtr item
    ) {
        if (tree->kind == Level::Kind::empty) {
            return make_single(std::move(item));
        }
        if (tree->kind == Level::Kind::single) {
            std::vector<ItemPtr> prefix{tree->single};
            std::vector<ItemPtr> suffix{std::move(item)};
            return make_deep(
                std::move(prefix), make_empty(), std::move(suffix)
            );
        }
        if (tree->suffix.size() < 4) {
            tree->suffix.push_back(std::move(item));
            update(tree.get());
            return tree;
        }
        std::vector<ItemPtr> promoted{
            tree->suffix[0], tree->suffix[1], tree->suffix[2]
        };
        tree->middle = push_back_item(
            std::move(tree->middle), make_node(std::move(promoted))
        );
        tree->suffix = {tree->suffix[3], std::move(item)};
        update(tree.get());
        return tree;
    }

    static std::unique_ptr<Level> from_items(
        std::vector<ItemPtr> items
    ) {
        auto result = make_empty();
        for (ItemPtr& item : items) {
            result = push_back_item(std::move(result), std::move(item));
        }
        return result;
    }

    static std::pair<ItemPtr, std::unique_ptr<Level>> view_left(
        std::unique_ptr<Level> tree
    ) {
        if (tree->kind == Level::Kind::empty) {
            throw std::out_of_range("FingerTree::pop_front");
        }
        if (tree->kind == Level::Kind::single) {
            return {tree->single, make_empty()};
        }
        ItemPtr result = tree->prefix.front();
        tree->prefix.erase(tree->prefix.begin());
        if (!tree->prefix.empty()) {
            update(tree.get());
            return {std::move(result), std::move(tree)};
        }
        if (tree->middle->kind != Level::Kind::empty) {
            auto [group, middle] = view_left(std::move(tree->middle));
            tree->prefix = group->children;
            tree->middle = std::move(middle);
            update(tree.get());
            return {std::move(result), std::move(tree)};
        }
        return {std::move(result), from_items(std::move(tree->suffix))};
    }

    static std::pair<ItemPtr, std::unique_ptr<Level>> view_right(
        std::unique_ptr<Level> tree
    ) {
        if (tree->kind == Level::Kind::empty) {
            throw std::out_of_range("FingerTree::pop_back");
        }
        if (tree->kind == Level::Kind::single) {
            return {tree->single, make_empty()};
        }
        ItemPtr result = tree->suffix.back();
        tree->suffix.pop_back();
        if (!tree->suffix.empty()) {
            update(tree.get());
            return {std::move(result), std::move(tree)};
        }
        if (tree->middle->kind != Level::Kind::empty) {
            auto [group, middle] = view_right(std::move(tree->middle));
            tree->suffix = group->children;
            tree->middle = std::move(middle);
            update(tree.get());
            return {std::move(result), std::move(tree)};
        }
        return {std::move(result), from_items(std::move(tree->prefix))};
    }

    static std::unique_ptr<Level> deep_left(
        std::vector<ItemPtr> prefix,
        std::unique_ptr<Level> middle,
        std::vector<ItemPtr> suffix
    ) {
        if (!prefix.empty()) {
            return make_deep(
                std::move(prefix), std::move(middle), std::move(suffix)
            );
        }
        if (middle->kind == Level::Kind::empty) {
            return from_items(std::move(suffix));
        }
        auto [group, rest] = view_left(std::move(middle));
        return make_deep(
            group->children, std::move(rest), std::move(suffix)
        );
    }

    static std::unique_ptr<Level> deep_right(
        std::vector<ItemPtr> prefix,
        std::unique_ptr<Level> middle,
        std::vector<ItemPtr> suffix
    ) {
        if (!suffix.empty()) {
            return make_deep(
                std::move(prefix), std::move(middle), std::move(suffix)
            );
        }
        if (middle->kind == Level::Kind::empty) {
            return from_items(std::move(prefix));
        }
        auto [group, rest] = view_right(std::move(middle));
        return make_deep(
            std::move(prefix), std::move(rest), group->children
        );
    }

    static std::vector<ItemPtr> make_nodes(
        const std::vector<ItemPtr>& items
    ) {
        if (items.size() < 2) {
            throw std::logic_error("finger tree concatenation bridge");
        }
        std::vector<ItemPtr> result;
        std::size_t index = 0;
        std::size_t remaining = items.size();
        if (remaining % 3 == 1) {
            result.push_back(make_node({items[0], items[1]}));
            result.push_back(make_node({items[2], items[3]}));
            index = 4;
            remaining -= 4;
        } else if (remaining % 3 == 2) {
            result.push_back(make_node({items[0], items[1]}));
            index = 2;
            remaining -= 2;
        }
        while (remaining != 0) {
            result.push_back(make_node(
                {items[index], items[index + 1], items[index + 2]}
            ));
            index += 3;
            remaining -= 3;
        }
        return result;
    }

    static std::unique_ptr<Level> append_between(
        std::unique_ptr<Level> left,
        std::vector<ItemPtr> between,
        std::unique_ptr<Level> right
    ) {
        if (left->kind == Level::Kind::empty) {
            for (auto iterator = between.rbegin();
                 iterator != between.rend(); ++iterator) {
                right = push_front_item(std::move(right), *iterator);
            }
            return right;
        }
        if (left->kind == Level::Kind::single) {
            for (auto iterator = between.rbegin();
                 iterator != between.rend(); ++iterator) {
                right = push_front_item(std::move(right), *iterator);
            }
            return push_front_item(std::move(right), left->single);
        }
        if (right->kind == Level::Kind::empty) {
            for (ItemPtr& item : between) {
                left = push_back_item(std::move(left), std::move(item));
            }
            return left;
        }
        if (right->kind == Level::Kind::single) {
            for (ItemPtr& item : between) {
                left = push_back_item(std::move(left), std::move(item));
            }
            return push_back_item(std::move(left), right->single);
        }

        std::vector<ItemPtr> bridge = left->suffix;
        bridge.insert(bridge.end(), between.begin(), between.end());
        bridge.insert(
            bridge.end(), right->prefix.begin(), right->prefix.end()
        );
        auto middle = append_between(
            std::move(left->middle),
            make_nodes(bridge),
            std::move(right->middle)
        );
        return make_deep(
            std::move(left->prefix),
            std::move(middle),
            std::move(right->suffix)
        );
    }

    static DigitSplit split_digit_count(
        const std::vector<ItemPtr>& digit, std::size_t count
    ) {
        DigitSplit result;
        std::size_t consumed = 0;
        for (std::size_t index = 0; index < digit.size(); ++index) {
            if (count < consumed + digit[index]->leaf_size) {
                result.pivot = digit[index];
                result.right.assign(
                    digit.begin() + static_cast<std::ptrdiff_t>(index + 1),
                    digit.end()
                );
                return result;
            }
            result.left.push_back(digit[index]);
            consumed += digit[index]->leaf_size;
        }
        throw std::logic_error("finger tree count split");
    }

    static LevelSplit split_level_count(
        std::unique_ptr<Level> tree, std::size_t count
    ) {
        if (tree->kind == Level::Kind::single) {
            return {make_empty(), tree->single, make_empty()};
        }
        if (tree->kind != Level::Kind::deep) {
            throw std::logic_error("finger tree empty split");
        }
        const std::size_t prefix_size = size_items(tree->prefix);
        if (count < prefix_size) {
            DigitSplit split = split_digit_count(tree->prefix, count);
            return {
                from_items(std::move(split.left)),
                std::move(split.pivot),
                deep_left(
                    std::move(split.right),
                    std::move(tree->middle),
                    std::move(tree->suffix)
                )
            };
        }
        const std::size_t middle_size = tree->middle->leaf_size;
        if (count < prefix_size + middle_size) {
            LevelSplit middle_split = split_level_count(
                std::move(tree->middle), count - prefix_size
            );
            const std::size_t before_children =
                prefix_size + middle_split.left->leaf_size;
            DigitSplit child_split = split_digit_count(
                middle_split.pivot->children, count - before_children
            );
            return {
                deep_right(
                    std::move(tree->prefix),
                    std::move(middle_split.left),
                    std::move(child_split.left)
                ),
                std::move(child_split.pivot),
                deep_left(
                    std::move(child_split.right),
                    std::move(middle_split.right),
                    std::move(tree->suffix)
                )
            };
        }
        DigitSplit split = split_digit_count(
            tree->suffix, count - prefix_size - middle_size
        );
        return {
            deep_right(
                std::move(tree->prefix),
                std::move(tree->middle),
                std::move(split.left)
            ),
            std::move(split.pivot),
            from_items(std::move(split.right))
        };
    }

    template<class Predicate>
    static DigitSplit split_digit_measured(
        const std::vector<ItemPtr>& digit,
        value_type accumulator,
        Predicate& predicate
    ) {
        DigitSplit result;
        for (std::size_t index = 0; index < digit.size(); ++index) {
            value_type next = Monoid.op(accumulator, digit[index]->measure);
            if (predicate(next)) {
                result.pivot = digit[index];
                result.right.assign(
                    digit.begin() + static_cast<std::ptrdiff_t>(index + 1),
                    digit.end()
                );
                return result;
            }
            result.left.push_back(digit[index]);
            accumulator = std::move(next);
        }
        throw std::logic_error("finger tree measured digit split");
    }

    template<class Predicate>
    static LevelSplit split_level_measured(
        std::unique_ptr<Level> tree,
        value_type accumulator,
        Predicate& predicate
    ) {
        if (tree->kind == Level::Kind::single) {
            return {make_empty(), tree->single, make_empty()};
        }
        if (tree->kind != Level::Kind::deep) {
            throw std::logic_error("finger tree empty measured split");
        }
        value_type after_prefix = Monoid.op(
            accumulator, measure_items(tree->prefix)
        );
        if (predicate(after_prefix)) {
            DigitSplit split = split_digit_measured(
                tree->prefix, std::move(accumulator), predicate
            );
            return {
                from_items(std::move(split.left)),
                std::move(split.pivot),
                deep_left(
                    std::move(split.right),
                    std::move(tree->middle),
                    std::move(tree->suffix)
                )
            };
        }
        value_type after_middle = Monoid.op(
            after_prefix, tree->middle->measure
        );
        if (predicate(after_middle)) {
            LevelSplit middle_split = split_level_measured(
                std::move(tree->middle), after_prefix, predicate
            );
            value_type before_children = Monoid.op(
                after_prefix, middle_split.left->measure
            );
            DigitSplit child_split = split_digit_measured(
                middle_split.pivot->children,
                std::move(before_children),
                predicate
            );
            return {
                deep_right(
                    std::move(tree->prefix),
                    std::move(middle_split.left),
                    std::move(child_split.left)
                ),
                std::move(child_split.pivot),
                deep_left(
                    std::move(child_split.right),
                    std::move(middle_split.right),
                    std::move(tree->suffix)
                )
            };
        }
        DigitSplit split = split_digit_measured(
            tree->suffix, std::move(after_middle), predicate
        );
        return {
            deep_right(
                std::move(tree->prefix),
                std::move(tree->middle),
                std::move(split.left)
            ),
            std::move(split.pivot),
            from_items(std::move(split.right))
        };
    }

    static const value_type& value_at_item(
        const ItemPtr& item, std::size_t index
    ) {
        if (item->is_leaf()) {
            if (index != 0) throw std::logic_error("finger tree leaf index");
            return *item->value;
        }
        for (const ItemPtr& child : item->children) {
            if (index < child->leaf_size) {
                return value_at_item(child, index);
            }
            index -= child->leaf_size;
        }
        throw std::logic_error("finger tree node index");
    }

    static const value_type& value_at_level(
        const Level* level, std::size_t index
    ) {
        if (level->kind == Level::Kind::single) {
            return value_at_item(level->single, index);
        }
        if (level->kind != Level::Kind::deep) {
            throw std::out_of_range("FingerTree::at");
        }
        for (const ItemPtr& item : level->prefix) {
            if (index < item->leaf_size) return value_at_item(item, index);
            index -= item->leaf_size;
        }
        if (index < level->middle->leaf_size) {
            return value_at_level(level->middle.get(), index);
        }
        index -= level->middle->leaf_size;
        for (const ItemPtr& item : level->suffix) {
            if (index < item->leaf_size) return value_at_item(item, index);
            index -= item->leaf_size;
        }
        throw std::out_of_range("FingerTree::at");
    }

public:
    StandardFingerTree() = default;
    StandardFingerTree(StandardFingerTree&&) noexcept = default;
    StandardFingerTree& operator=(StandardFingerTree&&) noexcept = default;
    StandardFingerTree(const StandardFingerTree&) = delete;
    StandardFingerTree& operator=(const StandardFingerTree&) = delete;

    [[nodiscard]] bool empty() const noexcept {
        return root_->kind == Level::Kind::empty;
    }
    [[nodiscard]] std::size_t size() const noexcept {
        return root_->leaf_size;
    }
    void clear() { root_ = make_empty(); }

    void push_front(const value_type& value) {
        root_ = push_front_item(
            std::move(root_), std::make_shared<Item>(value)
        );
    }
    void push_front(value_type&& value) {
        root_ = push_front_item(
            std::move(root_), std::make_shared<Item>(std::move(value))
        );
    }
    void push_back(const value_type& value) {
        root_ = push_back_item(
            std::move(root_), std::make_shared<Item>(value)
        );
    }
    void push_back(value_type&& value) {
        root_ = push_back_item(
            std::move(root_), std::make_shared<Item>(std::move(value))
        );
    }

    void pop_front() {
        auto [item, rest] = view_left(std::move(root_));
        if (!item->is_leaf()) throw std::logic_error("finger tree level");
        root_ = std::move(rest);
    }
    void pop_back() {
        auto [item, rest] = view_right(std::move(root_));
        if (!item->is_leaf()) throw std::logic_error("finger tree level");
        root_ = std::move(rest);
    }
    [[nodiscard]] const value_type& front() const {
        if (empty()) throw std::out_of_range("FingerTree::front");
        const ItemPtr& item = root_->kind == Level::Kind::single
            ? root_->single : root_->prefix.front();
        if (!item->is_leaf()) {
            throw std::logic_error("finger tree root digit level");
        }
        return *item->value;
    }
    [[nodiscard]] const value_type& back() const {
        if (empty()) throw std::out_of_range("FingerTree::back");
        const ItemPtr& item = root_->kind == Level::Kind::single
            ? root_->single : root_->suffix.back();
        if (!item->is_leaf()) {
            throw std::logic_error("finger tree root digit level");
        }
        return *item->value;
    }
    [[nodiscard]] const value_type& at(std::size_t index) const {
        if (index >= size()) throw std::out_of_range("FingerTree::at");
        return value_at_level(root_.get(), index);
    }
    [[nodiscard]] value_type fold() const { return root_->measure; }

    std::pair<StandardFingerTree, StandardFingerTree> split(
        std::size_t index
    ) && {
        if (index > size()) throw std::out_of_range("FingerTree::split");
        StandardFingerTree left;
        StandardFingerTree right;
        if (index == 0) {
            right.root_ = std::move(root_);
            return {std::move(left), std::move(right)};
        }
        if (index == size()) {
            left.root_ = std::move(root_);
            return {std::move(left), std::move(right)};
        }
        LevelSplit split = split_level_count(std::move(root_), index);
        left.root_ = std::move(split.left);
        right.root_ = push_front_item(
            std::move(split.right), std::move(split.pivot)
        );
        return {std::move(left), std::move(right)};
    }

    template<class Predicate>
    std::pair<StandardFingerTree, StandardFingerTree> split_by(
        Predicate predicate
    ) && {
        StandardFingerTree left;
        StandardFingerTree right;
        if (empty() || predicate(Monoid.e())) {
            right.root_ = std::move(root_);
            return {std::move(left), std::move(right)};
        }
        if (!predicate(root_->measure)) {
            left.root_ = std::move(root_);
            return {std::move(left), std::move(right)};
        }
        LevelSplit split = split_level_measured(
            std::move(root_), Monoid.e(), predicate
        );
        left.root_ = std::move(split.left);
        right.root_ = push_front_item(
            std::move(split.right), std::move(split.pivot)
        );
        return {std::move(left), std::move(right)};
    }

    void concatenate(StandardFingerTree&& other) {
        root_ = append_between(
            std::move(root_), {}, std::move(other.root_)
        );
        other.root_ = make_empty();
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_DETAIL_STANDARD_FINGER_TREE_HPP_INCLUDED
