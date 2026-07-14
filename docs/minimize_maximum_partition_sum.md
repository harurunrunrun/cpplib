---
title: Minimize Maximum Partition Sum (最大区間和最小化)
documentation_of: ../src/algorithm/other/minimize_maximum_partition_sum.hpp
---

整数列をちょうど `k` 個の空でない連続区間へ分割し、各区間和の最大値を最小化する。負数を含む列に対応する。

## `minimize_maximum_partition_sum`

```cpp
long long minimize_maximum_partition_sum(
    const vector<long long>& values,
    size_t partition_count
)
```

`values` を順序を保ったまま、ちょうど `partition_count` 個の空でない連続区間へ分割する。全区間和の最大値として実現できる最小値を返す。

固定した上限 `x` に対し、prefix和を $P_i$ とすると、位置 `j` の直後から位置 `i` までを1区間にできる条件は

$$
P_j \ge P_i-x
$$

である。座標圧縮したprefix和上に、各位置へ到達できる区間数の最小値と最大値を保持し、このsuffixをqueryして判定する。

### ちょうど `k` 個をmin/maxだけで判定できる理由

一般のDAGでは、始点から終点までのpath長集合に欠番があり得る。しかし、このprefix和の閾値graphでは到達可能な区間数が必ず整数区間になる。

位置 `i` より前を処理した時点で、ちょうど `t` 区間で到達した位置のprefix和の最大値を $B_t$ とする。到達不能なら $-\infty$ とする。任意の値 `y` に対するsuperlevel set

$$
C_y=\{t\mid B_t\ge y\}
$$

が整数区間であることを不変条件にする。初期状態では空集合または `{0}` である。位置 `i` へ到達できる区間数は $1+C_{P_i-x}$ なので区間になる。

位置 `i` を追加した後、`y > P_i` なら $C_y$ は変化しない。`y <= P_i` なら、新しい集合は古い $C_y$ と $1+C_{P_i-x}$ の和集合である。古い $C_y$ と $C_{P_i-x}$ は同じ列 $B$ のsuperlevel setなので、一方が他方を包含する。包含関係にある2整数区間の一方を1だけ平行移動して和集合を取っても欠番は生じない。したがって不変条件が保たれる。

以上より、末尾へ到達する区間数の最小値を `minimum`、最大値を `maximum` とすると、固定上限 `x` でちょうど `k` 区間へ分割できる必要十分条件は `minimum <= k && k <= maximum` である。

### 計算量

`N = values.size()`、$W=\sum_i |values_i|+1$ とする。`long long` 入力では二分探索回数は高々127回である。

| API・場合 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `partition_count == 1` または `partition_count == N` | $O(N)$ | `partition_count == 1` は $O(N)$、`partition_count == N` は $O(1)$ |
| `minimize_maximum_partition_sum`（一般の場合） | $O(N\log N\log W)$ | $O(N)$ |

## 注意点

- `values` は空でなく、`1 <= partition_count <= values.size()` でなければならない。違反時は `invalid_argument` を送出する。
- 各区間は空であってはならない。
- 中間のprefix和、二分探索境界、差分はsigned 128-bit整数で計算する。答えが `long long` で表現できない場合は `overflow_error` を送出する。
- 負数を許すため、非負列で使える「左から貪欲に詰める」判定は使用していない。
- 最適値だけを返し、分割位置は返さない。
