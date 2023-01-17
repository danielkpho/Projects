#include <stdio.h>
#include <assert.h>
#include "bst.h"

struct _Node {
    // In order for the test to work, the order of left, right and data must be the same with bst.c
    // To standardize, let's all use left, right then data
    struct _Node *left;
    struct _Node *right;
    int data;
};

int main() {
  Node *a, *b;
  a = addNode(NULL, 42);
  b = removeNode(a, 42);
  assert(b == NULL);

  a = addNode(NULL, 1);
  b = addNode(a, 2);

  displaySubtree(a);
  int c = numberLeaves(a);
  assert(c == 1); 

  c = nodeDepth(a,b);
  printf("%d\n", c);
  assert(c == 1);

  a = removeSubtree(a, 2);
  
  Node *n1, *n2, *n3, *n4;
    n1 = addNode(NULL, 5);
    n2 = addNode(n1, 6);
    n3 = addNode(n1, 4);
    assert(n1->left->data == 6);
    assert(n1->right->data == 4);
    printf("test passed\n");
    printf("CTC2: Test the structure of RBST of 5 6 4 3\n");
    n4 = addNode(n1, 3);
    assert(n1->right->right->data == 3);
    printf("test passed\n");
    printf("CTC3: Test numberLeaves correct, n1 should have 2 leaves\n");
    //leaf node means node with no children
    assert(numberLeaves(n1) == 2);
    printf("test passed\n");
    printf("CTC4: Test adding a duplicate node, should not add anything and return NULL\n");
    n3 = addNode(n1, 3);
    assert(n3 == NULL);
    assert(numberLeaves(n1) == 2);
    printf("test passed\n");
    printf("CTC5: Test nodeDepth normal case\n");
    c = nodeDepth(n1, n2);
    assert(c == 1);
    printf("test passed\n");
    printf("CTC5: Test nodeDepth case when node is not inside root\n");
    c = nodeDepth(n1->left, n2);
    printf("%d \n", c);
    assert(c == -1);
    printf("test passed\n");
    printf("CTC6: Test displaySubtree\n");
    displaySubtree(n1);
    printf("-\nthe output should be\n3\n4\n5\n6\n-\n");
    printf("CTC7: Test removeNode, remove 6\n");
    n2 = removeNode(n1, 6);
    assert(n2 == n1);
    assert(n1->left == NULL);
    assert(numberLeaves(n1) == 1);
    printf("test passed\n");
    printf("CTC8, Test removeNode, 1 right child, remove 4\n");
    n2 = removeNode(n1, 4);
    assert(n2 == n1);
    assert(n1->right->data == 3);
    printf("test passed\n");
    printf("CTC9, Test removeNode, 1 left child, add 4 remove 3\n");
    n3 = addNode(n1, 4);
    n2 = removeNode(n1, 3);
    assert(n2 == n1);
    assert(n1->right->data == 4);
    printf("test passed\n");

    printf("CTC10, Test removeNode, 2 child, add 10, 12, 8, then remove 10\n");
    n3 = addNode(n1, 6);
    n3 = addNode(n1, 10);
    n3 = addNode(n1, 12);
    n3 = addNode(n1, 8);
    n2 = removeNode(n1, 10);
    assert(n2 == n1);
    // To accommodate both inorder successor and inorder predecessor
    assert(n1->left->left->data == 8 || n1->left->left->data == 12);
    assert(numberLeaves(n1) == 2);
    printf("test passed\n");
    printf("Removing the entire tree to prevent memory leak\n");
    a = removeSubtree(n1, 5);
    printf("\nNow we are on to Extra Test Cases (ETC)\n");
    Node *newC, *d, *e, *f, *g, *h, *i, *j, *k;
    Node *w, *x, *y, *z;
    Node *test1, *test2, *test3, *test4, *test6, *test7;
    //
    //add
    //
    a = addNode(NULL, 8);
    b = addNode(a, 3);
    newC = addNode(a, 11);
    d = addNode(a, 1);
    e = addNode(a, 6);
    f = addNode(a, 9);
    g = addNode(a, 14);
    h = addNode(a, 7);
    i = addNode(a, 10);
    j = addNode(a, 12);
    k = addNode(a, 15);
    x = addNode(NULL, 90);
    //
    //subtree
    //
    printf("ETC1. Starting test, checking displaySubtree works: \n\n");
    printf("Should print:\n1\n3\n6\n7\n8\n9\n10\n11\n12\n14\n15\n\n");
    printf("Prints \n");
    displaySubtree(a);
    //
    //number leaves
    //
    printf("\nETC2. Testing numberLeaves\n");
    if (numberLeaves(a) == 5) {
        printf("PASS: Tree has 5 leaves\n");
    } else {
        printf("FAIL: Tree has 5 leaves\n");
    }
    //
    //node depth
    //
    printf("\nETC3. Testing nodeDepth\n");
    if (nodeDepth(a, a) == 0) {
        printf("PASS: Root has depth of 0\n");
    } else if (nodeDepth(a, a) != 0) {
        printf("FAIL: Root should have depth of 0\n");
    }
    if (nodeDepth(a, b) == 1) {
        printf("PASS: 3 has have depth of 1\n");
    } else if (nodeDepth(a, b) != 1) {
        printf("FAIL: 3 should have depth of 1\n");
    }
    if (nodeDepth(a, k) == 3) {
        printf("PASS: 15 have depth of 3\n");
    } else if (nodeDepth(a, k) != 3) {
        printf("FAIL: 15 should have depth of 3\n");
    }
    /*if (nodeDepth(a, x) == -1) {
        printf("PASS: 90 is not in tree\n");
    } else if (nodeDepth(a, k) != -1) {
        printf("FAIL: 90 is not in tree, should return -1\n");
    }
    if (nodeDepth(a, NULL) == -1) {
        printf("PASS: NULL cant be in the tree\n");
    } else if (nodeDepth(a, NULL) != -1) {
        printf("FAIL: NULL cant be in the tree, should return -1\n");
    }
    if (nodeDepth(NULL, NULL) == -1) {
        printf("PASS: NULL cant be in the NULL tree\n");
    } else if (nodeDepth(NULL, NULL) != -1) {
        printf("FAIL: NULL cant be in the NULL tree, should return -1\n");
    }*/
    //
    //Deletion
    //
    printf("\nETC4. Check that the pointers are the same \n");
    y = removeNode(a, 11);
    if (y == a) {
        printf("PASS: Pointers match\n");
    } else {
        printf("FAIL: Pointers didn't match\n");
    }
    printf("\nETC5. Testing removeNode worked, should print:\n1\n3\n6\n7\n8\n9\n10\n12\n14\n15\n\nPrints\n");
    displaySubtree(a);
    printf("\nETC6. Testing numberLeaves\n");
    if (numberLeaves(a) == 4 || numberLeaves(a) == 5) {
        printf("PASS: Tree has 4 or 5 leaves\n");
    } else {
        printf("FAIL: Tree does not have 4 or 5 leaves\n");
    }
    printf("\nTesting removeNode, removing root 8\n");
    z = removeNode(a, 8);
    printf("Should print:\n1\n3\n6\n7\n9\n10\n12\n14\n15\n\nPrints\n");
    displaySubtree(z);
    printf("\nETC7. Testing numberLeaves with new root\n");
    if (numberLeaves(z) == 4 || numberLeaves(z) == 5) {
        printf("PASS: Tree has 4 or 5 leaves\n");
    } else {
        printf("FAIL: Tree does not have 4 or 5 leaves\n");
    }
    removeSubtree(z, z->data);
    removeNode(x, x->data);
  return 0;
}
