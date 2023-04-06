# benchmark-sort-algos
Benchmarking C++ sorting algorithms from STL and Boost libraries with [Catch2](https://github.com/catchorg/Catch2/tree/v2.13.10) test framework.

Next algorithms are measured:
- from STL:
    1) std::sort
    2) std::sort with ExecutionPolicy (since C++17)
    3) std::stable_sort
    3) std::stable_sort with ExecutionPolicy (since C++17)
- from Boost libraries:
    1) boost::sort::spreadsort::spreadsort
    2) boost::sort::pdqsort
    3) boost::sort::spinsort
    4) boost::sort::flat_stable_sort
    5) boost::sort::block_indirect_sort
    6) boost::sort::sample_sort
    7) boost::sort::parallel_stable_sort
