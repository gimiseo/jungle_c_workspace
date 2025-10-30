#include "avl.h"
#include <stdio.h> // NULL 정의를 위해 (find_best_fit 반환용)

/*
 * ★★★ 핵심: nil 센티널 노드 ★★★
 */
static avl_node_t nil_sentinel;

/*
 * ----------------------------------------------------------------- 
 * 내부 헬퍼 함수 (Static)
 * -----------------------------------------------------------------
 */

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief 노드의 높이를 자식들을 기준으로 갱신합니다.
 * nil 노드의 높이는 0이므로, if문이 필요 없습니다.
 */
static void update_height(avl_node_t *node) {
    node->height = 1 + MAX(node->left->height, node->right->height);
}

/**
 * @brief 노드의 밸런스 팩터(BF)를 계산합니다.
 * nil 노드의 높이는 0이므로, if문이 필요 없습니다.
 */
static int get_balance_factor(avl_node_t *node) {
    return node->left->height - node->right->height;
}

/**
 * @brief u를 v로 교체 (부모 링크만 수정)
 * CLRS 수도코드와 유사하며, v가 nil일 때도 동작합니다.
 */
static void transplant(avl_tree_t *tree, avl_node_t *u, avl_node_t *v) {
    if (u->parent == &nil_sentinel) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    // v가 nil일지라도, nil의 parent를 설정합니다.
    // nil의 parent는 어차피 사용되지 않으므로 괜찮습니다.
    v->parent = u->parent;
}

/**
 * @brief 왼쪽 회전 (x 기준)
 *  x                  y
 * / \                / \
 * A   y      =>      x   C
 * / \               /     \
 * B   C          A         B
 */
static avl_node_t *rotate_left(avl_tree_t *tree, avl_node_t *x) {
    avl_node_t *y = x->right;

    // 1. y의 왼쪽 서브트리를 x의 오른쪽으로
    x->right = y->left;
    if (y->left != &nil_sentinel) {
        y->left->parent = x;
    }

    // 2. x의 부모를 y의 부모로
    transplant(tree, x, y);

    // 3. x를 y의 왼쪽 자식으로
    y->left = x;
    x->parent = y;

    // 4. 높이 갱신 (순서 중요: 자식(x) -> 부모(y))
    update_height(x);
    update_height(y);
    
    return y; // 새 서브트리의 루트 반환
}

/**
 * @brief 오른쪽 회전 (y 기준)
 * y                x
 * / \              / \
 * x   C    =>      A   y
 * / \                / \
 * A   B              B   C
 */
static avl_node_t *rotate_right(avl_tree_t *tree, avl_node_t *y) {
    avl_node_t *x = y->left;

    // 1. x의 오른쪽 서브트리를 y의 왼쪽으로
    y->left = x->right;
    if (x->right != &nil_sentinel) {
        x->right->parent = y;
    }

    // 2. y의 부모를 x의 부모로
    transplant(tree, y, x);

    // 3. y를 x의 오른쪽 자식으로
    x->right = y;
    y->parent = x;

    // 4. 높이 갱신 (순서 중요: 자식(y) -> 부모(x))
    update_height(y);
    update_height(x);

    return x; // 새 서브트리의 루트 반환
}

/**
 * @brief 삽입/삭제 후 노드 x부터 루트까지 올라가며 재조정
 */
static void rebalance_upwards(avl_tree_t *tree, avl_node_t *x) {
    while (x != &nil_sentinel) {
        update_height(x);
        int bf = get_balance_factor(x);

        avl_node_t *new_subtree_root = x;

        if (bf > 1) {
            if (get_balance_factor(x->left) < 0) {
                x->left = rotate_left(tree, x->left);
            }
            new_subtree_root = rotate_right(tree, x);
        }
        else if (bf < -1) {
            if (get_balance_factor(x->right) > 0) {
                x->right = rotate_right(tree, x->right);
            }
            new_subtree_root = rotate_left(tree, x);
        }
        
        x = new_subtree_root->parent; // 회전 후의 부모로 이동
    }
}

/**
 * @brief Best-fit 헬퍼 (재귀)
 */
static avl_node_t *find_best_fit_helper(avl_node_t *node, size_t size, avl_node_t *best) {
    if (node == &nil_sentinel) {
        return best;
    }

    if (node->size >= size) {
        // 현재 노드가 적합함. 더 좋은 (작은) 노드가 왼쪽에 있는지 탐색
        best = node;
        return find_best_fit_helper(node->left, size, best);
    } else {
        // 현재 노드가 너무 작음. 오른쪽에서만 탐색
        return find_best_fit_helper(node->right, size, best);
    }
}

/**
 * @brief (Public) 서브트리에서 최소 노드 찾기
 */
avl_node_t *avl_get_minimum(avl_node_t *node) {
    while (node->left != &nil_sentinel) {
        node = node->left;
    }
    return node;
}


/*
 * ----------------------------------------------------------------- 
 * Public API 함수 구현
 * -----------------------------------------------------------------
 */

/**
 * @brief AVL 트리 초기화
 */
void avl_init(avl_tree_t *tree) {
    // nil 센티널 노드를 단 한 번 초기화합니다.
    nil_sentinel.size = 0;
    nil_sentinel.height = 0; // ★★★ AVL의 핵심: nil의 높이는 0 ★★★
    nil_sentinel.left = &nil_sentinel;
    nil_sentinel.right = &nil_sentinel;
    nil_sentinel.parent = &nil_sentinel;

    // 트리의 루트를 nil로 설정합니다.
    tree->root = &nil_sentinel;
}

/**
 * @brief 트리에 새 노드 삽입 (BST 삽입 + 재조정)
 */
void avl_insert(avl_tree_t *tree, avl_node_t *z) {
    avl_node_t *y = &nil_sentinel; // 삽입될 위치의 부모
    avl_node_t *x = tree->root;    // 삽입될 위치 탐색

    // 1. 표준 BST 삽입 위치 찾기
    while (x != &nil_sentinel) {
        y = x;
        // Malloc Lab: 크기가 같으면 RBT처럼 왼쪽으로 보냄 (중복 허용)
        if (z->size < x->size) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    // 2. 새 노드(z) 초기화 및 연결
    z->parent = y;
    if (y == &nil_sentinel) {
        tree->root = z; // 트리가 비어있었음
    } else if (z->size < y->size) {
        y->left = z;
    } else {
        y->right = z;
    }

    z->left = &nil_sentinel;
    z->right = &nil_sentinel;
    z->height = 1; // 새 리프 노드의 높이는 1

    // 3. 부모(y)부터 루트까지 올라가며 높이 갱신 및 재조정
    rebalance_upwards(tree, y);
}

/**
 * @brief 트리에서 노드 삭제 (BST 삭제 + 재조정)
 */
void avl_delete(avl_tree_t *tree, avl_node_t *z) {
    avl_node_t *y; // 실제로 트리에서 '제거'될 노드 (z 또는 z의 후계자)
    avl_node_t *x; // y의 자리를 '대체'할 노드 (y의 자식 중 하나)

    // 1. 표준 BST 삭제 (CLRS 버전)
    // y: 제거될 노드 결정
    if (z->left == &nil_sentinel || z->right == &nil_sentinel) {
        y = z; // z가 자식이 0~1개인 경우, z를 제거
    } else {
        y = avl_get_minimum(z->right); // z가 자식이 2개인 경우, 후계자(y)를 제거
    }

    // x: y의 자리를 대체할 노드 결정
    if (y->left != &nil_sentinel) {
        x = y->left;
    } else {
        x = y->right;
    }

    // y를 트리에서 떼어냄 (x가 y의 자리를 대체)
    transplant(tree, y, x);

    // 재조정 시작점: x의 (새) 부모부터 시작
    avl_node_t *rebalance_start_node = x->parent;

    // 만약 z가 아닌 후계자(y)를 제거했다면,
    // y의 데이터를 z로 '복사'하는 대신, y로 z를 '교체'합니다.
    if (y != z) {
        // z를 y로 교체 (z의 링크들을 y로 이식)
        transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->right = z->right;
        y->right->parent = y;
        update_height(y);
        
        // 만약 y가 z의 직계 자식이 아니었다면, 
        // 재조정 시작점이 (y의 원래 부모)로 변경되어야 함
        if (rebalance_start_node == z) {
             rebalance_start_node = y;
        }
    }

    // 2. 재조정 시작점(x의 부모)부터 루트까지 올라가며 재조정
    rebalance_upwards(tree, rebalance_start_node);
}

/**
 * @brief Best-fit 검색
 */
avl_node_t *avl_find_best_fit(avl_tree_t *tree, size_t size) {
    avl_node_t *best = find_best_fit_helper(tree->root, size, &nil_sentinel);
    
    // nil을 반환하는 대신, C 표준인 NULL을 반환합니다.
    if (best == &nil_sentinel) {
        return NULL;
    } else {
        return best;
    }
}