#include "light.h"
#include "shape.h"
#include "camera.h"
#include <vector>
#include <functional>

Light::Light(const Vector & cente, unsigned char* colo) : center(cente){
   color = colo;
}

unsigned char* Light::getColor(unsigned char a, unsigned char b, unsigned char c){
   unsigned char* r = (unsigned char*)malloc(sizeof(unsigned char)*3);
   r[0] = a;
   r[1] = b;
   r[2] = c;
   return r;
}

Autonoma::Autonoma(const Camera& c): camera(c){
   listStart = NULL;
   listEnd = NULL;
   lightStart = NULL;
   lightEnd = NULL;
   depth = 10;
   skybox = BLACK;
}

Autonoma::Autonoma(const Camera& c, Texture* tex): camera(c){
   listStart = NULL;
   listEnd = NULL;
   lightStart = NULL;
   lightEnd = NULL;
   depth = 10;
   skybox = tex;
}

#ifdef ENABLE_MPOOL
SimplePool Autonoma::nodePool;  // 定义静态内存池

// 辅助函数，用于监控内存池使用情况
void printPoolStats() {
    size_t used = Autonoma::nodePool.getUsed();
    size_t capacity = Autonoma::nodePool.getCapacity();
    float frag = Autonoma::nodePool.getFragmentation();
    printf("Memory pool stats:\n");
    printf("- Used: %zu bytes\n", used);
    printf("- Capacity: %zu bytes (%.2f%%)\n", capacity, (float)used/capacity*100);
    printf("- Fragmentation: %.2f%%\n", frag * 100);
}

// 辅助函数，在需要时进行内存压缩
void checkAndCompact() {
    if (Autonoma::nodePool.getFragmentation() > 0.3) {  // 碎片率超过30%时压缩
        printf("High fragmentation detected, compacting memory pool...\n");
        Autonoma::nodePool.compact();
        printPoolStats();
    }
}
#endif

void Autonoma::addShape(Shape* r) {
#ifdef ENABLE_MPOOL
    ShapeNode* hi = static_cast<ShapeNode*>(nodePool.alloc(sizeof(ShapeNode)));
    if (nodePool.getUsed() > nodePool.getCapacity() * 0.8) {  // 使用超过80%时
        printf("Warning: Memory pool usage high\n");
        printPoolStats();
        checkAndCompact();  // 尝试压缩内存
    }
#else
    ShapeNode* hi = (ShapeNode*)malloc(sizeof(ShapeNode));
#endif
    hi->data = r;
    hi->next = hi->prev = NULL;
    if(listStart==NULL) {
        listStart = listEnd = hi;
    } else {
        listEnd->next = hi;
        hi->prev = listEnd;
        listEnd = hi;
    }
}

void Autonoma::removeShape(ShapeNode* s) {
    if(s==listStart) {
        if(s==listEnd) {
            listStart = listStart = NULL;
        } else {
            listStart = s->next;
            listStart->prev = NULL;
        }
    } else if(s==listEnd) {
        listEnd = s->prev;
        listEnd->next = NULL;
    } else {
        ShapeNode *b4 = s->prev, *aft = s->next;
        b4->next = aft;
        aft->prev = b4;
    }
#ifdef ENABLE_MPOOL
    nodePool.dealloc(s);
    if (nodePool.getFragmentation() > 0.3) {  // 删除操作后检查碎片率
        checkAndCompact();
    }
#else
    free(s);
#endif
}

void Autonoma::addLight(Light* r) {
#ifdef ENABLE_MPOOL
    LightNode* hi = static_cast<LightNode*>(nodePool.alloc(sizeof(LightNode)));
    if (nodePool.getUsed() > nodePool.getCapacity() * 0.8) {
        printf("Warning: Memory pool usage high\n");
        printPoolStats();
        checkAndCompact();
    }
#else
    LightNode* hi = (LightNode*)malloc(sizeof(LightNode));
#endif
    hi->data = r;
    hi->next = hi->prev = NULL;
    if(lightStart==NULL) {
        lightStart = lightEnd = hi;
    } else {
        lightEnd->next = hi;
        hi->prev = lightEnd;
        lightEnd = hi;
    }
}

void Autonoma::removeLight(LightNode* s) {
    if(s==lightStart) {
        if(s==lightEnd) {
            lightStart = lightStart = NULL;
        } else {
            lightStart = s->next;
            lightStart->prev = NULL;
        }
    } else if(s==lightEnd) {
        lightEnd = s->prev;
        lightEnd->next = NULL;
    } else {
        LightNode *b4 = s->prev, *aft = s->next;
        b4->next = aft;
        aft->prev = b4;
    }
#ifdef ENABLE_MPOOL
    nodePool.dealloc(s);
    if (nodePool.getFragmentation() > 0.3) {
        checkAndCompact();
    }
#else
    free(s);
#endif
}

void getLight(double* tColor, Autonoma* aut, Vector point, Vector norm, unsigned char flip){
   tColor[0] = tColor[1] = tColor[2] = 0.;
   LightNode *t = aut->lightStart;
   while(t!=NULL){
      double lightColor[3];     
      lightColor[0] = t->data->color[0]/255.;
      lightColor[1] = t->data->color[1]/255.;
      lightColor[2] = t->data->color[2]/255.;
      Vector ra = t->data->center-point;
      ShapeNode* shapeIter = aut->listStart;
      bool hit = false;
      while(!hit && shapeIter!=NULL){
        hit = shapeIter->data->getLightIntersection(Ray(point+ra*.01, ra), lightColor);
         shapeIter = shapeIter->next;
      }
      double perc = (norm.dot(ra)/(ra.mag()*norm.mag()));
      if(!hit){
      if(flip && perc<0) perc=-perc;
        if(perc>0){
      
         tColor[0]+= perc*(lightColor[0]);
         tColor[1]+= perc*(lightColor[0]);
         tColor[2]+= perc*(lightColor[0]);
         if(tColor[0]>1.) tColor[0] = 1.;
         if(tColor[1]>1.) tColor[1] = 1.;
         if(tColor[2]>1.) tColor[2] = 1.;
        }
      }
      t =t->next;
   }
}

#ifdef ENABLE_BVH
void Autonoma::buildBVH() {
    if (!listStart) return;
    
    std::vector<ShapeNode*> nodes;
    for(ShapeNode* node = listStart; node != nullptr; node = node->next) {
        nodes.push_back(node);
    }
    
    if (nodes.size() < 10) return;
    
    // 递归构建函数
    std::function<BVHNode*(std::vector<ShapeNode*>&)> build = 
        [](std::vector<ShapeNode*>& shapes) -> BVHNode* {
        if (shapes.empty()) return nullptr;
        if (shapes.size() <= 4) {  // 少量物体直接作为叶子节点
            BVHNode* leaf = new BVHNode();
            leaf->shapes = shapes;
            
            // 计算叶子节点的包围盒
            for (auto node : shapes) {
                Vector pos = node->shape->position;
                leaf->bounds.min = Vector::min(leaf->bounds.min, pos - 1);  // 简单地扩展1个单位
                leaf->bounds.max = Vector::max(leaf->bounds.max, pos + 1);
            }
            return leaf;
        }
        
        BVHNode* node = new BVHNode();
        
        // 简单地按中点分割
        auto mid = shapes.size() / 2;
        std::vector<ShapeNode*> left_shapes(shapes.begin(), shapes.begin() + mid);
        std::vector<ShapeNode*> right_shapes(shapes.begin() + mid, shapes.end());
        
        // 递归构建子树
        node->left = build(left_shapes);
        node->right = build(right_shapes);
        
        // 合并子节点的包围盒
        if (node->left) {
            node->bounds.min = Vector::min(node->bounds.min, node->left->bounds.min);
            node->bounds.max = Vector::max(node->bounds.max, node->left->bounds.max);
        }
        if (node->right) {
            node->bounds.min = Vector::min(node->bounds.min, node->right->bounds.min);
            node->bounds.max = Vector::max(node->bounds.max, node->right->bounds.max);
        }
        
        return node;
    };
    
    bvh_root = build(nodes);
}

Shape* Autonoma::getIntersection(Ray r, double& time) {
    #ifdef ENABLE_BVH
    if (bvh_root) {
        time = INFINITY;
        
        // 递归遍历函数
        std::function<Shape*(BVHNode*, double&)> traverse = 
            [&](BVHNode* node, double& min_time) -> Shape* {
            if (!node || !node->bounds.intersect(r)) return nullptr;
            
            // 叶子节点
            if (!node->shapes.empty()) {
                Shape* result = nullptr;
                for (auto shape_node : node->shapes) {
                    double t = shape_node->shape->getIntersection(r);
                    if (t < min_time) {
                        min_time = t;
                        result = shape_node->shape;
                    }
                }
                return result;
            }
            
            // 内部节点：递归遍历
            Shape* left_hit = traverse(node->left, min_time);
            Shape* right_hit = traverse(node->right, min_time);
            return (min_time == INFINITY) ? nullptr : (left_hit ? left_hit : right_hit);
        };
        
        return traverse(bvh_root, time);
    }
    #endif

    // 原始的线性搜索
    Shape* result = nullptr;
    for(ShapeNode* node = listStart; node != nullptr; node = node->next) {
        double t = node->shape->getIntersection(r);
        if(t < time) {
            time = t;
            result = node->shape;
        }
    }
    return result;
}
#endif

