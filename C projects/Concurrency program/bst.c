#include <stdio.h>
#include <stdlib.h>
#include "bst.h"
#include <assert.h>

typedef struct _Node // Declaring a struct Node
{ 
    struct _Node *left;
    struct _Node *right;
    int data;
}_Node;

// Function to create parent node
Node * parentNode(Node * root, int data) 
{
  Node* next;
  if (data > root-> data)
    next = root->left;
  else
    next = root->right;

  if (next == NULL || next-> data == data) // if next is null or data is the same as data
    return root;                            // return the root
  else
    return parentNode(next, data);
}

// Create node function 
Node * createNode(int data) 
{
  Node * node = (Node *) malloc(sizeof(Node)); // Allocating memory for newly created node// if memory allocation is successful
  node->data = data;
  node->left = node->right = NULL;
  return node;
}

Node* addNode(Node *root, int data)
{
  Node *newNode = malloc(sizeof(Node));
  newNode->data = data;
  newNode->left = NULL;
  newNode->right = NULL;

  Node *temp;
  Node *prev = NULL;

  temp = root;
  while(temp) {
    prev = temp;
    if (temp->data > newNode->data)
      temp = temp->right;
    else if (temp->data < newNode->data)
      temp = temp->left;
    else {
      free(newNode);
      return NULL;
    }
  }
  if (prev==NULL)
    root = newNode;
  else {
    if (prev->data > newNode->data)
      prev->right = newNode;
    else
      prev->left = newNode;
  }
  return root;
}

Node* removeNode(Node* root, int data)
{
  // Ensure root isn't null.
  if (root == NULL) {
    return NULL;
  }
  
  if (data > root->data) {  // data is in the left sub-tree.
    root->left = removeNode(root->left, data);
  } else if (data < root->data) { // data is in the right sub-tree.
    root->right = removeNode(root->right, data);
  } else { // Found the correct node with data
    // Check the three cases - no child, 1 child, 2 child...
    // No Children
    if (root->left == NULL && root->right == NULL) {
      free(root);
      root = NULL;
    }
    // 1 child (on the right)
    else if (root->left == NULL) {
      Node *temp = root; // save current node
      root = root->right;
      free(temp);
    }
    // 1 child (on the left)
    else if (root->right == NULL) {
      Node *temp = root; // save current node
      root = root->left;
      free(temp);
    }
    // Two children
    else {
      // find minimal data of right sub tree
      Node *temp = root->left; 
      while(temp->right != NULL) {
        temp = temp->right;
      }
      root->data = temp->data; // duplicate the node
      root->left = removeNode(root->left, root->data); // delete the duplicate node
    }
  }
  return root; // parent node can update reference
}
//Function to print Subtree

void displaySubtree(Node *N)
{
    if (N == NULL) // Return nothing when there is no subtree
        return;
    displaySubtree(N->right); 
    printf("%d\n", N->data);
    displaySubtree(N->left);
}

//Function to count leaves

int numberLeaves(Node *N)
{
    if (N == NULL) // Return 0 when there no nodes
    {
        return 0;
    }
    if (N->left == NULL && N->right == NULL) // Return 1 when there are no children of a node (it is a leaf)
    {
        return 1;
    }
    else // Return sum of leaves on left and right side of tree 
    {
        return numberLeaves(N->left) + numberLeaves(N->right);
    }
}

Node * freeSubtree(Node * root) {
  if (root == NULL)
    return NULL;
  freeSubtree(root->left);
  freeSubtree(root->right);
  free(root);
}
/*
   Deletes all nodes that belong to the subtree (of the tree of rooted in 'root')
   whose root node has data 'data'
 */
Node * removeSubtree(Node * root, int data) {
  // empty tree
  if (root == NULL)
    return NULL;
  // entire tree
  if (root->data == data) {
    freeSubtree(root);
    return NULL;
  }
  // free tree rooted in the left or right node and set the respective pointer to NULL
  Node * parent = parentNode(root, data);
  if (data > parent->data) {
    freeSubtree(parent->left);
    parent->left = NULL;
  } else {
    freeSubtree(parent->right);
    parent->right = NULL;
  }
  return root;
}
//Find the depth between the root and the node with the given data
int nodeDepth(Node *root, Node *N)
{
    
    if (root == NULL) // if root does not exist, return -1
    {
        return -1;
    } 
    else if (root->data == N->data) // if root data is equal to N data, return 0
    {
        return 0;
    } 
    else if(root->data > N->data) // else if root data is greater than N data, return 1 + nodeDepth of right subtree
    {
        return 1 + nodeDepth(root->right, N);
    }
    else // else return 1 + nodeDepth of left subtree
    {
        return 1 + nodeDepth(root->left,N);
    }
    
}

int countNodes(Node *N){
    int count = 0;
    // If the node is NULL, then the function returns 0
    if (N == NULL){
        return 0;
    }else{
        // If the node is not NULL, then the function is called recursively on the right and left sub trees
        count = 1 + countNodes(N -> left) + countNodes(N -> right);
    }
    // The count is returned
    return count;
}

float sumNodes(Node *N){
    float sum = 0;
    // If the node is NULL, then the function returns 0
    if (N == NULL){
        return 0;
    }else{
        // If the node is not NULL, then the function is called recursively on the right and left sub trees
        sum = N -> data + sumNodes(N -> left) + sumNodes(N -> right);
    }
    // The sum is returned
    return sum;
}

float avgSubtree (Node *N){
    float count = 0;
    float sum = 0;
    // If the node is NULL, then the function returns 0
    if (N == NULL){
        return 0;
    }else{
        // If the node is not NULL, then the function is called recursively on the right and left sub trees
        count = countNodes(N);
        sum = sumNodes(N);
    }
    // The average is returned
    return sum/count;
}
void inorderTreeWalk(Node *N, int *array, int *i){
    // If the node is NULL, then the function returns
    if (N == NULL){
        return;
    }else{
        // If the node is not NULL, then the function is called recursively on the right and left sub trees
        inorderTreeWalk(N -> left, array, i);
        array[*i] = N -> data;
        *i = *i + 1;
        inorderTreeWalk(N -> right, array, i);
    }
}

Node * arrayToTree(int *array, int start, int end){
    Node *root = NULL;
    int mid;
    // If the node is NULL, then the function returns NULL
    if (start > end){
        return NULL;
    }else{
        // If the node is not NULL, then the function is called recursively on the right and left sub trees
        mid = (start + end)/2;
        root = addNode(root, array[mid]);
        root -> left = arrayToTree(array, start, mid-1);
        root -> right = arrayToTree(array, mid+1, end);
    }
    // The balanced tree is returned
    return root;
}
Node * balanceTree (Node *N){
    Node *root = NULL;
    Node *temp = NULL;
    int i = 0;
    int count = 0;
    int *array = NULL;
    // If the node is NULL, then the function returns NULL
    if (N == NULL){
        return NULL;
    }else{
        // If the node is not NULL, then the function is called recursively on the right and left sub trees
        count = countNodes(N);
        array = malloc(count*sizeof(int));
        temp = N;
        inorderTreeWalk(temp, array, &i);
        root = arrayToTree(array, 0, count-1);
        free (array);
    }
    // The balanced tree is returned
    return root;
}


