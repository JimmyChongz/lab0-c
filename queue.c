#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

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
    list_for_each_safe (cur, tmp, head) {
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
    list_for_each (cur, head) {
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
    list_for_each (cur, head) {
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
    list_for_each_safe (cur, next, head) {
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

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

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
    return 0;
}
