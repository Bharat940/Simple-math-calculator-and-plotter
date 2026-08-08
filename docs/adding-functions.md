# Adding a New Math Function to MathStudio (v1.x)

Follow this step-by-step guide to register a new single-argument or multi-argument mathematical function in v1.x:

## 1. Register in `src/math/functions.cpp`

Add your function implementation to the built-in function map:

```cpp
// In src/math/functions.cpp:
{"myfunc", [](double x) {
    if (x < 0) throw std::domain_error("myfunc domain error");
    return std::log(x) + 1.0;
}}
```

## 2. Update Expression Validator Whitelist

Ensure the function name characters are accepted by `validateExpression()` in `src/main.cpp`.

## 3. Add Unit Tests

Add test cases in `tests/test_math.cpp` verifying accuracy and domain boundary handling:

```cpp
TEST(FunctionTest, MyFunc) {
    Expression e("myfunc(1)");
    EXPECT_NEAR(e.eval(0), 1.0, 1e-9);
}
```

## 4. Run Build & Verify

```bash
cmake --build build
./build/tests
```
