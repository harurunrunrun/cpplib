// competitive-verifier: PROBLEM https://www.spoj.com/problems/SALMAN/

#include <iostream>
#include "../../src/structure/tree/employee_salary_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    for(int test = 1; test <= tests; ++test){
        int n, q;
        std::cin >> n >> q;
        EmployeeSalaryTree<400000> tree(n);
        for(int employee = 0; employee < n; ++employee){
            int parent;
            long long salary;
            std::cin >> parent >> salary;
            tree.set_employee(employee, parent - 1, salary);
        }
        tree.build();
        std::cout << "Case " << test << ":\n";
        while(q-- != 0){
            int type, employee;
            std::cin >> type >> employee;
            --employee;
            if(type == 1) std::cout << tree.subtree_sum(employee) << '\n';
            else (void)tree.increase_subtree(employee);
        }
    }
}
