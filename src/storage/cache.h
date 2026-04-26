#pragma once
#include <string>
#include <unordered_map>
#include <list>
class LRUCache {
public:
    explicit LRUCache(size_t capacity) : cap(capacity) {}
    void   put(const std::string& key, const std::string& value);
    bool   get(const std::string& key, std::string& value);
    bool   contains(const std::string& key) const;
    size_t size() const { return cache.size(); }
private:
    size_t cap;
    std::list<std::string> order;
    std::unordered_map<std::string,std::pair<std::string,std::list<std::string>::iterator>> cache;
};
