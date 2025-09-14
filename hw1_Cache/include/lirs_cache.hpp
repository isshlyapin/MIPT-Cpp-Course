#include <iterator>
#include <list>
#include <utility>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include<unordered_map>

template<typename KeyT>
class LirsStack;

enum class LirsType : std::uint8_t {
    LIR = 0,
    HIR = 1
};

template <typename PageT, typename KeyT = int>
class LirsCache {
public:
    using CacheList = typename std::list<std::pair<KeyT, PageT>>;
    using CacheIt = typename CacheList::iterator;

    LirsCache(size_t sz, double part_lir=0.9) : lirsStack_(sz * 2) {
        assert(sz > 1);
        sz_hot_ = static_cast<size_t>(sz * part_lir);
        sz_cold_ = sz - sz_hot_;

        if (sz_cold_ == 0) {
            sz_cold_++;
            sz_hot_--;
        }

        if (sz_hot_ == 0) {
            sz_hot_++;
            sz_cold_--;
        }
    };

    template <typename F>
    bool lookup_update(KeyT key, F get_page) {
        bool result = false;
        if (hotHash_.contains(key)) {
            lirsStack_.move_to_front(key);
            if (lirsStack_.bottom().second == LirsType::HIR) {
                lirsStack_.pruning();                
            }            
            result = true;
        } else if (coldHash_.contains(key)) {
            if (lirsStack_.contains(key)) {
                lirsStack_.move_to_front(key);
                lirsStack_.update_type(key, LirsType::LIR);

                auto last_lir = lirsStack_.bottom();
                lirsStack_.update_type(last_lir.first, LirsType::HIR);
                lirsStack_.pruning();

                raise_hir_to_lir(key, last_lir.first);
            } else {
                lirsStack_.push_front(key, LirsType::HIR);

                auto it = coldHash_.find(key)->second;
                coldCache_.splice(coldCache_.end(), coldCache_, it);
            }

            result = true;
        } else {
            if (hotCache_.size() < sz_hot_) {
                hotCache_.push_back({key, get_page(key)});
                hotHash_.insert({key, std::prev(hotCache_.end())});
                lirsStack_.push_front(key, LirsType::LIR);
            } else if (coldCache_.size() < sz_cold_) {
                coldCache_.push_back({key, get_page(key)});
                coldHash_.insert({key, std::prev(coldCache_.end())});
                lirsStack_.push_front(key, LirsType::HIR);
            } else {
                coldHash_.erase(coldCache_.front().first);
                coldCache_.pop_front();
                if (lirsStack_.contains(key)) {
                    lirsStack_.move_to_front(key);
                    lirsStack_.update_type(key, LirsType::LIR);

                    auto last_lir = lirsStack_.bottom();
                    lirsStack_.update_type(last_lir.first, LirsType::HIR);
                    lirsStack_.pruning();

                    raise_hir_to_lir(key, last_lir.first);
                } else {
                    lirsStack_.push_front(key, LirsType::HIR);
                    coldCache_.push_back({key, get_page(key)});
                    coldHash_.insert({key, std::prev(coldCache_.end())});
                }
            }
            result = false;
        }

        return result;
    }

    void raise_hir_to_lir(KeyT key_h, KeyT key_l) {
        auto it_h = coldHash_.find(key_h)->second;
        auto it_l = hotHash_.find(key_l)->second;
        
        hotCache_.push_back(*it_h);
        coldCache_.push_back(*it_l);
        
        hotCache_.erase(it_l);
        hotHash_.erase(key_l);

        coldCache_.erase(it_h);
        coldHash_.erase(key_h);

        hotHash_.insert({key_h, std::prev(hotCache_.end())});
        coldHash_.insert({key_l, std::prev(coldCache_.end())});
    }

private:
    size_t sz_hot_;
    size_t sz_cold_;

    LirsStack<KeyT> lirsStack_;

    CacheList hotCache_;
    std::unordered_map<KeyT, CacheIt> hotHash_;

    CacheList coldCache_;
    std::unordered_map<KeyT, CacheIt> coldHash_;
};

template <typename KeyT>
class LirsStack {
public:
    using LirsStackIt = typename std::list<std::pair<KeyT, LirsType>>::iterator;

    LirsStack(size_t sz): sz_(sz) {}

    void pop_back() {
        lirsStackHash_.erase(lirsStack_.back().first);
        lirsStack_.pop_back();
    }

    size_t size() const {
        return lirsStack_.size();
    }

    void push_front(KeyT key, LirsType type) {
        if (size() == sz_) { pop_back(); }
        lirsStack_.push_front({key, type});
        auto res = lirsStackHash_.insert({key, lirsStack_.begin()});
        assert(res.second == true);
    }

    void push_back(KeyT key, LirsType type) {
        lirsStack_.push_back({key, type});
        auto res = lirsStackHash_.insert({key, std::prev(lirsStack_.end())});
        assert(res.second == false);
    }

    bool contains(KeyT key) const {
        return lirsStackHash_.contains(key);
    }

    void update_type(KeyT key, LirsType newType) {
        auto it = lirsStackHash_.find(key);
        it->second->second = newType;
    }

    void move_to_front(KeyT key) {
        auto it = lirsStackHash_.find(key)->second;
        lirsStack_.splice(lirsStack_.begin(), lirsStack_, it);
    }

    std::pair<KeyT, LirsType> top() const {
        return lirsStack_.front();
    }

    std::pair<KeyT, LirsType> bottom() const {
        return lirsStack_.back();
    }

    void pruning() {
        assert(!lirsStack_.empty());
        auto lastElement = bottom();
        while (lastElement.second == LirsType::HIR) {
            lirsStackHash_.erase(lastElement.first);
            lirsStack_.pop_back();

            lastElement = bottom();
        }
    }
private:
    size_t sz_;
    std::list<std::pair<KeyT, LirsType>> lirsStack_;
    std::unordered_map<KeyT, LirsStackIt> lirsStackHash_;
};