/*
ThreadedBinaryTree - это реализация сбалансированного двоичного дерева поиска 
с использованием потоков (threading) для оптимизации обхода
*/

/* 

Инварианты структуры данных:

1) BST: ключи строго упорядочены. Нет дубликатов.

2) Ссылки на детей: если left_th==false, то left - реальный ребенок и left->parent==this. 
                    если left_th==true, то left - указывает на предшественника в порядке обхода
                    (или sentinel_, если его нет).
                    Аналогично для right.

3) sentinel_ - это end(). Для непустого дерева:
   sentinel_->right = leftmost(root_) и sentinel_->left = rightmost(root_).
   leftmost(root_)->left (поток) == sentinel_
   rightmost(root_)->right (поток) == sentinel_

4) AVL: abs(balance_factor(node)) <= 1 и node->height согласованны.

*/

#pragma once

#include <cmath>
#include <cassert>
#include <utility>
#include <iostream>
#include <optional>
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
      bool left_th = true, bool right_th = true,
      int height   = 1
    ) : data(key, value), height(height), 
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
    using pointer    = const value_type*;
    using reference  = const value_type&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

    explicit ConstIterator(const Node* n = nullptr) : node_(n) {}

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
      node_ = prev(node_);
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
      if (n->right_th)  { return n->right; }
      return left_most(n->right);
    }

    const Node* prev(const Node* n) const {
      if (n == nullptr) { return nullptr; }
      if (n->left_th)   { return n->left; }
      return right_most(n->left);
    }

    const Node* node_;
  };

  class Iterator : public ConstIterator {
  public:
    using value_type = std::pair<const KeyT, ValueT>;
    using pointer    = value_type*;
    using reference  = value_type&;

    explicit Iterator(Node* n = nullptr) : ConstIterator(n) {}

    reference operator*() const noexcept {
      return const_cast<reference>(ConstIterator::operator*());
    }

    pointer operator->() const noexcept {
      return const_cast<pointer>(ConstIterator::operator->());
    }
  };

public:
  using value_type      = std::pair<const KeyT, ValueT>;
  using size_type       = size_t;
  using difference_type = std::ptrdiff_t;

  using pointer       =       value_type*;
  using const_pointer = const value_type*;

  using reference       =       value_type&;
  using const_reference = const value_type&;

  using iterator       = Iterator;
  using const_iterator = ConstIterator;

  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  ThreadedBinaryTree() : root_(nullptr), sentinel_(make_sentinel()), comp_(), size_(0) {
    assert(validate(&std::cerr));
  }

  ThreadedBinaryTree(ThreadedBinaryTree&& rhs) noexcept : ThreadedBinaryTree() {
    swap_with(rhs);
    assert(validate(&std::cerr));
  }

  ThreadedBinaryTree(const ThreadedBinaryTree& rhs) : ThreadedBinaryTree() {
    ThreadedBinaryTree<KeyT, ValueT, CompT> tmp;
    tmp.size_ = rhs.size_;
    tmp.comp_ = rhs.comp_;
    tmp.root_ = rhs.copy();
    tmp.update_sentinel();

    swap_with(tmp);
    assert(validate(&std::cerr));
   }

  ThreadedBinaryTree& operator=(const ThreadedBinaryTree& rhs) {
    if (this != &rhs) {
      ThreadedBinaryTree temp{rhs};
      swap_with(temp);
    }
    assert(validate(&std::cerr));
    return *this;
  }
  
  ThreadedBinaryTree& operator=(ThreadedBinaryTree&& rhs) noexcept {
    if (this != &rhs) { swap_with(rhs); }
    assert(validate(&std::cerr));
    return *this;
  }
  
  ~ThreadedBinaryTree() {
    assert(validate(&std::cerr));
    inorder([](Node* node) { delete node; });
    delete sentinel_;
  }

  size_t size() const { return size_; }

  bool empty() const { return size_ == 0; }

  iterator begin() { return root_ ? iterator(left_most(root_)) : end(); }
  iterator end()   { return iterator(sentinel_); }

  const_iterator begin() const { return root_ ? const_iterator(left_most(root_)) : end(); }  
  const_iterator end()   const { return const_iterator(sentinel_); }

  const_iterator cbegin() const { return begin(); }
  const_iterator cend()   const { return end(); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend()   { return reverse_iterator(begin()); }

  const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
  const_reverse_iterator rend()   const { return const_reverse_iterator(begin()); }

  const_reverse_iterator crbegin() const { return rbegin(); }
  const_reverse_iterator crend()   const { return rend(); }

  ValueT& operator[](const KeyT& key) {
    Node* node = find_node(key);
    if (node == sentinel_) {
      auto [it, ok] = insert(key, ValueT{});
      return it->second;
    }
    return node->data.second;
  }

  ValueT& at(const KeyT& key) {
    Node* node = find_node(key);
    if (node == sentinel_) {
      throw std::out_of_range("Key not found in the tree");
    }
    return node->data.second;
  }

  const ValueT& at(const KeyT& key) const {
    const Node* node = find_node(key);
    if (node == sentinel_) {
      throw std::out_of_range("Key not found in the tree");
    }
    return node->data.second;
  }

  iterator find(const KeyT& key) {
    return iterator(find_node(key));
  }

  const_iterator find(const KeyT& key) const {
    return const_iterator(find_node(key));
  }

  // Вставка нового узла в дерево
  std::pair<iterator, bool> insert(const KeyT& new_key, const ValueT& new_value) {
    assert(validate(&std::cerr));
    if (root_ == nullptr) {
      root_ = new Node{new_key, new_value};
      update_sentinel();
      ++size_;
      assert(validate(&std::cerr));
      return {iterator(root_), true};
    }
    
    Node* cur_node = root_;
    Node* last_left_step  = nullptr;
    Node* last_right_step = nullptr;
    
    while (true) {
      if (comp_(new_key, cur_node->data.first)) {
        if (cur_node->left_th) {        
          Node* new_node = new Node{new_key, new_value, last_right_step, cur_node, cur_node};
          attach_left_child(cur_node, new_node);
          fix_balance_up(new_node);
          update_sentinel();
          ++size_;
          assert(validate(&std::cerr));
          return {iterator(new_node), true};
        }
        last_left_step = cur_node;
        cur_node = cur_node->left;
      } else if (comp_(cur_node->data.first, new_key)) {
        if (cur_node->right_th) {
          Node* new_node = new Node{new_key, new_value, cur_node, last_left_step, cur_node};
          attach_right_child(cur_node, new_node);
          fix_balance_up(new_node);
          update_sentinel();
          ++size_;
          assert(validate(&std::cerr));
          return {iterator(new_node), true};
        }
        last_right_step = cur_node;
        cur_node = cur_node->right;
      } else {
        // assert(validate(&std::cerr));
        return {iterator(cur_node), false};
      }
    }
  }

  bool remove(const KeyT& key) {
    assert(validate(&std::cerr));
    Node* tnode = find_node(key); 
    if (tnode == sentinel_) { return false; }

    Node* balance_start = nullptr;
    if (right_is_thread(tnode)) {
      if (left_is_thread(tnode)) {
        if (is_root(tnode)) {
          root_ = nullptr;
        } else if (is_left_child(tnode)) {
          attach_left_thread(tnode->parent, tnode->left);
        } else if (is_right_child(tnode)) {
          attach_right_thread(tnode->parent, tnode->right);
        }
      } else {
        if (is_root(tnode)) {
          tnode->left->right  = nullptr;
          tnode->left->parent = nullptr;
          root_ = tnode->left;
        } else if (is_left_child(tnode)) {
          attach_right_thread(tnode->left, tnode->right);
          attach_left_child(tnode->parent, tnode->left);
        } else if (is_right_child(tnode)) {
          attach_right_thread(tnode->left, tnode->right);
          attach_right_child(tnode->parent, tnode->left);
        }
      }
      balance_start = tnode->parent;
    } else {
      Node* rmin_node = extract_min(tnode->right);

      rmin_node->right    = tnode->right;
      rmin_node->right_th = tnode->right_th;

      rmin_node->left    = tnode->left;
      rmin_node->left_th = tnode->left_th;

      if (!tnode->right_th) {
        tnode->right->parent = rmin_node;
        attach_left_thread(left_most(tnode->right), rmin_node);
      }
      if (!tnode->left_th) {
        tnode->left->parent = rmin_node;
        attach_right_thread(right_most(tnode->left), rmin_node);
      }

      Node* tparent = tnode->parent;
      rmin_node->parent = tparent;
      if (is_root(tnode)) {
        root_ = rmin_node;
      } else if (is_left_child(tnode)) {
        attach_left_child(tparent, rmin_node);
      } else if (is_right_child(tnode)) {
        attach_right_child(tparent, rmin_node);
      }
      balance_start = rmin_node;
    } 
     
    fix_balance_up(balance_start);
    update_sentinel();

    delete tnode;
    --size_;
  
    assert(validate(&std::cerr));
    return true;
  }

  // first not less than key
  iterator lower_bound(const KeyT& key) {
    return iterator(const_cast<Node*>(find_bound(key, /* upper = */ false)));
  }

  // first not less than key (const version)
  const_iterator lower_bound(const KeyT& key) const {
    return const_iterator(find_bound(key, /* upper = */ false));
  }

  // first greater than key
  iterator upper_bound(const KeyT& key) {
    return iterator(const_cast<Node*>(find_bound(key, /* upper = */ true)));
  }

  // first greater than key (const version)
  const_iterator upper_bound(const KeyT& key) const {
    return const_iterator(find_bound(key, /* upper = */ true));
  }

private:
  // Константы для балансировки AVL-дерева
  static constexpr int BALANCE_THRESHOLD_RIGHT =  2;   // Правое поддерево слишком высокое
  static constexpr int BALANCE_THRESHOLD_LEFT  = -2;   // Левое поддерево слишком высокое

  void attach_left_child(Node* parent, Node* child) {
    assert(parent);
    assert(child);
    parent->left    = child;
    parent->left_th = false;
  }

  void attach_right_child(Node* parent, Node* child) {
    assert(parent);
    assert(child);
    parent->right    = child;
    parent->right_th = false;
  }

  void attach_left_thread(Node* parent, Node* thread) {
    assert(parent);
    assert(thread);
    parent->left    = thread;
    parent->left_th = true;
  }

  void attach_right_thread(Node* parent, Node* thread) {
    assert(parent);
    assert(thread);
    parent->right    = thread;
    parent->right_th = true;
  }
  
  bool left_is_thread(const Node* node) const {
    assert(node != nullptr);
    return node->left_th;
  }

  bool right_is_thread(const Node* node) const {
    assert(node != nullptr);
    return node->right_th;
  }

  bool is_root(const Node* node) const {
    assert(node != nullptr);
    return node->parent == nullptr;
  }

  bool is_left_child(const Node* node) const {
    assert(node != nullptr);
    return node->parent != nullptr && node == node->parent->left;
  }

  bool is_right_child(const Node* node) const {
    assert(node != nullptr);
    return node->parent != nullptr && node == node->parent->right;
  }

  // Обобщенный поиск границы (lower_bound или upper_bound)
  // upper = false: lower_bound (первый >= key)
  // upper = true:  upper_bound (первый > key)
  const Node* find_bound(const KeyT& key, bool upper) const {
    if (root_ == nullptr) { return sentinel_; }

    const Node* cur_node = root_;
    const Node* res = sentinel_;

    while (cur_node != sentinel_) {
      bool go_left = upper 
        ? comp_(key, cur_node->data.first)           // key < cur_node (для upper_bound)
        : !comp_(cur_node->data.first, key);         // cur_node >= key (для lower_bound)

      if (go_left) {
        res = cur_node;
        if (left_is_thread(cur_node)) { break; }
        cur_node = cur_node->left;
      } else {
        if (right_is_thread(cur_node)) { break; }
        cur_node = cur_node->right;
      }
    }

    return res;
  }

  // Поиск элемента по ключу (const версия)
  const Node* find_node(const KeyT& key) const {
    const Node* cur_node = root_;
    if (cur_node == nullptr) { return sentinel_; }
    while (cur_node != sentinel_) {
      if (comp_(key, cur_node->data.first)) {
        if (left_is_thread(cur_node)) { return sentinel_; }
        cur_node = cur_node->left;
      } else if (comp_(cur_node->data.first, key)) {
        if (right_is_thread(cur_node)) { return sentinel_; }
        cur_node = cur_node->right;
      } else {
        return cur_node;
      }
    }
    return sentinel_;
  }

  // Поиск элемента по ключу (non-const версия через const)
  Node* find_node(const KeyT& key) {
    return const_cast<Node*>(std::as_const(*this).find_node(key));
  }

  // Inorder обход с вызовом функции visit для каждого узла
  template<typename F>
  void inorder(F visit) {
    Node* node = left_most(root_);
    while (node != sentinel_ && node != nullptr) {
      Node* vnode = node;
      if (right_is_thread(node)) { 
        node = node->right; 
      } else {
        node = left_most(node->right);
      }
      visit(vnode);
    }
    // assert(validate(&std::cerr));
  }

  // Inorder обход с вызовом функции visit для каждого узла (const version)
  template<typename F>
  void inorder(F visit) const {
    const Node* node = left_most(root_);
    while (node != sentinel_ && node != nullptr) {
      visit(node);
      if (right_is_thread(node)) {
        node = node->right;
      }
      else {
        node = left_most(node->right);
      }
    }
    // assert(validate(&std::cerr));
  }

  Node* extract_min(Node* node) {
    assert(node != nullptr);

    Node* mnode = left_most(node);
    
    if (is_root(mnode)) {
      if (right_is_thread(mnode)) {
        root_ = nullptr;
      } else {
        root_ = mnode->right;
        root_->parent = nullptr;
        attach_left_thread(root_, sentinel_);
      }
      return mnode;
    }
    
    if (is_left_child(mnode)) {
      if (right_is_thread(mnode)) {
        attach_left_thread(mnode->parent, mnode->left);
      } else {
        attach_left_thread(mnode->right, mnode->left);
        attach_left_child(mnode->parent, mnode->right);
        mnode->right->parent = mnode->parent;
      }
    } else {
      if (right_is_thread(mnode)) {
        attach_right_thread(mnode->parent, mnode->right);
      } else {
        attach_left_thread(mnode->right, mnode->left);
        attach_right_child(mnode->parent, mnode->right);
        mnode->right->parent = mnode->parent;
      }
    }

    Node* b_node = right_is_thread(mnode) ? mnode->parent : mnode->right;
    fix_balance_up(b_node);
    
    return mnode;
  }

  void fix_balance_up(Node* node) {
    while(node != nullptr) {
      Node* parent = node->parent;
      if (parent == nullptr) {
        root_ = balance(node);
      } else if (is_left_child(node)) {
        parent->left = balance(node);
      } else if (is_right_child(node)) {
        parent->right = balance(node);
      }
      node = parent;
    }
  }

  int height(const Node* node) const {
    return (node == nullptr) ? 0 : node->height; 
  }

  int bfactor(const Node* node) const {
    return height(right_ptr(node)) - height(left_ptr(node));    
  }

  void fixheight(Node* node) {
    int hl = height(left_ptr(node));
    int hr = height(right_ptr(node));
    node->height = std::max(hl, hr) + 1;
  }

  const Node* left_ptr(const Node* node) const {
    assert(node != nullptr);
    return left_is_thread(node) ? nullptr : node->left;
  }

  const Node* right_ptr(const Node* node) const {
    assert(node != nullptr);
    return right_is_thread(node) ? nullptr : node->right;
  }

  Node* rotate_right(Node* node) {
    if (node == nullptr) { return nullptr; }

    assert(!left_is_thread(node));
    Node* lnode = node->left;

    if (!left_is_thread(lnode)) {
      lnode->right->parent = node;
      node->left = lnode->right;
    } else {
      attach_left_thread(node, lnode);
    }

    lnode->parent = node->parent;
    node->parent = lnode;

    attach_right_child(lnode, node);

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
    if (bfactor(p) == BALANCE_THRESHOLD_RIGHT) {
      if (bfactor(p->right) < 0) {
        p->right = rotate_right(p->right);
      }
      return rotate_left(p);
    }
    if (bfactor(p) == BALANCE_THRESHOLD_LEFT) {
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

  Node* make_sentinel() {
    Node* snt   = new Node{KeyT{}, ValueT{}};    
    snt->height = 0;
    
    snt->left  = snt;
    snt->right = snt;
    return snt;
  }

  void update_sentinel() {
    assert(sentinel_);
    if (root_) {
      Node* lmost = left_most(root_);
      Node* rmost = right_most(root_);

      sentinel_->left  = rmost;
      sentinel_->right = lmost;

      lmost->left  = sentinel_;
      rmost->right = sentinel_;
    } else {
      sentinel_->left  = sentinel_;
      sentinel_->right = sentinel_;
    }
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
  
        copy_node->left  = (node->left  == sentinel_) ? nullptr : map[node->left];
        copy_node->right = (node->right == sentinel_) ? nullptr : map[node->right];
        
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

  void swap_with(ThreadedBinaryTree& other) noexcept {
    std::swap(root_, other.root_);
    std::swap(sentinel_, other.sentinel_);
    std::swap(comp_, other.comp_);
    std::swap(size_, other.size_);
  }

#ifndef NDEBUG
  // validate() that reports the first failure via return fail("...").
  // Pass &std::cerr if you want diagnostics; pass nullptr to stay silent.
  bool validate(std::ostream* out = nullptr) const {
    auto dbgs = [&](const char* msg) {
      if (out) { (*out) << msg << "\n"; }
    };
    auto fail = [&](const char* msg) -> bool {
      dbgs(msg);
      return false;
    };

    // ---- helpers ----
    auto is_real_left  = [&](const Node* n) { return n && !n->left_th;  };
    auto is_real_right = [&](const Node* n) { return n && !n->right_th; };

    auto left_child  = [&](const Node* n) -> const Node* { return is_real_left(n)  ? n->left  : nullptr; };
    auto right_child = [&](const Node* n) -> const Node* { return is_real_right(n) ? n->right : nullptr; };

    // ---- sentinel basic sanity ----
    if (sentinel_ == nullptr) { return fail("validate failed: sentinel_ == nullptr"); }
    if (sentinel_->height != 0) { return fail("validate failed: sentinel_->height != 0"); }

    // Empty tree contract
    if (root_ == nullptr) {
      if (size_ != 0) { return fail("validate failed: root_ == nullptr but size_ != 0"); }
      if (sentinel_->left  != sentinel_) { return fail("validate failed: empty tree: sentinel_->left != sentinel_"); }
      if (sentinel_->right != sentinel_) { return fail("validate failed: empty tree: sentinel_->right != sentinel_"); }
      return true;
    }

    // Non-empty tree basic expectations
    if (size_ == 0) { return fail("validate failed: root_ != nullptr but size_ == 0"); }
    if (root_->parent != nullptr) { return fail("validate failed: root_->parent != nullptr"); }

    // ---- structural DFS checks: BST order, parent links, AVL height/balance ----
    bool ok = true;

    std::function<int(const Node*, const KeyT*, const KeyT*)> dfs =
      [&](const Node* n, const KeyT* low, const KeyT* high) -> int {
        if (!ok) { return 0; }

        if (n == nullptr)   { ok = false; dbgs("validate failed: dfs: null node"); return 0; }
        if (n == sentinel_) { ok = false; dbgs("validate failed: dfs: sentinel used as real node"); return 0; }

        // Thread pointers must never be nullptr in your model (thread to node or sentinel)
        if (n->left_th  && n->left  == nullptr) { ok = false; dbgs("validate failed: left thread is nullptr");  return 0; }
        if (n->right_th && n->right == nullptr) { ok = false; dbgs("validate failed: right thread is nullptr"); return 0; }

        const KeyT& k = n->data.first;

        // low < k < high (strict)
        if (low  && !comp_(*low, k))  { ok = false; dbgs("validate failed: BST lower bound violated"); return 0; }
        if (high && !comp_(k, *high)) { ok = false; dbgs("validate failed: BST upper bound violated"); return 0; }

        const Node* L = left_child(n);
        const Node* R = right_child(n);

        if (L) {
          if (L == sentinel_) { ok = false; dbgs("validate failed: left child is sentinel");     return 0; }
          if (L->parent != n) { ok = false; dbgs("validate failed: left child parent mismatch"); return 0; }
        }
        if (R) {
          if (R == sentinel_) { ok = false; dbgs("validate failed: right child is sentinel");     return 0; }
          if (R->parent != n) { ok = false; dbgs("validate failed: right child parent mismatch"); return 0; }
        }

        int hl = L ? dfs(L, low, &k) : 0;
        int hr = R ? dfs(R, &k, high) : 0;

        if (!ok) { return 0; }

        int computed_h = (hl > hr ? hl : hr) + 1;
        if (n->height != computed_h) { ok = false; dbgs("validate failed: height mismatch"); return 0; }

        int bf = hr - hl;
        if (std::abs(bf) > 1) { ok = false; dbgs("validate failed: AVL balance factor violated"); return 0; }

        return computed_h;
      };

    (void)dfs(root_, nullptr, nullptr);
    if (!ok) { return false; } // dfs already printed the reason

    // ---- inorder thread/sentinel checks + node count ----
    const Node* first = left_most(root_);
    const Node* last  = right_most(root_);
    if (first == nullptr || last == nullptr) { 
      return fail("left_most/right_most returned nullptr for non-empty tree");
    }

    // sentinel should point to extremes
    if (sentinel_->right != first) { return fail("sentinel_->right != left_most(root_)"); }
    if (sentinel_->left  != last)  { return fail("sentinel_->left != right_most(root_)"); }

    // extremes should thread to sentinel
    if (!first->left_th) { 
      return fail("leftmost node: left_th is false (must be thread to sentinel_)");
    }
    if (first->left != sentinel_) { 
      return fail("leftmost node: left thread does not point to sentinel_");
    }

    if (!last->right_th) { 
      return fail("rightmost node: right_th is false (must be thread to sentinel_)");
    }
    if (last->right != sentinel_) { 
      return fail("rightmost node: right thread does not point to sentinel_"); 
    }

    // Walk inorder via threads and verify predecessor/successor linkage.
    size_t count = 0;
    const Node* prev = sentinel_;
    const Node* cur  = first;

    for (size_t steps = 0; steps <= size_ + 1; ++steps) {
      if (cur == sentinel_) { break; } 
      if (cur == nullptr) { return fail("inorder walk: encountered nullptr"); }

      // predecessor thread correctness
      if (cur->left_th) {
        if (cur->left != prev) { return fail("inorder walk: left thread != predecessor"); }
      }

      // successor thread correctness for prev + strict key order
      if (prev != sentinel_) {
        if (prev->right_th && prev->right != cur) { return fail("inorder walk: right thread != successor"); }
        if (!comp_(prev->data.first, cur->data.first)) { return fail("inorder walk: keys not strictly increasing"); }
      }

      ++count;

      const Node* next = cur->right_th ? cur->right : left_most(cur->right);
      prev = cur;
      cur  = next;
    }

    if (cur != sentinel_) { return fail("inorder walk: did not end at sentinel_ (cycle or broken threads)"); }
    if (count != size_) { return fail("inorder walk: visited node count != size_"); }

    return true;
  }
#endif

  Node* root_;
  Node* sentinel_;
  CompT comp_;
  size_t size_;
};

} // namespace myds