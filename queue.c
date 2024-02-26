#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
        return head;
    }
    return NULL;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    // Release all elements first, and then release the head.
    if (!l)
        return;
    element_t *n;
    element_t *node = list_first_entry(l, element_t, list);
    list_for_each_entry_safe (node, n, l, list) {
        // q_release_element(container_of(&node->list, element_t, list));
        q_release_element(node);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)  //***************考慮配置失敗
        return false;
    INIT_LIST_HEAD(&new->list);
    char *value = malloc(sizeof(char) * (strlen(s) + 1));
    if (!value) {  //***************
        free(new);
        return false;
    }
    strncpy(value, s, strlen(s));
    value[strlen(s)] = 0;
    new->value = value;
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)  //***************考慮配置失敗
        return false;
    INIT_LIST_HEAD(&new->list);
    char *value = malloc(sizeof(char) * (strlen(s) + 1));
    if (!value) {
        free(new);
        return false;
    }
    // memcpy(value, s, strlen(s) + 1);
    strncpy(value, s, strlen(s));
    value[strlen(s)] = 0;
    new->value = value;
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp = list_first_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = 0;
    }
    list_del_init(&tmp->list);
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp = list_last_entry(head, element_t, list);
    if (sp) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = 0;
    }
    list_del_init(&tmp->list);
    return tmp;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    int len = 0;
    struct list_head *node;
    list_for_each (node, head)
        len += 1;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *left = head->next;
    struct list_head *right = head->prev;
    while ((left != right) && (left->next != right)) {
        left = left->next;
        right = right->prev;
    }
    list_del(right);
    element_t *del = list_entry(right, element_t, list);
    q_release_element(del);
    return true;
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return false;
    bool flag = false;
    element_t *curr, *nex;
    list_for_each_entry_safe (curr, nex, head, list) {
        if (&nex->list != head && strcmp(curr->value, nex->value) == 0) {
            flag = true;
            list_del(&curr->list);
            q_release_element(curr);
        } else if (flag) {
            flag = false;
            list_del(&curr->list);
            q_release_element(curr);
        }
    }
    return true;
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;
    struct list_head *curr, *n;
    struct list_head *tmp = head;
    int count = 0;
    list_for_each_safe (curr, n, head) {
        if (count % 2 == 1) {
            list_move(curr, tmp);
            tmp = curr->next;
        }
        count++;
    }
}
// https://leetcode.com/problems/swap-nodes-in-pairs/


/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;
    struct list_head *curr, *n;
    list_for_each_safe (curr, n, head)
        list_move(curr, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_is_singular(head) || k == 1)
        return;
    int countK = 0, n = q_size(head) / k;
    struct list_head *curr, *safe, *tmp = head;
    list_for_each_safe (curr, safe, head) {
        if (countK % k == 0) {
            tmp = curr->prev;
        }
        if ((countK / k) < n)
            list_move(curr, tmp);
        countK++;
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void mergeTwoLists(struct list_head *L1, struct list_head *L2)
{
    if (!L1 || !L2)
        return;
    struct list_head *Lnode = L1->next, *Rnode = L2->next;
    while (Lnode != L1 && Rnode != L2) {
        if (strcmp(list_entry(Lnode, element_t, list)->value,
                   list_entry(Rnode, element_t, list)->value) < 0) {
            Lnode = Lnode->next;
        } else {
            Rnode = Rnode->next;
            list_move(Rnode->prev, Lnode->prev);
        }
    }
    if (q_size(L2) != 0) {
        list_splice_tail(L2, L1);
    }
}
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head->next;
    for (struct list_head *fast = head->next;
         fast != head && (fast->next) != head; fast = fast->next->next)
        slow = slow->next;
    struct list_head *mid = slow;
    LIST_HEAD(right);
    list_cut_position(&right, head, mid->prev);

    q_sort(head, 1);
    q_sort(&right, 1);
    mergeTwoLists(head, &right);
}
/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    struct list_head *curr = head->prev->prev;
    while (curr != head) {
        struct list_head *nex = curr->next;
        element_t *f = list_entry(curr, element_t, list);
        element_t *s = list_entry(nex, element_t, list);
        if (strcmp(f->value, s->value) > 0) {
            struct list_head *del = curr;
            list_del(del);
            curr = curr->prev;
            q_release_element(list_entry(del, element_t, list));
        } else
            curr = curr->prev;
    }
    return q_size(head);
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return 1;
    struct list_head *curr = head->prev->prev;
    while (curr != head) {
        struct list_head *nex = curr->next;
        element_t *f = list_entry(curr, element_t, list);
        element_t *s = list_entry(nex, element_t, list);
        if (strcmp(f->value, s->value) < 0) {
            struct list_head *del = curr;
            list_del(del);
            curr = curr->prev;
            q_release_element(list_entry(del, element_t, list));
        } else
            curr = curr->prev;
    }
    return q_size(head);
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head)
        return 0;
    queue_contex_t *queue_head = list_entry(head->next, queue_contex_t, chain);
    for (struct list_head *curr = head->next->next; curr != head;
         curr = curr->next) {
        queue_contex_t *queue = list_entry(curr, queue_contex_t, chain);
        mergeTwoLists(queue_head->q, queue->q);
        INIT_LIST_HEAD(queue->q);
        queue->size = 0;
    }
    return queue_head->size;
    // https://leetcode.com/problems/merge-k-sorted-lists/
}
