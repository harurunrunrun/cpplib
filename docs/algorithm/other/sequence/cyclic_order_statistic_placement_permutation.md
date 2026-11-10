---
title: Cyclic Order-statistic Placement Permutation (巡回順序統計配置順列) [CTRICK]
documentation_of: ../../../../src/algorithm/other/sequence/cyclic_order_statistic_placement_permutation.hpp
---

空き位置を巡回しながら、空き位置の中での順位で配置先を選んで得られる順列を構築する。

## API

### `cyclic_order_statistic_placement_permutation(size)`

```cpp
template<int MAX_SIZE = -1>
vector<int> cyclic_order_statistic_placement_permutation(int size);
```

値 `value = 1, 2, ..., size` を順に配置する。順位を $0$ で初期化し、残りの空き位置数を `remaining` として、各回で `rank = (rank + value) % remaining` と更新し、0-indexed で `rank` 番目の空き位置へ `value` を置く。置いた後は `remaining` を 1 減らし、それが正なら `rank %= remaining` とする。戻り値は位置順に並べた配置結果である。

`MAX_SIZE` を省略すると、空き位置の管理に整数全域（`int` の範囲）を扱う可変長の `Int_Set` を使う。指定すると全域が $[0, \texttt{MAX_SIZE})$ の固定長版になる。

```cpp
auto result = cyclic_order_statistic_placement_permutation(n);
auto fixed = cyclic_order_statistic_placement_permutation<100000>(n);
```

## 時間計算量・空間計算量

$N$ を `size`、$W$ を空き位置を管理する分岐数 16 の trie の深さ（固定長版は $O(\log\texttt{MAX_SIZE})$、可変長版は `int` の範囲に対する定数）とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `cyclic_order_statistic_placement_permutation` | $O(NW)$ | $O(NW)$ |

## 注意点

- `0 <= size` かつ、固定長版では `size <= MAX_SIZE` でなければならない。違反時は `out_of_range` を送出する。
- `MAX_SIZE` を指定する場合は正でなければならない（`static_assert`）。
- 内部の順序統計の取得に失敗した場合は `runtime_error` を送出する。
