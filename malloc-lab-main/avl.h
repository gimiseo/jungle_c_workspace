#ifndef AVL_H
#define AVL_H

#include <stddef.h> // for size_t

/*
 * AVL 트리 노드 구조체
 * Malloc Lab의 가용 블록 payload에 이 구조체가 
 * 덮어씌워진다고 가정합니다.
 */
typedef struct avl_node {
    size_t size;                // 블록 크기 (이것이 트리의 '키' 입니다)
    struct avl_node *left;      // 왼쪽 자식
    struct avl_node *right;     // 오른쪽 자식
    struct avl_node *parent;    // 부모 노드 (삽입/삭제 시 재조정에 필수)
    int height;                 // 이 노드를 루트로 하는 서브트리의 높이
} avl_node_t;

/*
 * AVL 트리 전체를 관리하는 구조체
 * 루트 포인터만 가집니다.
 * (nil 노드는 avl.c 내부에 static 전역 변수로 숨겨집니다)
 */
typedef struct avl_tree {
    avl_node_t *root;
} avl_tree_t;


/*
 * Public API
 */

/**
 * @brief AVL 트리를 초기화합니다.
 * (내부적으로 nil 센티널 노드를 초기화합니다.)
 * @param tree 초기화할 트리 포인터
 */
void avl_init(avl_tree_t *tree);

/**
 * @brief 트리에 새 노드를 삽입하고 재조정합니다.
 * @param tree 트리 포인터
 * @param node 삽입할 노드 (size, left, right, parent, height는 내부에서 설정됨)
 */
void avl_insert(avl_tree_t *tree, avl_node_t *node);

/**
 * @brief 트리에서 특정 노드를 삭제하고 재조정합니다.
 * @param tree 트리 포인터
 * @param node 삭제할 노드 포인터
 */
void avl_delete(avl_tree_t *tree, avl_node_t *node);

/**
 * @brief Best-fit으로 노드를 검색합니다.
 * @param tree 트리 포인터
 * @param size 요청하는 최소 블록 크기
 * @return size보다 크거나 같은 노드 중 가장 작은 노드. 없으면 NULL.
 */
avl_node_t *avl_find_best_fit(avl_tree_t *tree, size_t size);

/**
 * @brief (디버깅용) 트리의 최소 노드를 반환합니다.
 */
avl_node_t *avl_get_minimum(avl_node_t *node);

#endif // AVL_H