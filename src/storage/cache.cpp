#include "cache.h"
void LRUCache::put(const std::string& k,const std::string& v){
    auto it=cache.find(k); if(it!=cache.end()){order.erase(it->second.second);cache.erase(it);}
    order.push_front(k); cache[k]={v,order.begin()};
    if(cache.size()>cap){cache.erase(order.back());order.pop_back();}
}
bool LRUCache::get(const std::string& k,std::string& v){
    auto it=cache.find(k); if(it==cache.end())return false;
    order.erase(it->second.second); order.push_front(k);
    it->second.second=order.begin(); v=it->second.first; return true;
}
bool LRUCache::contains(const std::string& k)const{ return cache.count(k)>0; }
