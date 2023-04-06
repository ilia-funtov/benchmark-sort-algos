#include <algorithm>
#include <chrono>
#include <execution>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

#include <boost/sort/sort.hpp>

#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

using random_engine_type = std::mt19937;

template <typename T>
std::enable_if_t<std::is_integral_v<T>, std::vector<T>>
generate_test_vector(std::size_t size)
{
    std::vector<T> data;
    data.reserve(size);

    const auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    random_engine_type rng(seed);

    std::uniform_int_distribution<T> dist;

    std::generate_n(
        std::back_inserter(data),
        size,
        [&rng, &dist] { return dist(rng); });

    return data;
}

template <typename T>
std::enable_if_t<std::is_floating_point_v<T>, std::vector<T>>
generate_test_vector(std::size_t size)
{
    std::vector<T> data;
    data.reserve(size);

    const auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    random_engine_type rng(seed);

    std::uniform_real_distribution<T> dist;

    std::generate_n(
        std::back_inserter(data),
        size,
        [&rng, &dist] { return dist(rng); });

    return data;
}

template <typename T>
std::enable_if_t<std::is_same_v<T, std::string>, std::vector<T>>
generate_test_vector(std::size_t size)
{
    std::vector<T> data;
    data.reserve(size);

    const auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    random_engine_type rng(seed);

    std::uniform_int_distribution<std::size_t> len_dist(10, 100);
    std::uniform_int_distribution<char> value_dist('a', 'z');

    const auto gen_func = [&rng, &len_dist, &value_dist]
    {
        const auto len = len_dist(rng);
        std::string str;
        str.reserve(len);
        std::generate_n(
            std::back_inserter(str),
            len,
            [&rng, &value_dist] { return value_dist(rng); });

        return str;
    };

    std::generate_n(
        std::back_inserter(data),
        size,
        gen_func);

    return data;
}

TEMPLATE_TEST_CASE(
    "Compare performance of sort algorithms",
    "[vector][template]",
    int, std::int8_t, std::int16_t, float, double, std::string)
{
    const auto size = GENERATE(100U, 1000U, 10000U, 100000U, 1000000U);
    const auto str_size = std::to_string(size);

    // standard library

    BENCHMARK_ADVANCED("std::sort " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&data] { std::sort(std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("std::sort seq " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&data] { std::sort(std::execution::seq, std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("std::sort par " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&data] { std::sort(std::execution::par, std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("std::sort par_unseq " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&data] { std::sort(std::execution::par_unseq, std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("std::stable_sort " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&] { std::stable_sort(std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("std::stable_sort seq " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&] { std::stable_sort(std::execution::seq, std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("std::stable_sort par " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&] { std::stable_sort(std::execution::par, std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("std::stable_sort par_unseq " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&] { std::stable_sort(std::execution::par_unseq, std::begin(data), std::end(data)); });
    };

    // boost single threaded

    BENCHMARK_ADVANCED("boost::sort::spreadsort::spreadsort " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&] { boost::sort::spreadsort::spreadsort(std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("boost::sort::pdqsort " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&] { boost::sort::pdqsort(std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("boost::sort::spinsort " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&] { boost::sort::spinsort(std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("boost::sort::flat_stable_sort " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&] { boost::sort::flat_stable_sort(std::begin(data), std::end(data)); });
    };

    // boost parallel

    BENCHMARK_ADVANCED("boost::sort::block_indirect_sort " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&] { boost::sort::block_indirect_sort(std::begin(data), std::end(data)); });
    };

    BENCHMARK_ADVANCED("boost::sort::sample_sort " + str_size)(Catch::Benchmark::Chronometer meter)
    {
        auto data = generate_test_vector<TestType>(size);
        meter.measure([&] { boost::sort::sample_sort(std::begin(data), std::end(data)); });
    };

    if constexpr (!std::is_same_v<TestType, std::string>)
    {
        // This benchmark fails with SIGSEGV in case with std::string and array size of 1000000  
        BENCHMARK_ADVANCED("boost::sort::parallel_stable_sort " + str_size)(Catch::Benchmark::Chronometer meter)
        {
            auto data = generate_test_vector<TestType>(size);
            meter.measure([&] { boost::sort::parallel_stable_sort(std::begin(data), std::end(data)); });
        };
    }
}

int main(int argc, char* argv[])
{
    return Catch::Session().run(argc, argv);
}