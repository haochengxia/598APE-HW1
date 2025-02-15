#pragma once

#ifdef ENABLE_MPOOL
class SimplePool {
private:
    static const int POOL_SIZE = 10000;  // 固定大小
    char* memory;                        // 内存块
    int used;                           // 已使用数量
    
public:
    SimplePool() : used(0) {
        memory = new char[POOL_SIZE];
    }
    
    ~SimplePool() {
        delete[] memory;
    }
    
    void* alloc(size_t size) {
        if (used + size > POOL_SIZE) return nullptr;
        void* ptr = memory + used;
        used += size;
        return ptr;
    }
};
#endif 