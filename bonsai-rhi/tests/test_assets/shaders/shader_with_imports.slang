import test_module;

[shader("compute")]
[numthreads(1, 1, 1)]
void CSMain() {
    int c = add(1, 2);
}
