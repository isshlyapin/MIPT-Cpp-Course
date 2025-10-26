#pragma once

#include <vector>
#include <cassert>
#include <stdexcept>
#include <functional>
#include <unordered_map>

namespace myds {

template<typename KeyT, typename ValueT, typename CompT = std::less<KeyT>>
class ThreadedBinaryTree {
private:
  struct Node {
    Node(
      KeyT key, 
      ValueT value, 
      Node* left = nullptr, 
      Node* right = nullptr, 
      Node* parent = nullptr, 
      bool left_th = true, bool right_th = true
    ) : key(key), value(value), height(1), 
        left(left), right(right), parent(parent), 
        left_th(left_th), right_th(right_th) {}

    KeyT key; 
    ValueT value;

    int height;

    Node* left;
    Node* right;
    Node* parent;
    bool left_th, right_th;
  };

public:
  // Inorder обход с вызовом функции visit для каждого узла
  template<typename F>
  void inorder(F visit) {
    Node* node = left_most(root_);
    while (node != nullptr) {
      visit(node);
      if (node->right_th) { 
        node = node->right; 
      } else {
        node = left_most(node->right);
      }
    }
  }

  template<typename F>
  void inorder(F visit) const {
    const Node* node = left_most(root_);
    while (node != nullptr) {
      visit(node);
      if (node->right_th) {
        node = node->right;
      }
      else {
        node = left_most(node->right);
      }
    }
  }

  // Поиск самого левого узла относительно заданного
  const Node* left_most(const Node* root) const {
    if (root == nullptr) { return nullptr; }
    const Node* res = root;
    while (!res->left_th) {
      res = res->left;
    }
    return res;
  }

  // Поиск самого левого узла относительно заданного
  Node* left_most(Node* root) {
    if (root == nullptr) { return nullptr; }
    Node* res = root;
    while (!res->left_th) {
      res = res->left;
    }
    return res;
  }

  // Поиск самого правого узла относительно заданного
  const Node* right_most(const Node* root) const {
    if (root == nullptr) { return nullptr; }
    const Node* res = root;
    while (!res->right_th) {
      res = res->right;
    }
    return res;
  }

  // Поиск самого правого узла относительно заданного
  Node* right_most(Node* root) {
    if (root == nullptr) { return nullptr; }
    Node* res = root;
    while (!res->right_th) {
      res = res->right;
    }
    return res;
  }

  // Поиск элемента по ключу
  Node* find(const KeyT& key) {
    Node* cur_node = root_;
    while (cur_node != nullptr) {
      if (comp_(key, cur_node->key)) {
        if (cur_node->left_th) { return nullptr; }
        cur_node = cur_node->left;
      } else if (comp_(cur_node->key, key)) {
        if (cur_node->right_th) { return nullptr; }
        cur_node = cur_node->right;
      } else {
        return cur_node;
      }
    }

    return nullptr;
  }

  int height(const Node* node) {
    return (node == nullptr) ? 0 : node->height; 
  }

  int bfactor(const Node* node) {
    return height(right_ptr(node)) - height(left_ptr(node));    
  }

  void fixheight(Node* node) {
    int hl = height(left_ptr(node));
    int hr = height(right_ptr(node));
    node->height = std::max(hl, hr) + 1;
  }

  Node* left_ptr(const Node* node) {
    if (node == nullptr) {
      throw std::invalid_argument("The pointer cannot be nullptr");
    }
    return node->left_th ? nullptr : node->left;
  }

  Node* right_ptr(const Node* node) {
    if (node == nullptr) {
      throw std::invalid_argument("The pointer cannot be nullptr");
    }
    return node->right_th ? nullptr : node->right;
  }

  Node* rotate_right(Node* node) {
    if (node == nullptr) { return nullptr; }

    assert(!node->left_th);
    Node* lnode = node->left;

    assert(!lnode->right_th);
    lnode->right->parent = node;
    lnode->parent = node->parent;
    node->parent = lnode;
    node->left = lnode->right;
    lnode->right = node;
    fixheight(node);
    fixheight(lnode);
    return lnode; 
  }

  Node* rotate_left(Node* node) {
    if (node == nullptr) { return nullptr; }

    assert(!node->right_th);
    Node* rnode = node->right;

    assert(!rnode->left_th);
    rnode->left->parent = node;
    rnode->parent = node->parent;
    node->parent = rnode;
    node->right = rnode->left;
    rnode->left = node;
    fixheight(node);
    fixheight(rnode);
    return rnode;
  }

  Node* balance(Node* p) {
    fixheight(p);
    if (bfactor(p) == 2) {
      if (bfactor(p->right) < 0) {
        p->right = rotate_right(p->right);
      }
      return rotate_left(p);
    }
    if (bfactor(p) == -2) {
      if( bfactor(p->left) > 0) {
        p->left = rotate_left(p->left);
      }
      return rotate_right(p);
    }
    
    return p;
  }

  // Вставка нового узла в дерево
  bool insert(const KeyT& new_key, const ValueT& new_value) {
    if (root_ == nullptr) {
      root_ = new Node{new_key, new_value};
      return true;
    }
    
    Node* cur_node = root_;
    Node* last_left_step  = nullptr;
    Node* last_right_step = nullptr;
    
    while (cur_node != nullptr) {
      if (comp_(new_key, cur_node->key)) {
        if (cur_node->left_th) {
          cur_node->left = new Node{new_key, new_value, last_right_step, cur_node, cur_node};
          cur_node->left_th = false;
          break;
        } else {
          last_left_step = cur_node;
          cur_node = cur_node->left;
        }
      } else if (comp_(cur_node->key, new_key)) {
        if (cur_node->right_th) {
          cur_node->right = new Node{new_key, new_value, cur_node, last_left_step, cur_node};
          cur_node->right_th = false;
          break;
        } else {
          last_right_step = cur_node;
          cur_node = cur_node->right;
        }
      } else {
        return false;
      }
    }

    fix_balance_up(cur_node);
    
    return true;
  }

  Node* extract_min(Node* node) {
    if (node == nullptr) {
      throw std::invalid_argument("The pointer cannot be nullptr");
    }

    Node* mnode = left_most(node);
    if (mnode->right_th) {
      mnode->parent->left = mnode->left;
      mnode->parent->left_th = true;
    } else {
      mnode->right->left = mnode->left;
      mnode->right->parent = mnode->parent;
      mnode->parent->left = mnode->right;
    }

    Node* b_node = mnode->right_th ? mnode->parent : mnode->right;
    fix_balance_up(b_node);
    
    return mnode;
  }

  void fix_balance_up(Node* node) {
    while(node != nullptr) {
      Node* parent = node->parent;
      if (parent == nullptr) {
        root_ = balance(node);
      } else if (node == parent->left) {
        parent->left = balance(node);
      } else if (node == parent->right) {
        parent->right = balance(node);
      }
      node = parent;
    }
  }

  bool remove(const KeyT& key) {
    Node* tnode = find(key); 
    if (tnode == nullptr) {
      return false;
    }

    if (tnode->right_th) {
      if (tnode->left_th) {
        if (tnode->parent == nullptr) {
          root_ = nullptr;
        } else if (tnode == tnode->parent->left) {
          tnode->parent->left = tnode->left;
          tnode->parent->left_th = true;
        } else if (tnode == tnode->parent->right) {
          tnode->parent->right = tnode->right;
          tnode->parent->right_th = true;
        }
      } else {
        if (tnode->parent == nullptr) {
          tnode->left->right = nullptr;
          tnode->left->parent = nullptr;
          root_ = tnode->left;
        } else if (tnode == tnode->parent->left) {
          tnode->parent->left = tnode->left;
          tnode->left->right = tnode->right;
        } else if (tnode == tnode->parent->right) {
          tnode->parent->right = tnode->left;
          tnode->left->right = tnode->right;
        }
      }
    } else {
      Node* rmin_node = extract_min(tnode->right);
      
      rmin_node->right = tnode->right;
      rmin_node->right_th = false;

      rmin_node->left = tnode->left;
      rmin_node->left_th = false;

      left_most(tnode->right)->left = rmin_node;
      right_most(tnode->left)->right = rmin_node;

      if (tnode->parent == nullptr) {
        rmin_node->parent = nullptr;
        root_ = rmin_node;
      } else if (tnode == tnode->parent->left) {
        tnode->parent->left = rmin_node;
      } else if (tnode == tnode->parent->right) {
        tnode->parent->right = rmin_node;
      }
      fix_balance_up(rmin_node);
    }

    delete tnode;

    return true;
  }

  ThreadedBinaryTree() : root_(nullptr), comp_() {}
  
  ThreadedBinaryTree(ThreadedBinaryTree&& rhs) noexcept : root_(rhs.root_), comp_() {
    rhs.root_ = nullptr;
  }

  ThreadedBinaryTree(const ThreadedBinaryTree& rhs) : root_(rhs.copy()) {}
  
  ThreadedBinaryTree& operator=(const ThreadedBinaryTree& rhs) {
    if (this != &rhs) {
      ThreadedBinaryTree temp{rhs};
      std::swap(root_, temp.root_);
    }
    return *this;
  }
  
  ThreadedBinaryTree& operator=(ThreadedBinaryTree&& rhs) noexcept {
    if (this != &rhs) { std::swap(root_, rhs.root_); }

    return *this;
  }
  
  ~ThreadedBinaryTree() {
    clear();
  }

private:
  Node* copy() const {
    if (root_ == nullptr) { return nullptr; }
 
    std::unordered_map<const Node*, Node*> map;

    auto visit = [&map](const Node* node) {
      map[node] = new Node{node->key, node->value};
    };
    
    try {
      inorder(visit);

      for (auto [node, copy_node] : map) {
        copy_node->height = node->height;
  
        copy_node->left  = (node->left  == nullptr) ? nullptr : map[node->left];
        copy_node->right = (node->right == nullptr) ? nullptr : map[node->right];
        
        copy_node->left_th  = node->left_th;
        copy_node->right_th = node->right_th;    
  
        copy_node->parent = (node->parent == nullptr) ? nullptr : map[node->parent];
      }
  
      return map[root_];
    } catch (...) {
      for (auto [_, node] : map) { delete node; }
      throw;
    }
  }

  void clear() {
    std::vector<Node*> nodes;
    auto visit = [&nodes](Node* node) {
      nodes.push_back(node);
    };

    inorder(visit);

    for (auto ptr : nodes) {
      delete ptr;
    }
  }

  Node* root_;
  CompT comp_;
};

} // namespace myds