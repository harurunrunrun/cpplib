---
title: Maximum Philosopher Stones (賢者の石の最大回収数) [BYTESM2]
documentation_of: ../src/algorithm/other/maximum_philosopher_stones.hpp
---

格子の最上段から最下段へ移動して得る値の最大和を返す。

# maximum_philosopher_stones

```cpp
long long maximum_philosopher_stones(const vector<vector<int>>& grid);
```

次の行の同じ列または左右に1列ずれたセルへ移動できる。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `maximum_philosopher_stones` | $O(HW)$ | $O(W)$ |

## 注意点

- 空格子では0を返す。
- 行長が不一致なら `runtime_error` を送出する。
