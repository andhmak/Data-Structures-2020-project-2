///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT BList μέσω διπλά συνδεδεμένης λίστας.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTBList.h"


// Ενα BList είναι pointer σε αυτό το struct
struct blist {
	BListNode dummy_first;		// dummy κόμβος πριν τον πρώτο
	BListNode dummy_last;		// dummy κόμβος μετά τον τελευταίο (η κενή λίστα έχει 2 κόμβους)
	int size;					// μέγεθος, ώστε η list_size να είναι Ο(1)
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο της λίστας.
};

struct blist_node {
	BListNode prev;		// Δείκτης στον προηγούμενο
	BListNode next;		// Δείκτης στον επόμενο
	Pointer value;		// Η τιμή που αποθηκεύουμε στον κόμβο
};


BList blist_create(DestroyFunc destroy_value) {
	// Πρώτα δημιουργούμε το stuct
	BList blist = malloc(sizeof(*blist));
	// αρχικοποιούμε το μέγεθος
	blist->size = 0;
	// θέτουμε την συνάρτηση απελευθέρωσης μνήμης στοιχείου
	blist->destroy_value = destroy_value;

	// ΔΗμιουργούμε τους dummy και θέτουμε τις σχέσεις τους
	blist->dummy_first = malloc(sizeof(*blist->dummy_first));
	blist->dummy_last = malloc(sizeof(*blist->dummy_last));
	blist->dummy_first->next = blist->dummy_last;		// ο dummy_first έχει επόμενο τον dummy_last
	blist->dummy_last->prev = blist->dummy_first;		// ο dummy_last έχει προηγούμενο τον dummy_first
	blist->dummy_first->prev = NULL;			// ο dummy_first δεν έχει προηγούμενο
	blist->dummy_last->next = NULL;				// ο dummy_last δεν έχει επόμενο

	return blist;
}

int blist_size(BList blist) {
	return blist->size;
}

void blist_insert(BList blist, BListNode node, Pointer value) {
	// Αν το node είναι NULL απλά εισάγουμε πριν τον dummy_last
	if (node == NULL)
		node = blist->dummy_last;

	// Δημιουργία του νέου κόμβου
	BListNode new = malloc(sizeof(*new));
	new->value = value;

	// Σύνδεση του new με τους άλλους κόμβους
	new->next = node;
	new->prev = node->prev;
	node->prev->next = new;
	node->prev = new;

	// Ενημέρωση του size
	blist->size++;
}

void blist_remove(BList blist, BListNode node) {
	// Ο κόμβος προς διαγραφή είναι ο node, ο οποίος πρέπει να υπάρχει
	assert(node != NULL);		// LCOV_EXCL_LINE

	if (blist->destroy_value != NULL)
		blist->destroy_value(node->value);

	// Σύνδεση των γειτονικών nodes
	node->prev->next = node->next;		// πριν το free!
	node->next->prev = node->prev;

	free(node);

	// Ενημέρωση του last
	blist->size--;
}

Pointer blist_find(BList blist, Pointer value, CompareFunc compare) {
	BListNode node = blist_find_node(blist, value, compare);
	return node == NULL ? NULL : node->value;
}

DestroyFunc blist_set_destroy_value(BList blist, DestroyFunc destroy_value) {
	DestroyFunc old = blist->destroy_value;
	blist->destroy_value = destroy_value;
	return old;
}

void blist_destroy(BList blist) {
	// Διασχίζουμε όλη τη λίστα και κάνουμε free όλους τους κόμβους,
	// συμπεριλαμβανομένων και των dummy!
	//
	BListNode node = blist->dummy_first;
	while (node != NULL) {				// while αντί για for, γιατί θέλουμε να διαβάσουμε
		BListNode next = node->next;		// το node->next _πριν_ κάνουμε free!

		// Καλούμε τη destroy_value, αν υπάρχει (προσοχή, όχι στους dummy!)
		if (blist->destroy_value != NULL && node != blist->dummy_first && node != blist->dummy_last)
			blist->destroy_value(node->value);

		free(node);
		node = next;
	}

	// Τέλος free το ίδιο το struct
	free(blist);
}


// Διάσχιση της λίστας /////////////////////////////////////////////

BListNode blist_first(BList blist) {
	// Προσοχή, αν η λίστα είναι κενή το dummy_first δείχνει στον dummy_last, εμείς όμως θέλουμε να επιστρέψουμε NULL, όχι τον dummy_last!
	//
	if (blist->dummy_first->next == blist->dummy_last)
		return BLIST_EOF;		// κενή λίστα
	else
		return blist->dummy_first->next;
}

BListNode blist_last(BList blist) {
	// Προσοχή, αν η λίστα είναι κενή το dummy_last δείχνει στον dummy_first, εμείς όμως θέλουμε να επιστρέψουμε NULL, όχι τον dummy_first!
	//
	if (blist->dummy_last->prev == blist->dummy_first)
		return BLIST_BOF;		// κενή λίστα
	else
		return blist->dummy_last->prev;
}

BListNode blist_next(BList blist, BListNode node) {
	assert(node != NULL);	// LCOV_EXCL_LINE
	// Προσοχή, αν node είναι το last, δείχνει στον dummy_last, εμείς όμως θέλουμε να επιστρέψουμε BLIST_EOF, όχι τον dummy_last!
	//
	if (node->next == blist->dummy_last) {
		return BLIST_EOF;
	}
	return node->next;
}

BListNode blist_previous(BList blist, BListNode node) {
	assert(node != NULL);	// LCOV_EXCL_LINE
	// Προσοχή, αν node είναι το first, δείχνει στον dummy_first, εμείς όμως θέλουμε να επιστρέψουμε BLIST_BOF, όχι τον dummy_last!
	//
	if (node->prev == blist->dummy_first) {
		return BLIST_BOF;
	}
	return node->prev;
}

Pointer blist_node_value(BList blist, BListNode node) {
	assert(node != NULL);	// LCOV_EXCL_LINE
	return node->value;
}

BListNode blist_find_node(BList blist, Pointer value, CompareFunc compare) {
	// διασχίζουμε όλην τη λίστα, καλούμε την compare μέχρι να επιστρέψει 0
	//
	for (BListNode node = blist->dummy_first->next ; node != blist->dummy_last ; node = node->next)
		if (compare(value, node->value) == 0)
			return node;		// βρέθηκε

	return NULL;	// δεν υπάρχει
}