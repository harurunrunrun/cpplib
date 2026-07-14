#pragma once

#include <algorithm>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

#include "../../algorithm/tree/euler_tour.hpp"
#include "../segtree/lazysegtree.hpp"
#include "../types/monoid_act.hpp"

struct EmployeeSalaryAggregate{
    long long sum;
    long long minimum;
    long long count;
};

constexpr EmployeeSalaryAggregate employee_salary_op(
    EmployeeSalaryAggregate left,
    EmployeeSalaryAggregate right
){
    return {left.sum + right.sum, std::min(left.minimum, right.minimum), left.count + right.count};
}
constexpr EmployeeSalaryAggregate employee_salary_identity(){
    return {0, std::numeric_limits<long long>::max(), 0};
}
constexpr EmployeeSalaryAggregate employee_salary_add(
    long long delta,
    EmployeeSalaryAggregate aggregate
){
    if(aggregate.count != 0){
        aggregate.sum += delta * aggregate.count;
        aggregate.minimum += delta;
    }
    return aggregate;
}
constexpr long long employee_salary_compose(long long added, long long previous){ return added + previous; }
constexpr long long employee_salary_no_add(){ return 0; }
constexpr auto employee_salary_action = Monoid_Act<
    employee_salary_op,
    employee_salary_identity,
    employee_salary_add,
    employee_salary_compose,
    employee_salary_no_add
>{};

template<int MAX_SIZE>
struct EmployeeSalaryTree{
private:
    int _n;
    int root_vertex = -1;
    std::vector<long long> salaries;
    std::unique_ptr<TreeEulerTour<MAX_SIZE>> tour;
    std::unique_ptr<LazySegtree<employee_salary_action, MAX_SIZE>> tree;

public:
    explicit EmployeeSalaryTree(int n):
        _n(n), salaries(static_cast<std::size_t>(n)),
        tour(std::make_unique<TreeEulerTour<MAX_SIZE>>(n)){}

    void set_employee(int employee, int parent, long long salary){
        if(employee < 0 || _n <= employee || parent < -1 || _n <= parent){
            throw std::runtime_error("library assertion fault: range violation (set_employee).");
        }
        salaries[static_cast<std::size_t>(employee)] = salary;
        if(parent == -1) root_vertex = employee;
        else tour->add_edge(parent, employee);
    }

    void build(){
        if(root_vertex == -1) throw std::runtime_error("library assertion fault: root is missing (build).");
        tour->build(root_vertex);
        std::vector<EmployeeSalaryAggregate> preorder(static_cast<std::size_t>(_n));
        for(int employee = 0; employee < _n; ++employee){
            const long long salary = salaries[static_cast<std::size_t>(employee)];
            preorder[static_cast<std::size_t>(tour->tin(employee))] = {salary, salary, 1};
        }
        tree = std::make_unique<LazySegtree<employee_salary_action, MAX_SIZE>>(preorder);
    }

    long long subtree_sum(int employee) const{
        if(!tree) throw std::runtime_error("library assertion fault: not built (subtree_sum).");
        const auto [left, right] = tour->subtree_range(employee);
        return tree->prod(left, right).sum;
    }

    long long increase_subtree(int employee, long long cap = 1000){
        if(!tree) throw std::runtime_error("library assertion fault: not built (increase_subtree).");
        const auto [left, right] = tour->subtree_range(employee);
        const long long delta = std::min(cap, tree->prod(left, right).minimum);
        tree->apply(left, right, delta);
        return delta;
    }
};
