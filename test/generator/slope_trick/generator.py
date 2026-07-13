#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import heapq
import random
from pathlib import Path

LOWEST = -(1 << 63)
HIGHEST = (1 << 63) - 1


class ReferenceSlopeTrick:
    def __init__(self) -> None:
        self.minimum = 0
        self.left_add = 0
        self.right_add = 0
        self.left: list[int] = []
        self.right: list[int] = []

    def left_top(self) -> int:
        return -self.left[0] + self.left_add

    def right_top(self) -> int:
        return self.right[0] + self.right_add

    def push_left(self, value: int) -> None:
        heapq.heappush(self.left, -(value - self.left_add))

    def push_right(self, value: int) -> None:
        heapq.heappush(self.right, value - self.right_add)

    def pop_left(self) -> int:
        return -heapq.heappop(self.left) + self.left_add

    def pop_right(self) -> int:
        return heapq.heappop(self.right) + self.right_add

    def add_a_minus_x(self, a: int) -> None:
        if not self.right:
            self.push_left(a)
        elif self.right_top() < a:
            right = self.pop_right()
            self.minimum += a - right
            self.push_right(a)
            self.push_left(right)
        else:
            self.push_left(a)

    def add_x_minus_a(self, a: int) -> None:
        if not self.left:
            self.push_right(a)
        elif a < self.left_top():
            left = self.pop_left()
            self.minimum += left - a
            self.push_left(a)
            self.push_right(left)
        else:
            self.push_right(a)

    def add_abs(self, a: int) -> None:
        self.add_a_minus_x(a)
        self.add_x_minus_a(a)

    def eval(self, x: int) -> int:
        result = self.minimum
        result += sum(max((-stored + self.left_add) - x, 0) for stored in self.left)
        result += sum(max(x - (stored + self.right_add), 0) for stored in self.right)
        return result

    def argmin(self) -> tuple[int, int]:
        left = self.left_top() if self.left else LOWEST
        right = self.right_top() if self.right else HIGHEST
        return left, right

    def clear(self) -> None:
        self.__init__()


def write_case(out_dir: Path, index: int, commands: list[str], output: list[str]) -> None:
    (out_dir / f"case_{index:02d}.in").write_text(
        f"{len(commands)}\n" + "\n".join(commands) + "\n",
        encoding="utf-8",
    )
    (out_dir / f"case_{index:02d}.out").write_text(
        "\n".join(output) + "\n", encoding="utf-8"
    )


def hand_case() -> tuple[list[str], list[str]]:
    slope = ReferenceSlopeTrick()
    commands: list[str] = []
    output: list[str] = []

    def query_min() -> None:
        commands.append("MIN")
        output.append(str(slope.minimum))

    def query_argmin() -> None:
        commands.append("ARGMIN")
        output.append(" ".join(map(str, slope.argmin())))

    def query_eval(x: int) -> None:
        commands.append(f"EVAL {x}")
        output.append(str(slope.eval(x)))

    query_min()
    query_argmin()
    commands.append("ABS 3")
    slope.add_abs(3)
    query_min()
    query_argmin()
    query_eval(0)
    commands.append("ABS -2")
    slope.add_abs(-2)
    commands.append("CONST 7")
    slope.minimum += 7
    query_min()
    query_argmin()
    for x in (-10, -2, 0, 3, 10):
        query_eval(x)

    commands.append("SHIFT 4")
    slope.left_add += 4
    slope.right_add += 4
    query_argmin()
    commands.append("SHIFT_RANGE -1 2")
    slope.left_add -= 1
    slope.right_add += 2
    query_argmin()

    commands.append("CUMULATIVE_MIN")
    slope.right.clear()
    query_argmin()
    query_eval(100)
    commands.append("CLEAR_LEFT")
    slope.left.clear()
    query_argmin()
    query_eval(-100)

    commands.append("CLEAR")
    slope.clear()
    query_min()
    query_argmin()
    return commands, output


def random_naive_case(rng: random.Random) -> tuple[list[str], list[str]]:
    bound = 4096
    values = [0] * (2 * bound + 1)
    commands: list[str] = []
    output: list[str] = []
    infinity = 10**30

    def at(x: int) -> int:
        return x + bound

    for step in range(500):
        operation = rng.randrange(9)
        a = rng.randrange(-80, 81)
        if operation == 0:
            c = rng.randrange(-20, 21)
            commands.append(f"CONST {c}")
            values = [value + c for value in values]
        elif operation == 1:
            commands.append(f"A_MINUS_X {a}")
            values = [
                value + max(a - x, 0)
                for x, value in zip(range(-bound, bound + 1), values)
            ]
        elif operation == 2:
            commands.append(f"X_MINUS_A {a}")
            values = [
                value + max(x - a, 0)
                for x, value in zip(range(-bound, bound + 1), values)
            ]
        elif operation == 3:
            commands.append(f"ABS {a}")
            values = [
                value + abs(x - a)
                for x, value in zip(range(-bound, bound + 1), values)
            ]
        elif operation == 4:
            amount = rng.randrange(-4, 5)
            commands.append(f"SHIFT {amount}")
            previous = values
            values = [
                previous[at(x - amount)] if -bound <= x - amount <= bound else infinity
                for x in range(-bound, bound + 1)
            ]
        elif operation == 5:
            left = rng.randrange(-4, 5)
            right = left + rng.randrange(5)
            commands.append(f"SHIFT_RANGE {left} {right}")
            previous = values
            values = []
            for x in range(-bound, bound + 1):
                values.append(min(
                    (
                        previous[at(y)]
                        for y in range(x - right, x - left + 1)
                        if -bound <= y <= bound
                    ),
                    default=infinity,
                ))
        elif operation in (6, 7):
            commands.append("PREFIX_MIN" if operation == 6 else "CLEAR_RIGHT")
            for i in range(1, len(values)):
                values[i] = min(values[i], values[i - 1])
        else:
            commands.append("SUFFIX_MIN")
            for i in range(len(values) - 1, 0, -1):
                values[i - 1] = min(values[i - 1], values[i])

        for _ in range(3):
            x = rng.randrange(-200, 201)
            commands.append(f"EVAL {x}")
            output.append(str(values[at(x)]))
        if step % 17 == 0:
            commands.append("MIN")
            output.append(str(min(values)))

    return commands, output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True)
    args = parser.parse_args()
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)

    write_case(out_dir, 0, *hand_case())
    write_case(out_dir, 1, *random_naive_case(random.Random(20260713)))


if __name__ == "__main__":
    main()
