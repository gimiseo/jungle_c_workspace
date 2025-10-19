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
        grandparent->color = RBTREE_RED;
        R_rotate(t, grandparent);
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
        grandparent->color = RBTREE_RED;
        L_rotate(t, grandparent);
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
    else if (cur->key <= key)
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

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
