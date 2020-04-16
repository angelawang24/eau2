//lang::c++
#pragma once

#include <unordered_map> 
#include <mutex>
using namespace std;

// authors: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu

// A key consists of a string and a home node.
mutex waitNGetLock;
class Key {
public:
    string key;
    size_t node;

    // Constructor
    Key(string k, size_t n) {
        key = k;
        node = n;
    }

    // Destructor
    ~Key() { }

    // Match both key and node value 
    bool operator==(const Key& k) const
    { 
        return key.compare(k.key) == 0 && node == k.node; 
    }
 
    /** Return the raw char*. The result should not be modified or freed. */
    char* c_str() {  
        string entire_key = key + to_string(node);
        char* char_array = new char[entire_key.length() + 1];
        strcpy(char_array, entire_key.c_str()); 

        return char_array; 
    }

    size_t home() {
        return node;
    }  
};

// Overwrote so that find works with custom Key class
namespace std {
  template <>
  struct hash<Key>
  {
    std::size_t operator()(const Key& k) const
    {
      return 1;
    }
  };
}

/** A key buffer builds a key from various pieces.
 *  author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu */
class KeyBuff : public Object {                                                  
public:                                                                        
    Key* orig_; // external                                                                                                                       
                                                                                    
    KeyBuff(Key* orig) : orig_(orig) { } 

    ~KeyBuff() {
        delete orig_;
    }                                                  
    Key* get() {                                                                   
        char* kchar = orig_->c_str();                                                    
        Key* k = new Key(string(kchar), orig_->home());   
        delete [] kchar;                                                                                              
        return k;                                                                    
    }                                                                              
}; // KeyBuff  

// Represents a store for a singular node. Contains a map of Key to serialized blob (char*)
class KeyValueStore : public Object {
public:
    size_t node_idx; // for future reference
    // key = key consists of a string and a home node
    // value = char* serailized blob
    unordered_map<Key, char*> umap;

    //Default Constructor
    KeyValueStore() {
        node_idx = 0;
    }

    // Constructor
    KeyValueStore(size_t n) {
        node_idx = n;
    }

    // Destructor
    ~KeyValueStore() {
        unordered_map<Key, char*>::iterator it = umap.begin();
        // Iterate over the map using iterator
        while(it != umap.end())
        {
            delete [] it->second;
            it++;
        }
    }

    // Puts serialized blob with the corresponding key in the map
    // Overwrites if key already exists
    void put(Key k, char* v) {
        waitNGetLock.lock();
        umap[k] = v; // overwrite if key already exists
        waitNGetLock.unlock();
    }
    
    // Returns serialized blob that corresponds to given key
    char* get(Key k) {     
        assert(umap.find(k) != umap.end());
        // If key found then iterator to that key is returned 
        return umap.find(k)->second; // first=key, second=value
    }

    // Returns serialized blob the corresponds to given key
    char* waitAndGet(Key k) {
        bool is_found = false;
        while (!is_found) {
            // keep trying, blocked otherwise
            waitNGetLock.lock();
            if (umap.find(k) != umap.end()) { is_found = true; }
            waitNGetLock.unlock();
        }
        // If key found then iterator to that key is returned 
        return umap.find(k)->second; // first=key, second=value
    }
};

