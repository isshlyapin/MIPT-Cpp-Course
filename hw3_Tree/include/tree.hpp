#pragma once

#include <vector>
#include <cassert>
#include <utility>
#include <stdexcept>
#include <functional>
#include <unordered_map>

namespace myds {

template<typename KeyT, typename ValueT, typename CompT = std::less<KeyT>>
class ThreadedBinaryTree {
private:
  struct Node {
    Node(
      const KeyT& key, 
      const ValueT& value, 
      Node* left   = nullptr, 
      Node* right  = nullptr, 
      Node* parent = nullptr, 
      bool left_th = true, bool right_th = true
    ) : data(key, value), height(1), 
        left(left), right(right), parent(parent), 
        left_th(left_th), right_th(right_th) {}

    std::pair<const KeyT, ValueT> data;

    int height;

    Node* left;
    Node* right;
    Node* parent;
    bool left_th, right_th;
  };
  
  class ConstIterator {
  public:
    using value_type = std::pair<const KeyT, ValueT>;
    using pointer = const value_type*;
    using reference = const value_type&;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit ConstIterator(const Node* n = nullptr, const Node* r = nullptr) 
      : node_(n), root_(r) {}

    reference operator*() const noexcept {
      return node_->data;
    }

    pointer operator->() const noexcept {
      return &(node_->data);
    }

    ConstIterator& operator++() noexcept {
      node_ = next(node_);
      return *this;
    }

    ConstIterator operator++(int) noexcept {
      ConstIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    ConstIterator& operator--() noexcept {
      if (node_ == nullptr) {
        node_ = right_most(root_);
      } else {
        node_ = prev(node_);
      }      
      return *this;
    }

    ConstIterator operator--(int) noexcept {
      ConstIterator tmp = *this;
      --(*this);
      return tmp;
    }

    auto operator<=>(const ConstIterator& other) const = default;

  private:
    friend class ThreadedBinaryTree<KeyT, ValueT, CompT>;

    const Node* next(const Node* n) const {
      if (n == nullptr) { return nullptr; }
      if (n->right_th) { return n->right; }
      return left_most(n->right);
    }

    const Node* prev(const Node* n) const {
      if (n == nullptr) { return nullptr; }
      if (n->left_th) { return n->left; }
      return right_most(n->left);
    }

    const Node* node_;
    const Node* root_;
  };

  class Iterator : public ConstIterator {
  public:
    using value_type = std::pair<const KeyT, ValueT>;
    using pointer    = value_type*;
    using reference  = value_type&;

    explicit Iterator(Node* n = nullptr, Node* root = nullptr)
      : ConstIterator(n, root) {}

    reference operator*() const noexcept {
      return const_cast<reference>(ConstIterator::operator*());
    }

    pointer operator->() const noexcept {
      return const_cast<pointer>(ConstIterator::operator->());
    }
  };

public:
  using value_type = std::pair<const KeyT, ValueT>;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;

  using pointer = value_type*;
  using const_pointer = const value_type*;

  using reference = value_type&;
  using const_reference = const value_type&;

  using iterator = Iterator;
  using const_iterator = ConstIterator;

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  ThreadedBinaryTree() : root_(nullptr), comp_(), size_(0) {}

  ThreadedBinaryTree(ThreadedBinaryTree&& rhs) noexcept 
    : root_(rhs.root_), comp_(rhs.comp_), size_(rhs.size_) {}

  ThreadedBinaryTree(const ThreadedBinaryTree& rhs) 
    : root_(rhs.copy()), comp_(rhs.comp_), size_(rhs.size_) {}

  ThreadedBinaryTree& operator=(const ThreadedBinaryTree& rhs) {
    if (this != &rhs) {
      ThreadedBinaryTree temp{rhs};
      std::swap(root_, temp.root_);
      std::swap(comp_, temp.comp_);
      std::swap(size_, temp.size_);
    }
    return *this;
  }
  
  ThreadedBinaryTree& operator=(ThreadedBinaryTree&& rhs) noexcept {
    if (this != &rhs) { 
      std::swap(root_, rhs.root_); 
      std::swap(comp_, rhs.comp_);
      std::swap(size_, rhs.size_);
    }
    return *this;
  }
  
  ~ThreadedBinaryTree() {
    clear();
  }

  size_t size() const {
    return size_;
  }

  iterator begin() { return iterator(left_most(root_), root_); }
  iterator end() { return iterator(nullptr, root_); }

  const_iterator begin() const { return const_iterator(left_most(root_), root_); }  
  const_iterator end() const { return const_iterator(nullptr, root_); }

  const_iterator cbegin() const { return begin(); }
  const_iterator cend() const { return end(); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

  const_reverse_iterator crbegin() const { return rbegin(); }
  const_reverse_iterator crend() const { return rend(); }

  ValueT& operator[](const KeyT& key) {
    Node* node = find_node(key);
    if (node == nullptr) {
      insert(key, ValueT{});
      node = find_node(key);
    }
    return node->data.second;
  }

  ValueT& at(const KeyT& key) {
    Node* node = find_node(key);
    if (node == nullptr) {
      throw std::out_of_range("Key not found in the tree");
    }
    return node->data.second;
  }

  const ValueT& at(const KeyT& key) const {
    const Node* node = find_node(key);
    if (node == nullptr) {
      throw std::out_of_range("Key not found in the tree");
    }
    return node->data.second;
  }

  iterator find(const KeyT& key) {
    Node* node = find_node(key);
    if (node == nullptr) { return end(); }    
    return iterator(node, root_);
  }

  const_iterator find(const KeyT& key) const {
    const Node* node = find_node(key);
    if (node == nullptr) { return end(); }    
    return const_iterator(node, root_);
  }

  // Вставка нового узла в дерево
  std::pair<iterator, bool> insert(const KeyT& new_key, const ValueT& new_value) {
    if (root_ == nullptr) {
      root_ = new Node{new_key, new_value};
      ++size_;
      return {iterator(root_), true};
    }
    
    Node* cur_node = root_;
    Node* last_left_step  = nullptr;
    Node* last_right_step = nullptr;
    
    while (cur_node != nullptr) {
      if (comp_(new_key, cur_node->data.first)) {
        if (cur_node->left_th) {
          cur_node->left = new Node{new_key, new_value, last_right_step, cur_node, cur_node};
          cur_node->left_th = false;
          cur_node = cur_node->left;
          break;
        }
        last_left_step = cur_node;
        cur_node = cur_node->left;
      } else if (comp_(cur_node->data.first, new_key)) {
        if (cur_node->right_th) {
          cur_node->right = new Node{new_key, new_value, cur_node, last_left_step, cur_node};
          cur_node->right_th = false;
          cur_node = cur_node->right;
          break;
        }
        last_right_step = cur_node;
        cur_node = cur_node->right;
      } else {
        return {iterator(cur_node), false};
      }
    }

    fix_balance_up(cur_node);

    ++size_;
    return {iterator(cur_node), true};
  }

  bool remove(const KeyT& key) {
    Node* tnode = find_node(key); 
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
      rmin_node->right_th = tnode->right_th;

      rmin_node->left = tnode->left;
      rmin_node->left_th = tnode->left_th;

      if (!tnode->right_th) {
        left_most(tnode->right)->left = rmin_node;
      }
      if (!tnode->left_th) {
        right_most(tnode->left)->right = rmin_node;
      }

      Node* tparent = tnode->parent;
      rmin_node->parent = tparent;
      if (tparent == nullptr) {
        root_ = rmin_node;
      } else if (tnode == tparent->left) {
        tparent->left = rmin_node;
      } else if (tnode == tparent->right) {
        tparent->right = rmin_node;
      }
      fix_balance_up(rmin_node);
    }

    delete tnode;

    return true;
  }

  // first not less than key
  iterator lower_bound(const KeyT& key) {
    return iterator(const_cast<Node*>(find_bound(key, /* upper = */ false)), root_);
  }

  // first not less than key (const version)
  const_iterator lower_bound(const KeyT& key) const {
    return const_iterator(find_bound(key, /* upper = */ false), root_);
  }

  // first greater than key
  iterator upper_bound(const KeyT& key) {
    return iterator(const_cast<Node*>(find_bound(key, /* upper = */ true)), root_);
  }

  // first greater than key (const version)
  const_iterator upper_bound(const KeyT& key) const {
    return const_iterator(find_bound(key, /* upper = */ true), root_);
  }

private:
  // Обобщенный поиск границы (lower_bound или upper_bound)
  // upper = false: lower_bound (первый >= key)
  // upper = true:  upper_bound (первый > key)
  const Node* find_bound(const KeyT& key, bool upper) const {
    const Node* cur_node = root_;
    const Node* res = nullptr;

    while (cur_node != nullptr) {
      bool go_left = upper 
        ? comp_(key, cur_node->data.first)           // key < cur_node (для upper_bound)
        : !comp_(cur_node->data.first, key);         // cur_node >= key (для lower_bound)

      if (go_left) {
        res = cur_node;
        if (cur_node->left_th) { break; }
        cur_node = cur_node->left;
      } else {
        if (cur_node->right_th) { break; }
        cur_node = cur_node->right;
      }
    }

    return res;
  }

  // Поиск элемента по ключу (const версия)
  const Node* find_node(const KeyT& key) const {
    const Node* cur_node = root_;
    while (cur_node != nullptr) {
      if (comp_(key, cur_node->data.first)) {
        if (cur_node->left_th) { return nullptr; }
        cur_node = cur_node->left;
      } else if (comp_(cur_node->data.first, key)) {
        if (cur_node->right_th) { return nullptr; }
        cur_node = cur_node->right;
      } else {
        return cur_node;
      }
    }

    return nullptr;
  }

  // Поиск элемента по ключу (non-const версия через const)
  Node* find_node(const KeyT& key) {
    return const_cast<Node*>(std::as_const(*this).find_node(key));
  }

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

  // Inorder обход с вызовом функции visit для каждого узла (const version)
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

  Node* extract_min(Node* node) {
    assert(node != nullptr);

    Node* mnode = left_most(node);
    
    if (mnode == root_) {
      if (mnode->right_th) {
        root_ = nullptr;
      } else {
        root_ = mnode->right;
        root_->parent = nullptr;
        root_->left = nullptr;
        root_->left_th = true;
      }
      return mnode;
    }
    
    if (mnode == mnode->parent->left) {
      if (mnode->right_th) {
        mnode->parent->left = mnode->left;
        mnode->parent->left_th = true;
      } else {
        mnode->right->left = mnode->left;
        mnode->right->parent = mnode->parent;
        mnode->parent->left = mnode->right;
      }        
    } else {
      if (mnode->right_th) {
        mnode->parent->right = mnode->right;
        mnode->parent->right_th = true;
      } else {
        mnode->right->left = mnode->left;
        mnode->right->parent = mnode->parent;
        mnode->parent->right = mnode->right;
      }
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
    assert(node != nullptr);
    return node->left_th ? nullptr : node->left;
  }

  Node* right_ptr(const Node* node) {
    assert(node != nullptr);
    return node->right_th ? nullptr : node->right;
  }

  Node* rotate_right(Node* node) {
    if (node == nullptr) { return nullptr; }

    assert(!node->left_th);
    Node* lnode = node->left;

    if (!lnode->right_th) {
      lnode->right->parent = node;
      node->left = lnode->right;
    } else {
      node->left = lnode;
      node->left_th = true;
    }

    lnode->parent = node->parent;
    node->parent = lnode;

    lnode->right = node;
    lnode->right_th = false;

    fixheight(node);
    fixheight(lnode);
    return lnode; 
  }

  Node* rotate_left(Node* node) {
    if (node == nullptr) { return nullptr; }

    assert(!node->right_th);
    Node* rnode = node->right;

    if (!rnode->left_th) {
      rnode->left->parent = node;
      node->right = rnode->left;
    } else {
      node->right = rnode;
      node->right_th = true; 
    }

    rnode->parent = node->parent;
    node->parent = rnode;

    rnode->left = node;
    rnode->left_th = false;

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
      if(bfactor(p->left) > 0) {
        p->left = rotate_left(p->left);
      }
      return rotate_right(p);
    }
    
    return p;
  }

  // Поиск самого левого узла относительно заданного
  static Node* left_most(Node* root) {
    if (root == nullptr) { return nullptr; }
    Node* res = root;
    while (!res->left_th) {
      res = res->left;
    }
    return res;
  }

  static const Node* left_most(const Node* root) {
    if (root == nullptr) { return nullptr; }
    const Node* res = root;
    while (!res->left_th) {
      res = res->left;
    }
    return res;
  }

  // Поиск самого правого узла относительно заданного
  static Node* right_most(Node* root) {
    if (root == nullptr) { return nullptr; }
    Node* res = root;
    while (!res->right_th) {
      res = res->right;
    }
    return res;
  }

  static const Node* right_most(const Node* root) {
    if (root == nullptr) { return nullptr; }
    const Node* res = root;
    while (!res->right_th) {
      res = res->right;
    }
    return res;
  }

  Node* copy() const {
    if (root_ == nullptr) { return nullptr; }
 
    std::unordered_map<const Node*, Node*> map;

    auto visit = [&map](const Node* node) {
      map[node] = new Node{node->data.first, node->data.second};
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
  size_t size_;
};

} // namespace myds