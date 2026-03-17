# Bridge.provide vs Bridge.provide_safe vs k_mutex — Race Condition Demonstration (UNO-Q)

## Overview

This project demonstrates a race condition when using `Bridge.provide()` on Arduino UNO-Q, and how it can be resolved using either:

- a Zephyr mutex (`k_mutex`) (RTOS-level solution)

---

## What is demonstrated

A callback (`updateCounter`) updates two shared variables:

- `counter`
- `doubleValue = counter * 2`

An artificial delay is introduced between the two assignments:

```cpp
counter = x;
delay(5);
doubleValue = x * 2;
```

At the same time, `loop()` reads these variables.

---

## Problem: race condition

With:

```cpp
Bridge.provide("update", updateCounter);
```

the callback runs concurrently with `loop()`.

This may lead to inconsistent reads such as:

```
ERROR inconsistent state: counter=19 double=36
```

This happens because `loop()` reads the variables while they are being updated.

---

## Solution 1 — provide_safe()

Replace:

```cpp
Bridge.provide("update", updateCounter);
```

with:

```cpp
Bridge.provide_safe("update", updateCounter);
```

Result:

- the callback is executed in the same context as `loop()`
- no concurrent access
- no inconsistent state

👉 “Check the link”  
[UNO-Q-bridge-provide-safe-vs-provide](https://github.com/philippe86220/UNO-Q-bridge-provide-safe-vs-provide)

---

## Solution 2 — Zephyr mutex

Keep `Bridge.provide()` and protect the shared data:

```cpp
k_mutex_lock(&data_mutex, K_FOREVER);

counter = x;
delay(5);
doubleValue = x * 2;

k_mutex_unlock(&data_mutex);
```

Also protect reads in `loop()`:

```cpp
k_mutex_lock(&data_mutex, K_FOREVER);
int c = counter;
int d = doubleValue;
k_mutex_unlock(&data_mutex);
```

Result:

- concurrent execution is still allowed
- but access is synchronized
- no inconsistent state

---

## Key idea

Race condition = unsynchronized access to shared data

Solutions:

- provide_safe() → implicit serialization
- mutex → explicit synchronization

---

## Conclusion

The issue is not Bridge itself, but the lack of synchronization.

Both approaches solve the same problem at different levels:

| Method            | Level        | Complexity |
|------------------|-------------|-----------|
| provide_safe()   | high-level  | simple    |
| k_mutex (Zephyr) | RTOS-level  | advanced  |

---

## How to reproduce

1. Run the sketch with:

```
Bridge.provide("update", updateCounter);
```

→ observe inconsistent state

2. Replace with:

```
Bridge.provide_safe("update", updateCounter);
```

👉 “Check the link”  
[UNO-Q-bridge-provide-safe-vs-provide](https://github.com/philippe86220/UNO-Q-bridge-provide-safe-vs-provide)

→ errors disappear

3. Alternatively, keep `provide()` and add a mutex

→ errors disappear

---

## Acknowledgments

This project benefited from discussions and technical assistance provided by ChatGPT (OpenAI), used as a learning and development tool.
