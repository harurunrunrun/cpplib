---
title: Employee Salary Tree (従業員給与部分木更新) [SALMAN]
documentation_of: ../src/structure/tree/employee_salary_tree.hpp
---

従業員木の部分木給与和と、部分木一括加算を扱う。

## API

### `EmployeeSalaryAggregate` と給与加算モノイド作用

`EmployeeSalaryAggregate` は区間の `sum`・`minimum`・`count` を保持する。
`employee_salary_op`、`employee_salary_identity`、`employee_salary_add`、
`employee_salary_compose`、`employee_salary_no_add` と
`employee_salary_action` は、この集約値への区間加算を定義する。
各関数とfield参照の時間計算量は $O(1)$。


### `EmployeeSalaryTree<MAX_SIZE>(n)`

頂点数 `n` の空の構造を作る。時間計算量は $O(n)$。

### `set_employee(employee, parent, salary)`

従業員の親と初期給与を設定する。根では `parent=-1`。時間計算量は償却 $O(1)$。

### `build()`

部分木区間と遅延segment treeを構築する。時間計算量は $O(n)$。

### `subtree_sum(employee)`

部分木の給与和を返す。時間計算量は $O(\log n)$。

### `increase_subtree(employee, cap = 1000)`

`min(cap, 部分木の最小給与)` を部分木全体へ加え、その加算値を返す。時間計算量は $O(\log n)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下で、親関係は根が1個の木でなければならない。
- `build` 前のqueryでは `runtime_error`。
- 給与和と加算後給与は `long long` の範囲に入る必要がある。
