#include <algorithm>
#include <execution>
#include <iostream>
#include <future>

using namespace std;

#define PROFILE_CONCAT_INTERNAL(X, Y) X##Y
#define PROFILE_CONCAT(X, Y) PROFILE_CONCAT_INTERNAL(X, Y)
#define UNIQUE_VAR_NAME_PROFILE PROFILE_CONCAT(profileGuard, __LINE__)
#define LOG_DURATION(x) LogDuration UNIQUE_VAR_NAME_PROFILE(x)

class LogDuration {
public:
    // заменим имя типа std::chrono::steady_clock
    // с помощью using для удобства
    using Clock = std::chrono::steady_clock;

    LogDuration(const std::string& id) : id_(id) {
    }

    ~LogDuration() {
        using namespace std::chrono;
        using namespace std::literals;

        const auto end_time = Clock::now();
        const auto dur = end_time - start_time_;
        std::cerr << id_ << ": "s << duration_cast<nanoseconds>(dur).count() << " ms"s << std::endl;
    }

private:
    const std::string id_;
    const Clock::time_point start_time_ = Clock::now();
};

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(const execution::sequenced_policy&,
                                RandomAccessIterator range_begin, RandomAccessIterator range_end,
                                const Value& value) {
    auto left_bound = range_begin;
    auto right_bound = range_end;
    while (left_bound + 1 < right_bound) {
        const auto middle = left_bound + (right_bound - left_bound) / 2;
        if (*middle < value) {
            left_bound = middle;
        } else {
            right_bound = middle;
        }
    }
    if (left_bound == range_begin && !(*left_bound < value)) {
        return left_bound;
    } else {
        return right_bound;
    }
}

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(const execution::parallel_policy&,
                                RandomAccessIterator range_begin, RandomAccessIterator range_end,
                                const Value& value) {
    auto left_bound = range_begin;
    auto right_bound = range_end;

    while (left_bound + 1 < right_bound) {
        bool report_first_ = false;
        bool report_second_ = false;

        const auto middle_left = left_bound + (right_bound - left_bound) / 3;
        const auto middle_right = right_bound - (right_bound - left_bound) / 3;

        future<bool> first_ = async( [&value, middle_left] { return (*middle_left < value); });
        future<bool> second_ = async( [&value, middle_right] { return (*middle_right < value); });
        
        if (value < *left_bound) {
            return left_bound;
        }
        
        report_first_ = first_.get();
        report_second_ = second_.get();
        
        if (report_first_) {
            //return LowerBound(std::execution::par, middle_right, right_bound, value);
            left_bound = middle_right;
        } else if (report_second_) {
            //return LowerBound(std::execution::par, middle_left, middle_right, value);
            left_bound = middle_left;
            right_bound = middle_right;
        } else {
            //return LowerBound(std::execution::par, left_bound, middle_left, value);
            right_bound = middle_left;
        }
    }

    if (!(*left_bound < value)) {
        return left_bound;
    } else {
        return right_bound;
    }
    
}

template <typename RandomAccessIterator, typename Value>
RandomAccessIterator LowerBound(RandomAccessIterator range_begin, RandomAccessIterator range_end,
                                const Value& value) {
    return LowerBound(execution::seq, range_begin, range_end, value);
}

int main() {
    const vector<string> strings = {"cat", "dog", "dog", "dog", "horse"};

    const vector<string> requests = {"bear", "cat", "deer", "dog", "dogs", "horses"};
    {
        LOG_DURATION("seq"s);

        // последовательные версии
        cout << "Request [" << requests[0] << "] → position "
             << LowerBound(strings.begin(), strings.end(), requests[0]) - strings.begin() << endl;
        cout << "Request [" << requests[1] << "] → position "
             << LowerBound(execution::seq, strings.begin(), strings.end(), requests[1])
                - strings.begin()
             << endl;
        cout << "Request [" << requests[2] << "] → position "
             << LowerBound(execution::seq, strings.begin(), strings.end(), requests[2])
                - strings.begin()
             << endl;
    }
    
    {
        LOG_DURATION("par"s);
        // параллельные
        cout << "Request [" << requests[0] << "] → position "
             << LowerBound(execution::par, strings.begin(), strings.end(), requests[0])
                - strings.begin() << endl;
        cout << "Request [" << requests[1] << "] → position "
             << LowerBound(execution::par, strings.begin(), strings.end(), requests[1])
                - strings.begin() << endl;
        cout << "Request [" << requests[2] << "] → position "
             << LowerBound(execution::par, strings.begin(), strings.end(), requests[2])
                - strings.begin() << endl;
    }
}