#pragma once
#include <algorithm>
#include <cstdlib>
#include <future>
#include <map>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include <execution>
#include <mutex>

using namespace std::string_literals;

template <typename Key, typename Value>
class ConcurrentMap {
public:
    static_assert(std::is_integral_v<Key>, "ConcurrentMap supports only integer keys"s);
    
    explicit ConcurrentMap(size_t chunk_count) : chunks_(chunk_count) {}

    struct Chunk {
        std::mutex mutex_;
        std::map<Key, Value> chunk_map_;
    };

    struct Access {
        Access(const Key& key, Chunk& chunk) : guard(chunk.mutex_), ref_to_value(chunk.chunk_map_[key]){}
        std::lock_guard<std::mutex> guard;
        Value& ref_to_value;
    };

    Access operator[](const Key& key) {
        return Access{key, chunks_[static_cast<uint64_t>(key) % chunks_.size()]};
    }

    std::map<Key, Value> BuildOrdinaryMap() {
        std::map<Key, Value> result;
        
        std::for_each(std::execution::seq,
                        std::begin(chunks_),
                        std::end(chunks_),
                        [&result](auto& chunk_){
                            std::lock_guard<std::mutex> guard(chunk_.mutex_);
                            result.insert(chunk_.chunk_map_.begin(), chunk_.chunk_map_.end());
                        });
        return result;
    }
    
    auto erase(const Key& key) {
        auto& chunk = chunks_[static_cast<uint64_t>(key) % chunks_.size()];
        std::lock_guard guard(chunk.mutex_);
        return chunk.chunk_map_.erase(key);
    }

private:
    std::vector<Chunk> chunks_;
};