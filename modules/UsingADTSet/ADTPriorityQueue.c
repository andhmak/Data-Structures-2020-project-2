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
};

struct priority_queue_node {
    SetNode setnode;
};

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
    pqueue->set = set_create(compare, destroy_value);
    if (values != NULL) {
        for (uint i = 0 ; i < vector_size(values) ; i++) {
            set_insert(pqueue->set, vector_get_at(values, i));
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
    return set_node_value(pqueue->set, set_last(pqueue->set));
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
    set_insert(pqueue->set, value);
    PriorityQueueNode pqnode = (PriorityQueueNode) set_find_node(pqueue->set, value);
    return pqnode;
}

// Αφαιρεί την μεγαλύτερη τιμή της ουράς (μη ορισμένο αποτέλεσμα αν η ουρά είναι κενή)

void pqueue_remove_max(PriorityQueue pqueue) {
    set_remove(pqueue->set, set_node_value(pqueue->set, set_last(pqueue->set)));
}

// Αλλάζει τη συνάρτηση που καλείται σε κάθε αφαίρεση/αντικατάσταση στοιχείου σε
// destroy_value. Επιστρέφει την προηγούμενη τιμή της συνάρτησης.

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value) {
    return set_set_destroy_value(pqueue->set, destroy_value);
}

// Ελευθερώνει όλη τη μνήμη που δεσμεύει η ουρά pqueue.
// Οποιαδήποτε λειτουργία πάνω στη ουρά μετά το destroy είναι μη ορισμένη.

void pqueue_destroy(PriorityQueue pqueue) {
    set_destroy(pqueue->set);
    free(pqueue);
}



//// Νέες συναρτήσεις για την εργασία 2 //////////////////////////////////////////

// Επιστρέφει το περιεχόμενο του κόμβου node

Pointer pqueue_node_value(PriorityQueue pqueue, PriorityQueueNode node) {
    return set_node_value(pqueue->set, (SetNode) node);
}

// Αφαιρεί τον κόμβο node ο οποίος μπορεί να βρίσκεται σε οποιαδήποτε θέση της ουράς
// (μη ορισμένη συμπεριφορά αν δεν ανήκει στην ουρά).

void pqueue_remove_node(PriorityQueue pqueue, PriorityQueueNode node) {
    set_remove(pqueue->set, set_node_value(pqueue->set, (SetNode) node));
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
    set_remove_node(pqueue->set, (SetNode) node);
    set_insert_node(pqueue->set, (SetNode) node);
}