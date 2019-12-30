#ifndef JG_BUCKET_ITERATOR_HPP
#define JG_BUCKET_ITERATOR_HPP

#include "node.hpp"

#include <iterator>
#include <vector>

namespace jg::details
{

template <class Key, class T, bool isConst, bool projectToConstKey>
class bucket_iterator
{
    using nodes_container_type =
        std::conditional_t<isConst, const std::vector<node<Key, T>>, std::vector<node<Key, T>>>;
    using node_index_type = node_index_t<Key, T>;
    using projected_type = std::pair<std::conditional_t<projectToConstKey, const Key, Key>, T>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::conditional_t<isConst, const projected_type, projected_type>;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using pointer = value_type*;

    bucket_iterator() = default;
    bucket_iterator(nodes_container_type& nodes_container) : nodes_container(&nodes_container) {}

    bucket_iterator(node_index_type index, nodes_container_type& nodes_container)
        : nodes_container(&nodes_container), current_node_index_(index)
    {}

    constexpr auto operator*() const noexcept -> reference
    {
        if constexpr (projectToConstKey)
        {
            return (*nodes_container)[current_node_index_].pair.const_;
        }
        else
        {
            return (*nodes_container)[current_node_index_].pair.non_const_;
        }
    }

    constexpr auto operator++() noexcept -> bucket_iterator&
    {
        current_node_index_ = (*nodes_container)[current_node_index_].next;
        return *this;
    }

    constexpr auto operator++(int) noexcept -> bucket_iterator
    {
        auto old = (*this);
        ++(*this);
        return old;
    }

    constexpr auto operator-> () const noexcept -> pointer
    {
        if constexpr (projectToConstKey)
        {
            return &(*nodes_container)[current_node_index_].pair.const_;
        }
        else
        {
            return &(*nodes_container)[current_node_index_].pair.non_const_;
        }
    }

    auto current_node_index() const -> node_index_type { return current_node_index_; }

private:
    nodes_container_type* nodes_container;
    node_index_type current_node_index_ = node_end_index<Key, T>;
};

template <class Key, class T, bool isConst, bool projectToConstKey>
constexpr auto operator!=(
    const bucket_iterator<Key, T, isConst, projectToConstKey>& lhs,
    const bucket_iterator<Key, T, isConst, projectToConstKey>& rhs) noexcept -> bool
{
    return lhs.current_node_index() != rhs.current_node_index();
}

} // namespace jg::details

#endif // JG_BUCKET_ITERATOR_HPP
