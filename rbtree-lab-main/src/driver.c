#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 트리를 예쁘게 보여주는 함수랍니다!
void printSimpleTree(node_t *root, int level, char side, node_t *nil) {
    if (root == nil)
        return;

    for (int i = 0; i < level; i++)
        printf("    "); // 들여쓰기 숑숑

    // 터미널 색깔을 위한 ANSI 코드에요! 반짝반짝!
    const char *red_color = "\033[0;31m";
    const char *reset_color = "\033[0m";

    if (root->color == RBTREE_RED) {
        printf("%s", red_color); // 빨강 노드는 특별하게 빨강색으로!
    }
    
    if (side == 'L')
        printf(" L-");
    else if (side == 'R')
        printf(" R-");

    printf("[%d:%s]%s\n", 
           root->key,
           root->color == RBTREE_RED ? "빨강색" : "검정색",
           root->color == RBTREE_RED ? reset_color : ""); // 빨강색이었으면 원래대로 돌아와요! 뿅!
    
    printSimpleTree(root->left, level + 1, 'L', nil);
    printSimpleTree(root->right, level + 1, 'R', nil);
}

// 숫자를 순서대로 나열해주는 함수에요!
void inOrderIterative(node_t *root, node_t *nil) {
    if (root == nil)
        return;
    inOrderIterative(root->left, nil);
    printf("%d ", root->key);
    inOrderIterative(root->right, nil);
}

/* -------------------------------------------------------------------------- */
/* RB-트리가 규칙을 잘 지키는지 검사할 시간! ( •̀ ω •́ )✧           */
/* -------------------------------------------------------------------------- */

// [규칙 4] 빨강 노드의 자식은 모두 검정색인지 볼게요!
int check_red_children_are_black(const node_t *node, const node_t *nil) {
    if (node == nil) {
        return 1;
    }

    if (node->color == RBTREE_RED) {
        if (node->left->color == RBTREE_RED || node->right->color == RBTREE_RED) {
            fprintf(stderr, "규칙 위반! 빨강 노드(%d)가 빨강 자식을 가졌어요! 안돼!\n", node->key);
            return 0;
        }
    }

    return check_red_children_are_black(node->left, nil) && check_red_children_are_black(node->right, nil);
}

// [규칙 5] 모든 길의 검정 노드 개수가 같은지 세어볼까요?
int get_black_height(const node_t *node, const node_t *nil) {
    if (node == nil) {
        return 1; // NIL 노드는 검정색으로 쳐서 높이가 1이에요!
    }

    int left_bh = get_black_height(node->left, nil);
    if (left_bh == -1) return -1;

    int right_bh = get_black_height(node->right, nil);
    if (right_bh == -1) return -1;

    if (left_bh != right_bh) {
        fprintf(stderr, "규칙 위반! 노드(%d)에서 블랙 높이가 달라요! 왼쪽은 %d, 오른쪽은 %d 에요!\n", node->key, left_bh, right_bh);
        return -1;
    }

    return left_bh + (node->color == RBTREE_BLACK);
}

// 모든 규칙을 잘 지켰나 최종 확인!
void validate_rbtree(rbtree *t) {
    if (t == NULL || t->root == t->nil) {
        printf("✅ 트리가 비어있어요. 검사 통과! >_<\n");
        return;
    }

    int is_valid = 1;

    // [규칙 2] 뿌리(루트)는 검정색인가요?
    if (t->root->color != RBTREE_BLACK) {
        fprintf(stderr, "규칙 위반! 루트 노드가 검정색이 아니에요! T^T\n");
        is_valid = 0;
    }

    // [규칙 4] 빨강 노드의 자식들은 모두 검정색?
    if (!check_red_children_are_black(t->root, t->nil)) {
        is_valid = 0;
    }

    // [규칙 5] 모든 길의 검정 높이는 같은가요?
    if (get_black_height(t->root, t->nil) == -1) {
        is_valid = 0;
    }

    if (is_valid) {
        printf("✅ RB 트리 규칙을 아주 잘 지키고 있어요! 참 잘했어요! >_<\n");
    } else {
        printf("❌ 이런! RB 트리 규칙을 어겼어요. 구조가 잘못된 것 같아요! ( •́ ̯•̀ )\n");
    }
}


/* -------------------------------------------------------------------------- */
/* 메인 함수! 여기서 모든 일이 시작돼요! (두근두근)             */
/* -------------------------------------------------------------------------- */

// 메뉴판이에요!
void print_menu() {
    printf("\n----------------------------------\n");
    printf("1. 숫자 여러 개 한 번에 넣기 (예: \"10 20 5 -5\")\n");
    printf("2. 숫자 한 개만 쏙 넣기\n");
    printf("3. 트리 깨끗하게 비우기\n");
    printf("0. 이제 그만하기\n");
    printf("----------------------------------\n");
    printf("어떤 걸 해볼까요?: ");
}

int main(void) {
    rbtree *tree = new_rbtree();
    if (tree == NULL) {
        fprintf(stderr, "트리 만들기에 실패했어요... 힝.\n");
        return 1;
    }

    int choice;
    char input_buffer[1024];

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("잘못된 입력이에요! 숫자를 입력해주세요! (｡•́︿•̀｡)\n");
            continue;
        }
        
        while (getchar() != '\n'); 

        switch (choice) {
            case 1: { // 숫자 여러 개 넣기
                printf("넣고 싶은 숫자들을 띄어쓰기로 구분해서 알려주세요: ");
                if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
                    continue;
                }
                
                char *token = strtok(input_buffer, " \t\n");
                while (token != NULL) {
                    rbtree_insert(tree, atoi(token));
                    token = strtok(NULL, " \t\n");
                }
                break;
            }
            case 2: { // 숫자 한 개 넣기
                int key;
                printf("넣고 싶은 숫자 한 개를 알려주세요: ");
                if (scanf("%d", &key) == 1) {
                     rbtree_insert(tree, key);
                } else {
                    printf("숫자를 잘못 입력했어요.\n");
                }
                while (getchar() != '\n');
                break;
            }
            case 3: { // 트리 비우기
                printf("트리를 깨끗하게 비우고 있어요...\n");
                delete_rbtree(tree);
                tree = new_rbtree();
                break;
            }
            case 0: { // 그만하기
                printf("트리를 지우고 떠날 시간이에요. 안녕! (^_^)/\n");
                delete_rbtree(tree);
                return 0;
            }
            default: {
                printf("없는 번호에요! 다시 골라주세요.\n");
                continue;
            }
        }

        // 작업이 끝나면 결과를 보여줄게요!
        printf("\n\n-- 짜잔! 현재 트리 모습이에요 --\n");
        printSimpleTree(tree->root, 0, ' ', tree->nil);
        printf("\n-- 숫자를 순서대로 나열하면? --\n");
        inOrderIterative(tree->root, tree->nil);
        printf("\n\n-- 규칙을 잘 지키고 있는지 볼까요? --\n");
        validate_rbtree(tree);
        printf("------------------------\n\n");
    }

    return 0;
}