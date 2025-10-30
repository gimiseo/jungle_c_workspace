#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  if (!p)
    return NULL;

  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  if (!nil_node)
  {
    free(p);
    return NULL;
  }
  nil_node->color = RBTREE_BLACK;
  nil_node->parent = nil_node;
  nil_node->left = nil_node;
  nil_node->right = nil_node;

  p->nil = nil_node;
  p->root = nil_node;

  return p;
}

static void allFreeInTree(node_t *node, node_t *nil)
{
  if (node == nil)
    return;
  allFreeInTree(node->left, nil);
  allFreeInTree(node->right, nil);
  free(node);
}

void delete_rbtree(rbtree *t) {
  if(!t)
    return;
  allFreeInTree(t->root, t->nil);
  free(t->nil);
  free(t);
}

///////////////////////////////////////////////////////////////////////////////

static node_t *make_node(rbtree *t, color_t color, key_t key, node_t *parent)
{
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  if (!new_node)
    return NULL;
  new_node->color = color;
  new_node->key = key;
  new_node->parent = parent;
  new_node->left = t->nil;
  new_node->right = t->nil;

  return new_node;
}
static  void L_rotate(rbtree *t, node_t *node)
{
  node_t *r_node = node->right;
  node->right = r_node->left;
  if (r_node->left != t->nil)
    r_node->left->parent = node;
  r_node->parent = node->parent;
  if (node->parent == t->nil)
    t->root = r_node;
  else if (node == node->parent->right)
    node->parent->right = r_node;
  else
    node->parent->left = r_node;
  node->parent = r_node;
  r_node->left = node;
}

static  void R_rotate(rbtree *t, node_t *node)
{
  node_t *l_node = node->left;
  node->left = l_node->right;
  if (l_node->right != t->nil)
    l_node->right->parent = node;
  l_node->parent = node->parent;
  if (node->parent == t->nil)
    t->root = l_node;
  else if (node == node->parent->right)
    node->parent->right = l_node;
  else
    node->parent->left = l_node;
  node->parent = l_node;
  l_node->right = node;
}

static void insert_fix_up(rbtree *t, node_t *child)
{
  node_t *uncle;
  while (child != t->root && child->parent->color == RBTREE_RED)
  {
    node_t *grandparent = child->parent->parent;

    //부모는 할배의 왼쪽 자식이다
    if (child->parent == grandparent->left)
    {
      uncle = grandparent->right;
      //case 1: 삼촌이 레드다
      if (uncle->color == RBTREE_RED)
      {
        grandparent->color = RBTREE_RED;
        uncle->color = RBTREE_BLACK;
        child->parent->color = RBTREE_BLACK;
        child = grandparent;
      }//case2,3 삼촌은 블랙이다
      else
      {
        if (child == child->parent->right)
        {
          child = child->parent;
          L_rotate(t, child);
        }
        child->parent->color = RBTREE_BLACK;
        child->parent->parent->color = RBTREE_RED;
        R_rotate(t, child->parent->parent);
      }
    }//부모는 할배의 오른쪽 자식이다. 미러링
    else
    {
      uncle = grandparent->left;
      if (uncle->color == RBTREE_RED)
      {
        grandparent->color = RBTREE_RED;
        uncle->color = RBTREE_BLACK;
        child->parent->color = RBTREE_BLACK;
        child = grandparent;
      }
      else
      {
        if (child == child->parent->left)
        {
          child= child->parent;
          R_rotate(t, child);
        }
        child->parent->color = RBTREE_BLACK;
        child->parent->parent->color = RBTREE_RED;
        L_rotate(t, child->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  
  node_t *parent, *child;
  node_t *cur, *return_node;

  if (!t)
    return NULL;
  
  //루트면 root값 만들고 내보내기
  if (t->root == t->nil)
  {
    t->root = make_node(t, RBTREE_BLACK, key, t->nil);
    if (!t->root)
      return NULL;
    return t->root;
  }
  

  //넣을자리 찾기
  cur = t->root;
  while (cur != t->nil)
  {
    parent = cur;
    if (cur->key > key)
      cur = cur->left;
    else 
      cur = cur->right;
  }

  //노드만들기
  return_node = make_node(t, RBTREE_RED, key, parent);
  if(!return_node)
    return NULL;
  //부모자식찾아주기
  if (parent->key > key)
    parent->left = return_node;
  else
    parent->right = return_node;

  //최종적으로 fix_up돌리기
  child = return_node;
  insert_fix_up(t, child);
  return return_node;
}
///////////////////////////////////////////////////////////////////////////////


node_t *rbtree_find(const rbtree *t, const key_t key) {
  if(!t)
    return NULL;
  node_t *result = t->root;
  while (result != t->nil)
  {
    if (result->key > key)
      result = result->left;
    else if (result->key < key)
      result = result->right;
    else
      break;
  }
  if (result == t->nil)
    return NULL;
  return result;
}

node_t *rbtree_min(const rbtree *t) {
  if(!t)
    return NULL;
  node_t *result = t->root;
  while (result->left != t->nil)
    result = result->left;
  if (result == t->nil)
    return NULL;
  return result;
}

node_t *rbtree_max(const rbtree *t) {
  if(!t)
    return NULL;
  node_t *result = t->root;
  while (result->right != t->nil)
    result = result->right;
  if (result == t->nil)
    return NULL;
  return result;
}
////////////////////////////////////////////////////////////////////////////////

static void erase_fix_up(rbtree *t, node_t *node)
{
  node_t *bro; 

  while (node != t->root && node->color == RBTREE_BLACK)
  {
    if (node == node->parent->left)
    {
      bro = node->parent->right;

      if (bro->color == RBTREE_RED)
      {
        bro->color = RBTREE_BLACK;
        node->parent->color = RBTREE_RED;
        L_rotate(t, node->parent);
        bro = node->parent->right;
      }

      if (bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK)
      {
        bro->color = RBTREE_RED;
        node = node->parent;
      }
      else
      {
        if (bro->right->color == RBTREE_BLACK)
        {
          bro->left->color = RBTREE_BLACK;
          bro->color = RBTREE_RED;
          R_rotate(t, bro);
          bro = node->parent->right;
        }

        bro->color = node->parent->color;
        node->parent->color = RBTREE_BLACK;
        bro->right->color = RBTREE_BLACK;
        L_rotate(t, node->parent);
        node = t->root;
      }
    }
    else
    {
      bro = node->parent->left;


      if (bro->color == RBTREE_RED)
      {
        bro->color = RBTREE_BLACK;
        node->parent->color = RBTREE_RED;
        R_rotate(t, node->parent);
        bro = node->parent->left;
      }
      if (bro->left->color == RBTREE_BLACK && bro->right->color == RBTREE_BLACK)
      {
        bro->color = RBTREE_RED;
        node = node->parent;
      }
      else
      {
        if (bro->left->color == RBTREE_BLACK)
        {
          bro->right->color = RBTREE_BLACK;
          bro->color = RBTREE_RED;
          L_rotate(t, bro);
          bro = node->parent->left;
        }

        bro->color = node->parent->color;
        node->parent->color = RBTREE_BLACK;
        bro->left->color = RBTREE_BLACK;
        R_rotate(t, node->parent);
        node = t->root;
      }
    }
  }
 
  node->color = RBTREE_BLACK;
}

static void rbtree_transplant(rbtree *t, node_t *u, node_t *v) 
{
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;
  v->parent = u->parent;
}

static node_t *rbtree_find_successor(rbtree *t, node_t *node) 
{
  while (node->left != t->nil)
    node = node->left;
  return node;
}

int rbtree_erase(rbtree *t, node_t *p) 
{
  if (!t || t->root == t->nil || p == NULL || p == t->nil)
    return 0;

  node_t *y = p;
  node_t *x;
  color_t y_original_color = y->color; 

  if (p->left == t->nil)
  {
    x = p->right;
    rbtree_transplant(t, p, p->right);
  } 
  else if (p->right == t->nil) {
    x = p->left;
    rbtree_transplant(t, p, p->left);
  }

  else
  {
    y = rbtree_find_successor(t, p->right);
    y_original_color = y->color; 
    x = y->right;

    if (y->parent == p)
      x->parent = y;
    else 
    {
      rbtree_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rbtree_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if (y_original_color == RBTREE_BLACK)
    erase_fix_up(t, x);
  free(p);
  return 1;
}
////////////////////////////////////////////////////////////////////////////////

void inorder_helper(node_t *node, node_t *nil, key_t *arr, size_t n, int *count)
{
  if (node == nil)
    return;
  inorder_helper(node->left, nil, arr, n, count);
  if (n > *count)
  {
    arr[*count] = node->key;
    (*count)++;
  }
  else
    return;
  inorder_helper(node->right, nil, arr, n, count);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (!t || t->root == t->nil || !arr)
    return 0;
  int count = 0;
  inorder_helper(t->root, t->nil, arr, n, &count);
  
  return count;
}