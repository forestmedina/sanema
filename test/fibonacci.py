import time
def fib(n):
    if n < 2:
        return n
    else:
        return fib(n - 1) + fib(n - 2)

start = time.perf_counter()
fib(40)
total=(time.perf_counter() - start)*1000
print(f"Completed Execution in {total} miliseconds")