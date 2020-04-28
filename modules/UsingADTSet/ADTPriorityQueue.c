///////////////////////////////////////////////////////////////////
//
// ADT Priority Queue
//
// Abstract ουρά προτεραιότητας. Σε κάθε remove αφαιρείται το
// μεγαλύτερο στοιχείο (με βάση τη συνάρτηση compare).
//
///////////////////////////////////////////////////////////////////

//#pragma once // #include το πολύ μία φορά

#include <stdlib.h>
#include "common_types.h"
#include "ADTVector.h"
#include "ADTSet.h"

// Μία ουρά προτεραιότητας αναπαριστάται από τον τύπο PriorityQueue
// Απροσδιόριστη συμπεριφορά αν προστεθεί στοιχείο ισοδύναμο (σύμφωνα με την compare) με ήδη υπάρχον

typedef struct priority_queue* PriorityQueue;
typedef struct priority_queue_node* PriorityQueueNode;

struct priority_queue {
    Set set;
    CompareFunc compare;
    DestroyFunc destroy;
};

struct priority_queue_node {
    Pointer value;
};

static CompareFunc temp_compare;
static DestroyFunc temp_destroy;

int compare_nodes(Pointer a, Pointer b) {
    PriorityQueueNode nodea = a, nodeb = b;
    int result = temp_compare(nodea->value, nodeb->value);
    if (result != 0) {
        return result;
    }
    else {
        return nodea != nodeb;
    }
}

void destroy_node(Pointer node) {
    PriorityQueueNode pnode = (PriorityQueueNode) node;
    if (temp_destroy != NULL) {
        temp_destroy(pnode->value);
    }
    free(pnode);
}

//CompareFunc compare_nodes(CompareFunc compare) {
//    int newcompare(Pointer a, Pointer b) {
//        int result = compare((PriorityQueueNode) a)->value, (PriorityQueueNode) b)->value);
//        if (result) {
//            return result;
//        }
//        else {
//            a - b;
//        }
//    }
//    return newcompare;
//}

// Δημιουργεί και επιστρέφει μια νέα ουρά προτεραιότητας, της οποίας τα στοιχεία συγκρίνονται με βάση τη συνάρτηση compare.
// Αν destroy_value != NULL, τότε καλείται destroy_value(value) κάθε φορά που αφαιρείται ένα στοιχείο.
// Αν values != NULL, τότε η ουρά αρχικοποιείται με τα στοιχεία του Vector values.

PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
    PriorityQueue pqueue = malloc(sizeof(*pqueue));
    pqueue->compare = compare;
    pqueue->destroy = destroy_value;
    pqueue->set = set_create(compare_nodes, destroy_node);
    PriorityQueueNode pnode;
    temp_compare = compare;
    if (values != NULL) {
        for (uint i = 0 ; i < vector_size(values) ; i++) {
            pnode = malloc(sizeof(*pnode));
            pnode->value = vector_get_at(values, i);
            set_insert(pqueue->set, pnode);
        }
    }
    return pqueue;
}

// Επιστρέφει τον αριθμό στοιχείων που περιέχει η ουρά pqueue

int pqueue_size(PriorityQueue pqueue) {
    return set_size(pqueue->set);
}

// Επιστρέφει το μεγαλύτερο στοιχείο της ουράς (μη ορισμένο αποτέλεσμα αν η ουρά είναι κενή)

Pointer pqueue_max(PriorityQueue pqueue) {
    PriorityQueueNode pnodemax = set_node_value(pqueue->set, set_last(pqueue->set));
    return pnodemax->value;
}

// Προσθέτει την τιμή value στην ουρά pqueue. Επιστρέφει τον κόμβο που προστέθηκε.
//
// ΠΡΟΣΟΧΗ:
// Όσο το value είναι μέλος της ουράς, οποιαδήποτε μεταβολή στο περιεχόμενό του (στη μνήμη που δείχνει) δεν πρέπει
// να αλλάζει τη σχέση διάταξης (compare) με οποιοδήποτε άλλο στοιχείο, διαφορετικά έχει μη ορισμένη συμπεριφορά.
//
// Για μεταβολές στη διάταξη υπάρχουν 2 επιλογές:
// - αφαίρεση του στοιχείου, μεταβολή και ξανά προσθήκη
// - χρήση της συνάρτησης pqueue_update_order

PriorityQueueNode pqueue_insert(PriorityQueue pqueue, Pointer value) {
    temp_compare = pqueue->compare;
    PriorityQueueNode pnode = malloc(sizeof(*pnode));
    pnode->value = value;
    set_insert(pqueue->set, pnode);
    return pnode;
}

// Αφαιρεί την μεγαλύτερη τιμή της ουράς (μη ορισμένο αποτέλεσμα αν η ουρά είναι κενή)

void pqueue_remove_max(PriorityQueue pqueue) {
    temp_destroy = pqueue->destroy;
    temp_compare = pqueue->compare;
    set_remove(pqueue->set, set_node_value(pqueue->set, set_last(pqueue->set)));
}

// Αλλάζει τη συνάρτηση που καλείται σε κάθε αφαίρεση/αντικατάσταση στοιχείου σε
// destroy_value. Επιστρέφει την προηγούμενη τιμή της συνάρτησης.

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value) {
    DestroyFunc old_destroy = pqueue->destroy;
    pqueue->destroy = destroy_value;
    return old_destroy;
}

// Ελευθερώνει όλη τη μνήμη που δεσμεύει η ουρά pqueue.
// Οποιαδήποτε λειτουργία πάνω στη ουρά μετά το destroy είναι μη ορισμένη.

void pqueue_destroy(PriorityQueue pqueue) {
    temp_destroy = pqueue->destroy;
    set_destroy(pqueue->set);
    free(pqueue);
}



//// Νέες συναρτήσεις για την εργασία 2 //////////////////////////////////////////

// Επιστρέφει το περιεχόμενο του κόμβου node

Pointer pqueue_node_value(PriorityQueue pqueue, PriorityQueueNode node) {
    return node->value;
}

// Αφαιρεί τον κόμβο node ο οποίος μπορεί να βρίσκεται σε οποιαδήποτε θέση της ουράς
// (μη ορισμένη συμπεριφορά αν δεν ανήκει στην ουρά).

void pqueue_remove_node(PriorityQueue pqueue, PriorityQueueNode node) {
    temp_compare = pqueue->compare;
    temp_destroy = pqueue->destroy;
    set_remove(pqueue->set, node);
}

// Ενημερώνει την ουρά μετά από αλλαγή της διάταξης της τιμής του κόμβου node.
// Η μόνη επιτρεπτή χρήση είναι η εξής:
// 1. Αλλαγή της διάταξης του κόμβου node
//    (αλλάζοντας τα περιεχόμενα του αντίστοιχου value)
// 2. Κλήση της συνάρτησης pqueue_update_order για αποκατάσταση της σειράς.
//
// Ανάμεσα στα βήματα 1 και 2 ΔΕΝ επιτρέπεται:
// - η αλλαγή οποιουδήποτε άλλου κόμβου
// - η κλήση οποιασδήποτε άλλης συνάρτησης pqueue_*

void pqueue_update_order(PriorityQueue pqueue, PriorityQueueNode node) {
    temp_compare = pqueue->compare;
    temp_destroy = pqueue->destroy;
    DestroyFunc old_destroy = set_set_destroy_value(pqueue->set, NULL);
    set_remove_wrongvalue(pqueue->set, node);
    set_insert(pqueue->set, node);
    set_set_destroy_value(pqueue->set, old_destroy);
}