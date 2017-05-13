//
//  MCTree.c
//  Sapindus
//
//  Created by YuliSun on 05/12/2016.
//  Copyright © 2016 oreisoft. All rights reserved.
//

#include "MCTree.h"

BSTNode* BSTNodeCreate(MCGeneric value) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    node->value = value;
    node->left  = null;
    node->right = null;
    return node;
}

oninit(MCBST)
{
    if (init(MCObject)) {
        var(root) = null;
        var(count) = 0;
        return obj;
    }else{
        return null;
    }
}

static BSTNode* freenode(BSTNode* node) {
    if (node) {
        free(node);
        return null;
    }
    return node;
}

static BSTNode* printnode(BSTNode* node) {
    if (node) {
        printf("node.value=%.2f [left=%p right=%p]\n",
               node->value.mcfloat, node->left, node->right);
    }
    return node;
}

method(MCBST, void, bye, voida)
{
    MCBST_traverseTree(0, obj, freenode);
}

function(BSTNode*, insert, BSTNode* root, MCGeneric newval)
{
    as(MCBST);
    if (!root) {
        root = BSTNodeCreate(newval);
        obj->count++;
    }
    if (MCGenericCompare(newval, root->value) < 0)
        root->left = insert(0, obj, root->left, newval);
    if (MCGenericCompare(newval, root->value) > 0)
        root->right = insert(0, obj, root->right, newval);
    return root;
}

function(void, traverse, BSTNode* root, BSTNode* (*funcptr)(BSTNode* node))
{
    as(MCBST);
    if (!root) return;
    if (root->left)
        traverse(0, obj, root->left, funcptr);
    root = (*funcptr)(root);
    if (root->right)
        traverse(0, obj, root->right, funcptr);
}

method(MCBST, void, insertValue, MCGeneric newval)
{
    var(root) = insert(0, obj, var(root), newval);
}

method(MCBST, void, traverseTree, BSTNode* (*funcptr)(BSTNode* node))
{
    traverse(0, obj, var(root), funcptr);
}

method(MCBST, void, printTree, voida)
{
    traverse(0, null, var(root), printnode);
    printf("total %ld nodes\n", obj->count);
}

onload(MCBST)
{
    if (load(MCObject)) {
        binding(MCBST, void, bye, voida);
        binding(MCBST, void, insertValue, MCGeneric newval);
        binding(MCBST, void, traverseTree, void (*funcptr)(BSTNode* node));
        binding(MCBST, void, printTree, voida);
        return cla;
    }else{
        return null;
    }
}

/*
 Trie Tree (Digital, Radix, Prefix - Tree)
 */

function(TrieNode*, createNode, char byte);
function(void, releaseNode, TrieNode* node);

oninit(MCTrie)
{
    if (init(MCObject)) {
        var(root)  = createNode(0, 0, ' ');
        var(count) = 0;
        return obj;
    } else {
        return null;
    }
}

method(MCTrie, void, bye, voida)
{
    releaseNode(0, 0, obj->root);
    superbye(MCObject);
}

function(TrieNode*, createNode, char byte)
{
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    node->isLeaf = false;
    node->byte = byte;
    for (int i=0; i<MCTrieWidth; i++)
        node->childs[i] = null;
    return node;
}

function(void, releaseNode, TrieNode* node)
{
    if (node) {
        for (int i=0; i<MCTrieWidth; i++)
            if (node->childs[i])
                releaseNode(0, 0, node->childs[i]);
        free(node);
    }
}

//return current node
function(TrieNode*, insertNodeIntoParent, TrieNode* parent, TrieNode* node)
{
    if (parent && node) {
        TrieNode* current = parent->childs[node->byte];
        if (current == null) {
            parent->childs[node->byte] = node;
            return node;
        } else {
            return current;
        }
    }
    return null;
}

//return Leaf node
function(TrieNode*, insertWordIntoParent, TrieNode* parent, const char* word)
{
    as(MCTrie);
    if (parent && word) {
        size_t len = strlen(word);
        TrieNode *node=null, *p=parent;
        for (size_t i=0; i<len; i++) {
            char c = *word;
            node = createNode(0, obj, c);
            //update parent
            p = insertNodeIntoParent(0, obj, p, node);
            word++;
        }
        if (node) {
            //last node is leaf
            node->isLeaf = true;
        }
        return node;
    }
    return null;
}

function(TrieNode*, retrievalNodeByKey, const char* word)
{
    as(MCTrie);
    size_t len = strlen(word);
    TrieNode *node=null, *p=obj->root;
    for (size_t i=0; i<len; i++) {
        char c = *word;
        node = p->childs[c];
        if (node) {
            p = node;
            word++;
        }
    }
    return node;
}

//function(MCArray*, keysWithPrefixFromIndex, const char* prefix, TrieNode* index)
//{
//    as(MCTrie);
//    MCArray* array = new(MCArray);
//    TrieNode* node = retrievalNodeByKey(0, obj, prefix);
//    for (int i=0; i<MCTrieWidth; i++) {
//        TrieNode* child = node->childs[i];
//        if (child->isLeaf) {
//            
//        } else {
//            
//        }
//    }
//    
//    return array;
//}

method(MCTrie, void, insertValueByKey, MCGeneric newval, const char* word)
{
    TrieNode* leaf = insertWordIntoParent(0, obj, obj->root, word);
    leaf->value = newval;
}

method(MCTrie, MCGeneric, valueOfKey, const char* word)
{
    TrieNode* node = retrievalNodeByKey(0, obj, word);
    //last leaf node have value
    if (node->isLeaf) {
        return node->value;
    }
    return MCGenericVp(null);
}

method(MCTrie, MCArray*, keysWithPrefix, const char* prefix)
{
    MCArray* array = new(MCArray);
    return array;
}

method(MCTrie, MCBool, hasKey, const char* word)
{
    TrieNode* node = retrievalNodeByKey(0, obj, word);
    if (node && node->isLeaf) {
        return true;
    }
    return false;
}

method(MCTrie, void, printTree, voida)
{
    
}

onload(MCTrie)
{
    if (load(MCObject)) {
        binding(MCTrie, void, bye, voida);
        binding(MCTrie, void, insertValueByKey, MCGeneric newval, const char* word);
        binding(MCTrie, MCGeneric, valueOfKey, const char* word);
        binding(MCTrie, MCArray*, keysWithPrefix, const char* prefix);
        binding(MCTrie, MCBool, hasKey, const char* word);
        binding(MCTrie, void, printTree, voida);
        return cla;
    } else {
        return null;
    }
}

