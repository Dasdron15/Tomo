# comment
import math

class C:
    CONST = 42
    def f(self, x):
        if x > 1:
            print("hi")
        else:
            for i in range(3):
                print(i)
        return x + 1

def g(a: int) -> int:
    return a * C.CONST

val = g(3)
s = f"val={val}"
nums = [n*n for n in range(5)]
