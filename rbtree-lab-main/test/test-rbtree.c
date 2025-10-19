#include "rbtree.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// fork와 waitpid, 그리고 signal 설명을 위한 헤더 파일이에요!
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

/* -------------------------------------------------------------------------- */
/* 기존 테스트 함수들은 수정 없이 그대로 사용해요!                               */
/* -------------------------------------------------------------------------- */
void test_init(void) {
  rbtree *t = new_rbtree();
  assert(t != NULL);
#ifdef SENTINEL
  assert(t->nil != NULL);
  assert(t->root == t->nil);
#else
  assert(t->root == NULL);
#endif
  delete_rbtree(t);
}

void test_insert_single(const key_t key) {
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);
  assert(p != NULL);
  assert(t->root == p);
  assert(p->key == key);
#ifdef SENTINEL
  assert(p->left == t->nil);
  assert(p->right == t->nil);
  assert(p->parent == t->nil);
#else
  assert(p->left == NULL);
  assert(p->right == NULL);
  assert(p->parent == NULL);
#endif
  delete_rbtree(t);
}

void test_find_single(const key_t key, const key_t wrong_key) {
  rbtree *t = new_rbtree();
  rbtree_insert(t, key);
  node_t *q = rbtree_find(t, key);
  assert(q != NULL);
  assert(q->key == key);
  q = rbtree_find(t, wrong_key);
  assert(q == NULL);
  delete_rbtree(t);
}

void test_erase_root(const key_t key) {
  rbtree *t = new_rbtree();
  node_t *p = rbtree_insert(t, key);
  rbtree_erase(t, p);
#ifdef SENTINEL
  assert(t->root == t->nil);
#else
  assert(t->root == NULL);
#endif
  delete_rbtree(t);
}

static void insert_arr(rbtree *t, const key_t *arr, const size_t n) {
  for (size_t i = 0; i < n; i++) {
    rbtree_insert(t, arr[i]);
  }
}

static int comp(const void *p1, const void *p2) {
  const key_t *e1 = p1;
  const key_t *e2 = p2;
  if (*e1 < *e2) return -1;
  if (*e1 > *e2) return 1;
  return 0;
}

void test_minmax(key_t *arr, const size_t n) {
  assert(n > 0 && arr != NULL);
  rbtree *t = new_rbtree();
  insert_arr(t, arr, n);
  qsort(arr, n, sizeof(key_t), comp);
  node_t *p = rbtree_min(t);
  assert(p->key == arr[0]);
  node_t *q = rbtree_max(t);
  assert(q->key == arr[n - 1]);
  delete_rbtree(t);
}

void test_to_array(rbtree *t, const key_t *arr, const size_t n) {
  insert_arr(t, arr, n);
  qsort((void *)arr, n, sizeof(key_t), comp);
  key_t *res = calloc(n, sizeof(key_t));
  rbtree_to_array(t, res, n);
  for (size_t i = 0; i < n; i++) {
    assert(arr[i] == res[i]);
  }
  free(res);
}

void test_multi_instance() {
  rbtree *t1 = new_rbtree(), *t2 = new_rbtree();
  key_t arr1[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  const size_t n1 = sizeof(arr1) / sizeof(arr1[0]);
  insert_arr(t1, arr1, n1);
  key_t arr2[] = {4, 8, 10, 5, 3};
  const size_t n2 = sizeof(arr2) / sizeof(arr2[0]);
  insert_arr(t2, arr2, n2);
  delete_rbtree(t2);
  delete_rbtree(t1);
}

bool search_traverse(const node_t *p, key_t *min, key_t *max, node_t *nil) {
  if (p == nil) return true;
  *min = *max = p->key;
  key_t l_min, l_max, r_min, r_max;
  if (!search_traverse(p->left, &l_min, &l_max, nil) || l_max > p->key) return false;
  if (!search_traverse(p->right, &r_min, &r_max, nil) || r_min < p->key) return false;
  *min = (p->left == nil) ? p->key : l_min;
  *max = (p->right == nil) ? p->key : r_max;
  return true;
}

void test_search_constraint(const rbtree *t) {
  node_t *nil = t->nil;
  key_t min, max;
  assert(search_traverse(t->root, &min, &max, nil));
}

bool color_traverse(const node_t *p, int black_depth, int *path_black_depth, node_t *nil) {
    if (p == nil) {
        if (*path_black_depth == -1) *path_black_depth = black_depth;
        return *path_black_depth == black_depth;
    }
    if (p->color == RBTREE_RED) {
        if (p->left->color == RBTREE_RED || p->right->color == RBTREE_RED) return false;
    }
    int next_depth = black_depth + (p->color == RBTREE_BLACK);
    return color_traverse(p->left, next_depth, path_black_depth, nil) && color_traverse(p->right, next_depth, path_black_depth, nil);
}

void test_color_constraint(const rbtree *t) {
  assert(t->root->color == RBTREE_BLACK);
  int path_black_depth = -1;
  assert(color_traverse(t->root, 0, &path_black_depth, t->nil));
}


void test_rb_constraints(const key_t arr[], const size_t n) {
  rbtree *t = new_rbtree();
  insert_arr(t, arr, n);
  test_search_constraint(t);
  test_color_constraint(t);
  delete_rbtree(t);
}

void test_distinct_values() {
  const key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  test_rb_constraints(entries, sizeof(entries) / sizeof(entries[0]));
}

void test_duplicate_values() {
  const key_t entries[] = {10, 5, 5, 34, 6, 23, 12, 12, 6, 12};
  test_rb_constraints(entries, sizeof(entries) / sizeof(entries[0]));
}

void test_minmax_suite() {
  key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  test_minmax(entries, sizeof(entries) / sizeof(entries[0]));
}

void test_to_array_suite() {
  rbtree *t = new_rbtree();
  key_t entries[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  test_to_array(t, entries, sizeof(entries) / sizeof(entries[0]));
  delete_rbtree(t);
}

void test_find_erase(rbtree *t, const key_t *arr, const size_t n) {
  for (size_t i = 0; i < n; i++) rbtree_insert(t, arr[i]);
  for (size_t i = 0; i < n; i++) {
    node_t *p = rbtree_find(t, arr[i]);
    assert(p != NULL && p->key == arr[i]);
    rbtree_erase(t, p);
  }
  for (size_t i = 0; i < n; i++) assert(rbtree_find(t, arr[i]) == NULL);
}

void test_find_erase_fixed() {
  const key_t arr[] = {10, 5, 8, 34, 67, 23, 156, 24, 2, 12};
  rbtree *t = new_rbtree();
  test_find_erase(t, arr, sizeof(arr) / sizeof(arr[0]));
  delete_rbtree(t);
}

void test_find_erase_rand(const size_t n, const unsigned int seed) {
  srand(seed);
  rbtree *t = new_rbtree();
  key_t *arr = calloc(n, sizeof(key_t));
  for (size_t i = 0; i < n; i++) arr[i] = rand();
  test_find_erase(t, arr, n);
  free(arr);
  delete_rbtree(t);
}

/* -------------------------------------------------------------------------- */
/* 여기서부터가 진짜 마법이에요! ✨ fork로 안전하게 테스트해요!        */
/* -------------------------------------------------------------------------- */

const char *get_test_description(const char *test_name) {
  if (strcmp(test_name, "test_init") == 0) return "새로운 RB-Tree가 올바르게 초기화되는지 확인해요! (tree->root == tree->nil)";
  if (strcmp(test_name, "test_insert_single") == 0) return "노드 한 개를 삽입했을 때, root가 잘 설정되는지 확인해요!";
  if (strcmp(test_name, "test_find_single") == 0) return "노드 한 개를 넣고, 잘 찾아지는지 확인해요!";
  if (strcmp(test_name, "test_erase_root") == 0) return "단 하나뿐인 루트 노드를 지웠을 때, 트리가 비어있는 상태가 되는지 확인해요!";
  if (strcmp(test_name, "test_find_erase_fixed") == 0) return "미리 정해진 값들을 넣고, 순서대로 잘 지워지는지 확인해요!";
  if (strcmp(test_name, "test_minmax_suite") == 0) return "여러 값을 넣었을 때, 최솟값과 최댓값을 잘 찾는지 확인해요!";
  if (strcmp(test_name, "test_to_array_suite") == 0) return "트리의 모든 값을 배열로 잘 변환하는지 확인해요!";
  if (strcmp(test_name, "test_distinct_values") == 0) return "중복 없는 값들을 넣었을 때, RB-Tree의 규칙(색깔, 순서)을 잘 지키는지 확인해요!";
  if (strcmp(test_name, "test_duplicate_values") == 0) return "중복 있는 값들을 넣었을 때, RB-Tree의 규칙(색깔, 순서)을 잘 지키는지 확인해요!";
  if (strcmp(test_name, "test_multi_instance") == 0) return "여러 개의 트리를 동시에 만들어도 서로 영향을 주지 않는지 확인해요!";
  if (strcmp(test_name, "test_find_erase_rand") == 0) return "랜덤한 값들을 넣고 지웠을 때도, 문제가 없는지 확인해요!";
  return "설명이 없는 테스트에요...";
}

#define RUN_TEST(test_func, ...)                                               \
  do {                                                                         \
    char answer;                                                               \
    printf("\n----------------------------------\n");                           \
    printf("👉 다음 테스트를 실행해볼까요? [%s(%s)] (y/n): ", #test_func, #__VA_ARGS__); \
    scanf(" %c", &answer);                                                     \
    while (getchar() != '\n');                                                 \
    if (answer == 'y' || answer == 'Y') {                                      \
      pid_t pid = fork();                                                      \
      if (pid == -1) {                                                         \
        perror("fork"); exit(1);                                               \
      } else if (pid == 0) { /* 자식 프로세스 */                                \
        printf("🚀 테스트 시작!\n");                                            \
        test_func(__VA_ARGS__);                                                  \
        exit(0); /* 성공! */                                                   \
      } else { /* 부모 프로세스 */                                             \
        int status;                                                            \
        waitpid(pid, &status, 0);                                              \
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {                   \
          /* [수정!] 성공했을 때도 자세한 정보를 보여줘요! */                  \
          printf("\n✅ 우와! [%s] 테스트를 통과했어요! 정말 대단해요! 짝짝짝! 🥳\n", #test_func); \
          printf("   이 테스트는... '%s'를 확인하는 중이었어요.\n", get_test_description(#test_func)); \
          printf("   호출된 함수: %s(%s)\n", #test_func, #__VA_ARGS__);       \
        } else {                                                               \
          printf("\n❌ 이런! [%s] 테스트에서 문제가 발생했어요! ( •́ ̯•̀ )\n\n", #test_func); \
          printf("   이 테스트는... '%s'를 확인하는 중이었어요.\n", get_test_description(#test_func)); \
          printf("   호출된 함수: %s(%s)\n", #test_func, #__VA_ARGS__);       \
          if (WIFSIGNALED(status)) { /* 시그널로 종료되었을 때 */              \
            printf("   발생한 오류: %s (Signal %d)\n", strsignal(WTERMSIG(status)), WTERMSIG(status)); \
            if (WTERMSIG(status) == SIGABRT) {                                 \
                printf("   (힌트: assert() 문이 실패하면 보통 이 오류가 발생해요!)\n"); \
            } else if (WTERMSIG(status) == SIGSEGV) {                          \
                printf("   (힌트: 잘못된 메모리 주소에 접근했어요! 포인터를 확인해보세요!)\n"); \
            }                                                                  \
          }                                                                    \
          printf("   아마 이 기능이 아직 구현되지 않았거나, 버그가 있는 것 같아요!\n\n"); \
          printf("   프로그램을 종료합니다. 힘내세요! (๑•̀ㅂ•́)و✧\n\n\n");              \
          exit(1);                                                             \
        }                                                                      \
      }                                                                        \
    } else {                                                                   \
      printf("🥺 테스트를 건너뛸게요. 다음에 또 만나요!\n");                    \
      return 0;                                                                \
    }                                                                          \
  } while (0)

int main(void) {
  printf("🎉 RB-Tree 테스트를 시작합니다! 🎉\n");

  RUN_TEST(test_init);
  RUN_TEST(test_insert_single, 1024);
  RUN_TEST(test_find_single, 512, 1024);
  RUN_TEST(test_distinct_values);
  RUN_TEST(test_duplicate_values);
  RUN_TEST(test_multi_instance);
  
  // 아직 구현 중인 함수들은 이 아래에 모아두면 좋아요!
  RUN_TEST(test_erase_root, 128);
  RUN_TEST(test_find_erase_fixed);
  RUN_TEST(test_minmax_suite);
  RUN_TEST(test_to_array_suite);
  RUN_TEST(test_find_erase_rand, 1000, 17);

  printf("\n\n🎊 모든 테스트를 완료했어요! 완벽해요! 🎊\n");
  return 0;
}
