#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *tmp;
    struct list_head *cur;
    list_for_each_safe(cur, tmp, head) {
        q_release_element(list_entry(cur, element_t, list));
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *newNode = malloc(sizeof(element_t));
    if (!newNode)
        return false;
    newNode->value = strdup(s);
    if (!newNode->value) {
        q_release_element(newNode);
        return false;
    }
    list_add(&newNode->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *newNode = malloc(sizeof(element_t));
    if (!newNode)
        return false;
    newNode->value = strdup(s);
    if (!newNode->value) {
        q_release_element(newNode);
        return false;
    }
    list_add_tail(&newNode->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->next, element_t, list);
    if (sp) {
        // Save one byte for null character
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->next);
    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->prev, element_t, list);
    if (sp) {
        // Save one byte for null character
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(head->prev);
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    int length = 0;
    struct list_head *cur;
    list_for_each(cur, head) {
        length++;
    }
    return length;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-theㄈ-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *fast = head->next;
    struct list_head *slow = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    struct list_head **indirect = &head->next;
    while (*indirect != head) {
        if ((*indirect)->next != head &&
            !strcmp(list_entry(*indirect, element_t, list)->value,
                    list_entry((*indirect)->next, element_t, list)->value)) {
            struct list_head *tmp = *indirect;
            char *sp = strdup(list_entry(*indirect, element_t, list)->value);
            while (tmp != head &&
                   !strcmp(sp, list_entry(tmp, element_t, list)->value)) {
                struct list_head *toDel = tmp;
                tmp = tmp->next;
                list_del(toDel);
                q_release_element(list_entry(toDel, element_t, list));
            }
            free(sp);
        } else {
            indirect = &(*indirect)->next;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *cur;
    list_for_each(cur, head) {
        if (cur->next == head)
            break;
        list_move(cur, cur->next);
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *cur, *next;
    list_for_each_safe(cur, next, head) {
        list_move(cur, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k == 1)
        return;
    int count = q_size(head);
    struct list_head *cur = head->next;
    while (count >= k) {
        struct list_head *prev, *next;
        prev = cur->prev;
        next = cur->next;
        for (int i = 0; i < k; i++) {
            list_move(cur, prev);
            cur = next;
            next = next->next;
        }
        count -= k;
    }
}

/* Merge @right list to @left list in ascending/descending order */
void merge_two_lists(struct list_head *left,
                     struct list_head *right,
                     bool descend)
{
    if (!left || !right)
        return;
    if (list_empty(right))
        return;
    if (list_empty(left)) {
        list_splice_init(right, left);
        return;
    }
    LIST_HEAD(merged);
    while (!list_empty(left) && !list_empty(right)) {
        element_t *curLeft = list_entry(left->next, element_t, list);
        element_t *curRight = list_entry(right->next, element_t, list);
        int cmp = strcmp(curLeft->value, curRight->value);
        if (descend ? cmp > 0 : cmp <= 0) {
            list_move_tail(&curLeft->list, &merged);
        } else {
            list_move_tail(&curRight->list, &merged);
        }
    }

    if (!list_empty(left)) {
        list_splice_tail_init(left, &merged);
    }
    if (!list_empty(right)) {
        list_splice_tail_init(right, &merged);
    }
    list_splice_init(&merged, left);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head *fast = head->next;
    struct list_head *slow = head->next;

    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    LIST_HEAD(left);
    list_cut_position(&left, head, slow->prev);

    q_sort(head, descend);
    q_sort(&left, descend);

    merge_two_lists(&left, head, descend);
    INIT_LIST_HEAD(head);
    list_splice(&left, head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    struct list_head *min = head->prev;
    struct list_head *cur = min->prev;
    while (cur != head) {
        struct list_head *prev = cur->prev;
        if (strcmp(list_entry(cur, element_t, list)->value,
                   list_entry(min, element_t, list)->value) >= 0) {
            list_del(cur);
            q_release_element(list_entry(cur, element_t, list));
        } else {
            min = min->prev;
        }
        cur = prev;
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    struct list_head *max = head->prev;
    struct list_head *cur = max->prev;
    while (cur != head) {
        struct list_head *prev = cur->prev;
        if (strcmp(list_entry(cur, element_t, list)->value,
                   list_entry(max, element_t, list)->value) <= 0) {
            list_del(cur);
            q_release_element(list_entry(cur, element_t, list));
        } else {
            max = max->prev;
        }
        cur = prev;
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return list_first_entry(head, queue_contex_t, chain)->size;

    queue_contex_t *cur, *next;
    queue_contex_t *merged = list_first_entry(head, queue_contex_t, chain);
    list_for_each_entry_safe(cur, next, head, chain) {
        if (&cur->chain == head->next)
            continue;
        merged->size += cur->size;
        merge_two_lists(merged->q, cur->q, descend);
    }
    return merged->size;
}

/**
 * q_shuffle() - Randomly shuffle the elements in the queue
 * @head: header of queue
 *
 * This function randomly rearranges the elements in the queue using
 * the Fisher-Yates shuffle algorithm, ensuring each permutation has
 * an equal probability of occurring.
 *
 * No effect if queue is NULL or empty. If there has only one element, do
 * nothing.
 */
void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    int len = q_size(head);
    srand(time(NULL));
    LIST_HEAD(new);

    while (len) {
        int random = rand() % len;
        struct list_head *cur = head->next;
        while (random--) {
            cur = cur->next;
        }
        struct list_head *tmp = cur;
        list_move_tail(tmp, &new);
        len--;
    }
    list_splice_tail(&new, head);
}