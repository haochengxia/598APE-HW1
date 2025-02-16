#pragma once

#ifdef ENABLE_MPOOL
class SimplePool {
private:
    static const int INITIAL_SIZE = 100000;  // 增大初始大小
    static const int GROWTH_FACTOR = 2;      // 增长因子
    
    struct Block {
        char* data;
        bool used;
        size_t size;
        Block* next;
        Block(size_t s) : size(s), used(true), next(nullptr) {
            data = new char[s];
        }
        ~Block() { delete[] data; }
    };
    
    Block* firstBlock;
    size_t totalCapacity;
    size_t totalUsed;
    
public:
    SimplePool() : totalCapacity(0), totalUsed(0), firstBlock(nullptr) {}
    
    ~SimplePool() {
        while (firstBlock) {
            Block* next = firstBlock->next;
            delete firstBlock;
            firstBlock = next;
        }
    }
    
    void* alloc(size_t size) {
        // 查找可用块
        Block* current = firstBlock;
        while (current) {
            if (!current->used && current->size >= size) {
                current->used = true;
                totalUsed += size;
                return current->data;
            }
            current = current->next;
        }
        
        // 没有找到合适的块，创建新块
        Block* newBlock = new Block(size);
        totalCapacity += size;
        totalUsed += size;
        
        // 添加到链表头部
        newBlock->next = firstBlock;
        firstBlock = newBlock;
        
        return newBlock->data;
    }
    
    void* alloc() {
        return alloc(sizeof(void*));
    }
    
    void dealloc(void* ptr) {
        Block* current = firstBlock;
        while (current) {
            if (current->data == ptr) {
                current->used = false;
                totalUsed -= current->size;
                return;
            }
            current = current->next;
        }
    }
    
    void free(void* ptr) {
        dealloc(ptr);
    }
    
    void reset() {
        Block* current = firstBlock;
        while (current) {
            current->used = false;
            current = current->next;
        }
        totalUsed = 0;
    }
    
    // 合并相邻的空闲块
    void compact() {
        if (!firstBlock) return;
        
        Block* current = firstBlock;
        while (current && current->next) {
            if (!current->used && !current->next->used) {
                // 合并相邻的空闲块
                Block* toDelete = current->next;
                current->size += toDelete->size;
                current->next = toDelete->next;
                delete toDelete;
            } else {
                current = current->next;
            }
        }
    }
    
    size_t getUsed() const { return totalUsed; }
    size_t getCapacity() const { return totalCapacity; }
    
    // 获取碎片率
    float getFragmentation() const {
        size_t freeBlocks = 0;
        Block* current = firstBlock;
        while (current) {
            if (!current->used) freeBlocks++;
            current = current->next;
        }
        return freeBlocks > 0 ? (float)freeBlocks / getTotalBlocks() : 0;
    }
    
private:
    size_t getTotalBlocks() const {
        size_t count = 0;
        Block* current = firstBlock;
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }
};
#endif 
