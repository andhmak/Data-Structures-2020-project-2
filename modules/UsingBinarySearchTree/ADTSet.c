//////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Set μέσω Binary Search Tree (BST) και DoublyLinkedList
//
//////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTBList.h"
#include "ADTSet.h"


// Υλοποιούμε τον ADT Set μέσω BST και DoublyLL, οπότε το struct set είναι
// ένα Δυαδικό Δέντρο Αναζήτησης που περιέχει μια αμφίδρομη λίστα.
struct set {
	SetNode root;				// η ρίζα, NULL αν είναι κενό δέντρο
	BList blist;				// η αμφίδρομη λίστα
	int size;					// μέγεθος, ώστε η set_size να είναι Ο(1)
	CompareFunc compare;		// η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του set
};

// Ενώ το struct set_node είναι κόμβος ενός Δυαδικού Δέντρου Αναζήτησης

struct set_node {
	BListNode bnode;			// Αντίστοιχος κόμβος στην λίστα
	SetNode left, right;		// Παιδιά
	SetNode parent;				// Πατέρας
	Pointer value;
};

// Παρατηρήσεις για τις node_* συναρτήσεις
// - είναι βοηθητικές (κρυφές από το χρήστη) και υλοποιούν διάφορες λειτουργίες πάνω σε κόμβους του BST.
// - είναι αναδρομικές, η αναδρομή είναι γενικά πολύ βοηθητική στα δέντρα.
// - όσες συναρτήσεις _τροποποιούν_ το δέντρο, ουσιαστικά ενεργούν στο _υποδέντρο_ με ρίζα τον κόμβο node, και επιστρέφουν τη νέα
//   ρίζα του υποδέντρου μετά την τροποποίηση. Η νέα ρίζα χρησιμοποιείται από την προηγούμενη αναδρομική κλήση.
//
// Οι set_* συναρτήσεις (πιο μετά στο αρχείο), υλοποιούν τις συναρτήσεις του ADT Set, και είναι απλές, καλώντας τις αντίστοιχες node_*.


// Δημιουργεί και επιστρέφει έναν κόμβο με τιμή value (χωρίς παιδιά)

static SetNode node_create(Pointer value) {
	SetNode node = malloc(sizeof(*node));
	node->left = NULL;
	node->right = NULL;
	node->bnode = NULL;
	node->parent = NULL;
	node->value = value;
	return node;
}

// Επιστρέφει τον κόμβο με τιμή ίση με value στο υποδέντρο με ρίζα node, διαφορετικά NULL

static SetNode node_find_equal(SetNode node, CompareFunc compare, Pointer value) {
	// κενό υποδέντρο, δεν υπάρχει η τιμή
	if (node == NULL)
		return NULL;
	
	// Το πού βρίσκεται ο κόμβος που ψάχνουμε εξαρτάται από τη διάταξη της τιμής
	// value σε σχέση με την τιμή του τρέχοντος κόμβο (node->value)
	//
	int compare_res = compare(value, node->value);			// αποθήκευση για να μην καλέσουμε την compare 2 φορές
	if (compare_res == 0)									// value ισοδύναμη της node->value, βρήκαμε τον κόμβο
		return node;
	else if (compare_res < 0)								// value < node->value, ο κόμβος που ψάχνουμε είναι στο αριστερό υποδέντρο
		return node_find_equal(node->left, compare, value);
	else													// value > node->value, ο κόμβος που ψάχνουμε είνια στο δεξιό υποδέντρο
		return node_find_equal(node->right, compare, value);
}

// Επιστρέφει τον μικρότερο κόμβο του υποδέντρου με ρίζα node

static SetNode node_find_min(SetNode node) {
	return node != NULL && node->left != NULL
		? node_find_min(node->left)				// Υπάρχει αριστερό υποδέντρο, η μικρότερη τιμή βρίσκεται εκεί
		: node;									// Αλλιώς η μικρότερη τιμή είναι στο ίδιο το node
}

// Επιστρέφει τον μεγαλύτερο κόμβο του υποδέντρου με ρίζα node

static SetNode node_find_max(SetNode node) {
	return node != NULL && node->right != NULL
		? node_find_max(node->right)			// Υπάρχει δεξί υποδέντρο, η μεγαλύτερη τιμή βρίσκεται εκεί
		: node;									// Αλλιώς η μεγαλύτερη τιμή είναι στο ίδιο το node
}

// Αν υπάρχει κόμβος με τιμή ισοδύναμη της value, αλλάζει την τιμή του σε value, διαφορετικά προσθέτει
// νέο κόμβο με τιμή value. Επιστρέφει τη νέα ρίζα του υποδέντρου, και θέτει το *inserted σε true
// αν έγινε προσθήκη, ή false αν έγινε ενημέρωση.

static SetNode node_insert(Set set, SetNode node, CompareFunc compare, Pointer value, bool* inserted, Pointer* old_value, SetNode parent, char direction) {
	// Αν το υποδέντρο είναι κενό, δημιουργούμε νέο κόμβο ο οποίος γίνεται ρίζα του υποδέντρου
	if (node == NULL) {
		*inserted = true;			// κάναμε προσθήκη
		SetNode newnode = node_create(value);	// δημιουργούμε τον κόμβο
		// αν υπάρχει πατέρας (δεν προσθέτουμε την ρίζα)
		if (parent != NULL) {
			// αν έχουμε "κατέβει" δεξιά
			if (direction) {
				// βάζουμε τον κόμβο στην λιστα
				blist_insert(set->blist, blist_next(set->blist, parent->bnode), newnode);
				// περνάμε στον κόμβο την θέση του στην λίστα
				newnode->bnode = blist_next(set->blist, parent->bnode);
			}
			// αν έχουμε "κατέβει" αριστερά
			else {
				// βάζουμε τον κόμβο στην λιστα
				blist_insert(set->blist, parent->bnode, newnode);
				// περνάμε στον κόμβο την θέση του στην λίστα
				newnode->bnode = blist_previous(set->blist, parent->bnode);
			}
		}
		// αν προσθέτουμε την ρίζα
		else {
			// βάζουμε τον πρώυο κόμβο στην λιστα
			blist_insert(set->blist, BLIST_EOF, newnode);
			// περνάμε στον κόμβο την θέση του στην λίστα
			newnode->bnode = blist_first(set->blist);
		}
		// περνάμε τον πατέρα
		newnode->parent = parent;
		// προσθέτουμε τον κόμβο
		return newnode;
	}

	// Το που θα γίνει η προσθήκη εξαρτάται από τη διάταξη της τιμής
	// value σε σχέση με την τιμή του τρέχοντος κόμβου (node->value)
	//
	int compare_res = compare(value, node->value);
	if (compare_res == 0) {
		// βρήκαμε ισοδύναμη τιμή, κάνουμε update
		*inserted = false;
		*old_value = node->value;
		node->value = value;

	} else if (compare_res < 0) {
		// value < node->value, συνεχίζουμε αριστερά (0), περνόντας το node σαν πατέρα
		node->left = node_insert(set, node->left, compare, value, inserted, old_value, node, 0);

	} else {
		// value > node->value, συνεχίζουμε δεξιά (1), περνόντας το node σαν πατέρα
		node->right = node_insert(set, node->right, compare, value, inserted, old_value, node, 1);
	}

	return node;	// η ρίζα του υποδέντρου δεν αλλάζει
}

// Διαγράφει το κόμβο με τιμή ισοδύναμη της value, αν υπάρχει. Επιστρέφει τη νέα ρίζα του
// υποδέντρου, και θέτει το *removed σε true αν έγινε πραγματικά διαγραφή.

static SetNode node_remove(Set set, SetNode node, CompareFunc compare, Pointer value, bool* removed, Pointer* old_value) {
	if (node == NULL) {
		*removed = false;		// κενό υποδέντρο, δεν υπάρχει η τιμή
		return NULL;
	}

	int compare_res = compare(value, node->value);
	if (compare_res == 0) {
		// Βρέθηκε ισοδύναμη τιμή στον node, οπότε τον διαγράφουμε. Το πώς θα γίνει αυτό εξαρτάται από το αν έχει παιδιά.
		*removed = true;
		*old_value = node->value;

		if (node->left == NULL) {
			// Δεν υπάρχει αριστερό υποδέντρο, οπότε διαγράφεται απλά ο κόμβος και νέα ρίζα μπαίνει το δεξί παιδί
			SetNode right = node->right;	// αποθήκευση πριν το free!
			blist_remove(set->blist, node->bnode);	// αφαίρεση από την λιστα
			free(node);
			return right;

		} else if (node->right == NULL) {
			// Δεν υπάρχει δεξί υποδέντρο, οπότε διαγράφεται απλά ο κόμβος και νέα ρίζα μπαίνει το αριστερό παιδί
			SetNode left = node->left;		// αποθήκευση πριν το free!
			blist_remove(set->blist, node->bnode);	// αφαίρεση από την λιστα
			free(node);
			return left;

		} else {
			// Υπάρχουν και τα δύο παιδιά. Αντικαθιστούμε την τιμή του node με την μικρότερη του δεξιού υποδέντρου 
			// (δηλαδή την επόμενη στην λίστα), η οποία αφαιρείται.

			SetNode min_right = blist_node_value(set->blist, blist_next(set->blist, node->bnode));

			// Σύνδεση του min_right στη θέση του node και επιδιόρθωση του
			// μέρους του δέντρου από όπου αφαιρείται ο min_right
			if (min_right != node->right){
				min_right->parent->left = min_right->right;
				if (min_right->right != NULL) {
					min_right->right->parent = min_right->parent;
				}
				min_right->right = node->right;
				if (min_right->right != NULL) {
					min_right->right->parent = min_right;
				}
			}

			min_right->left = node->left;
			if (min_right->left != NULL) {
				min_right->left->parent = min_right;
			}

			blist_remove(set->blist, node->bnode);	// αφαίρεση από την λιστα

			free(node);
			return min_right;
		}
	}

	// compare_res != 0, συνεχίζουμε στο αριστερό ή δεξί υποδέντρο, η ρίζα δεν αλλάζει.
	if (compare_res < 0) {
		node->left  = node_remove(set, node->left,  compare, value, removed, old_value);
		if (node->left != NULL) {
			node->left->parent = node;
		}
	}
	else {
		node->right = node_remove(set, node->right, compare, value, removed, old_value);
		if (node->right != NULL) {
			node->right->parent = node;
		}
	}
	return node;
}

// Καταστρέφει όλο το υποδέντρο με ρίζα node

static void node_destroy(SetNode node, DestroyFunc destroy_value) {
	if (node == NULL)
		return;
	
	// πρώτα destroy τα παιδιά, μετά free το node
	node_destroy(node->left, destroy_value);
	node_destroy(node->right, destroy_value);

	if (destroy_value != NULL)
		destroy_value(node->value);

	free(node);
}


//// Συναρτήσεις του ADT Set. Γενικά πολύ απλές, αφού καλούν τις αντίστοιχες node_*

Set set_create(CompareFunc compare, DestroyFunc destroy_value) {
	assert(compare != NULL);	// LCOV_EXCL_LINE

	// δημιουργούμε το stuct
	Set set = malloc(sizeof(*set));
	set->root = NULL;			// κενό δέντρο
	set->size = 0;
	set->compare = compare;
	set->destroy_value = destroy_value;
	set->blist = blist_create(NULL);

	return set;
}

int set_size(Set set) {
	return set->size;
}

void set_insert(Set set, Pointer value) {
	bool inserted;
	Pointer old_value;
	set->root = node_insert(set, set->root, set->compare, value, &inserted, &old_value, NULL, 0);

	// Το size αλλάζει μόνο αν μπει νέος κόμβος. Στα updates κάνουμε destroy την παλιά τιμή
	if (inserted)
		set->size++;
	else if (set->destroy_value != NULL)
		set->destroy_value(old_value);
}

bool set_remove(Set set, Pointer value) {
	bool removed;
	Pointer old_value = NULL;
	set->root = node_remove(set, set->root, set->compare, value, &removed, &old_value);

	// Το size αλλάζει μόνο αν πραγματικά αφαιρεθεί ένας κόμβος
	if (removed) {
		set->size--;

		if (set->destroy_value != NULL)
			set->destroy_value(old_value);
	}

	return removed;
}

Pointer set_find(Set set, Pointer value) {
	SetNode node = node_find_equal(set->root, set->compare, value);
	return node == NULL ? NULL : node->value;
}

DestroyFunc set_set_destroy_value(Set vec, DestroyFunc destroy_value) {
	DestroyFunc old = vec->destroy_value;
	vec->destroy_value = destroy_value;
	return old;
}

void set_destroy(Set set) {
	node_destroy(set->root, set->destroy_value);
	blist_destroy(set->blist);
	free(set);
}

SetNode set_first(Set set) {
	if (blist_first(set->blist) == BLIST_BOF) {
		return SET_BOF;
	}
	return blist_node_value(set->blist, blist_first(set->blist));
}

SetNode set_last(Set set) {
	if (blist_last(set->blist) == BLIST_EOF) {
		return SET_EOF;
	}
	return blist_node_value(set->blist, blist_last(set->blist));
}

SetNode set_previous(Set set, SetNode node) {
	if (blist_previous(set->blist, node->bnode) == BLIST_BOF) {
		return SET_BOF;
	}
	return (SetNode) blist_node_value(set->blist, blist_previous(set->blist, node->bnode));
}

SetNode set_next(Set set, SetNode node) {
	if (blist_next(set->blist, node->bnode) == BLIST_EOF) {
		return SET_EOF;
	}
	return (SetNode) blist_node_value(set->blist, blist_next(set->blist, node->bnode));
}

Pointer set_node_value(Set set, SetNode node) {
	return node->value;
}

SetNode set_find_node(Set set, Pointer value) {
	return node_find_equal(set->root, set->compare, value);
}



// Συναρτήσεις που δεν υπάρχουν στο public interface αλλά χρησιμοποιούνται στα tests.
// Ελέγχουν ότι το δέντρο είναι ένα σωστό BST.

// LCOV_EXCL_START (δε μας ενδιαφέρει το coverage των test εντολών, και επιπλέον μόνο τα true branches εκτελούνται σε ένα επιτυχημένο test)

static bool node_is_bst(SetNode node, CompareFunc compare) {
	if (node == NULL)
		return true;

	// Ελέγχουμε την ιδιότητα:
	// κάθε κόμβος είναι > αριστερό παιδί, > δεξιότερο κόμβο του αριστερού υποδέντρου, < δεξί παιδί, < αριστερότερο κόμβο του δεξιού υποδέντρου.
	// Είναι ισοδύναμη με την BST ιδιότητα (κάθε κόμβος είναι > αριστερό υποδέντρο και < δεξί υποδέντρο) αλλά ευκολότερο να ελεγθεί.
	bool res = true;
	if(node->left != NULL)
		res = res && compare(node->left->value, node->value) < 0 && compare(node_find_max(node->left)->value, node->value) < 0;
	if(node->right != NULL)
		res = res && compare(node->right->value, node->value) > 0 && compare(node_find_min(node->right)->value, node->value) > 0;

	return res &&
		node_is_bst(node->left, compare) &&
		node_is_bst(node->right, compare);
}

bool set_is_proper(Set node) {
	return node_is_bst(node->root, node->compare);
}

// LCOV_EXCL_STOP

// συναρτήσεις για το bonus της άσκησης 3, με την set_remove_node να είναι και για το bonus της 5

// Αφαιρεί τον κόμβο node

void set_remove_node(Set set, SetNode node) {
	// Εύρεση θέσης του node, για να μπει εκεί ο κόμβος που θα πάρει την θέση του
	SetNode *position = &(set->root);
	if (node != set->root) {
		if (node->parent->right == node) {
			position = &(node->parent->right);
		}
		else {
			position = &(node->parent->left);
		}
	}
	// Δεν υπάρχει αριστερό υποδέντρο, οπότε διαγράφεται απλά ο κόμβος και στην θέση του node μπαίνει το δεξί παιδί
	if (node->right == NULL) {
		*position = node->left;
		if (node->left != NULL) {
			node->left->parent = node->parent;
		}
		blist_remove(set->blist, node->bnode);
		if (set->destroy_value != NULL) {
			set->destroy_value(node->value);
		}
		free(node);
	}
	// Δεν υπάρχει δεξιό υποδέντρο, οπότε διαγράφεται απλά ο κόμβος και στην θέση του node μπαίνει το αριστερό παιδί
	else if (node->left == NULL) {
		*position = node->right;
		if (node->right != NULL) {
			node->right->parent = node->parent;
		}
		blist_remove(set->blist, node->bnode);
		if (set->destroy_value != NULL) {
			set->destroy_value(node->value);
		}
		free(node);
	}
	else {
		// Υπάρχουν και τα δύο παιδιά. Αντικαθιστούμε την τιμή του node με την μικρότερη του δεξιού υποδέντρου 
		// (δηλαδή την επόμενη στην λίστα), η οποία αφαιρείται.
		SetNode min_right = blist_node_value(set->blist, blist_next(set->blist, node->bnode));

		// Σύνδεση του min_right στη θέση του node και επιδιόρθωση του
		// μέρους του δέντρου από όπου αφαιρείται ο min_right
		if (min_right != node->right) {
			min_right->parent->left = min_right->right;
			if (min_right->right != NULL) {
				min_right->right->parent = min_right->parent;
			}
			min_right->right = node->right;
			if (min_right->right != NULL) {
				min_right->right->parent = min_right;
			}
		}
		
		min_right->left = node->left;
		if (min_right->left != NULL) {
			min_right->left->parent = min_right;
		}
		*position = min_right;
		min_right->parent = node->parent;
		blist_remove(set->blist, node->bnode);
		if (set->destroy_value != NULL) {
			set->destroy_value(node->value);
		}
		free(node);
	}
	set->size--;
}

// Αφαιρεί τον την τιμή value, που βρίσκεται πιθανώς σε λάθος σημείο του δέντρου

void set_remove_wrong_value(Set set, Pointer value) {
	// Ψάχνουμε όλους τους κόμβους
	for (SetNode node = set_first(set) ; node != SET_EOF ; node  = set_next(set, node)) {
		// Όταν βρούμε τον κόμβο που ψάχνουμε τον αφαιρούμε
		if (!set->compare(set_node_value(set, node), value)) {
			set_remove_node(set, node);
			break;
		}
	}
}

// Προσθέτει τον κόμβο node (δεν χρησιμοποιείται κάπου)

void set_insert_node(Set set, SetNode node) {
	// Το πού θα γίνει η προσθήκη εξαρτάται από τη διάταξη της τιμής σύμφωνα με την compare
	SetNode iternode = set->root;
	while (1) {
		if (set->compare(node->value, iternode->value) < 0) {
			if (iternode->left != NULL) {
				iternode = iternode->left;
			}
			else {
				iternode->left = node;
				node->parent = iternode;
				blist_insert(set->blist, iternode->bnode, node);
				node->bnode = blist_previous(set->blist, iternode->bnode);
				return;
			}
		}
		else {
			if (iternode->right != NULL) {
				iternode = iternode->right;
			}
			else {
				iternode->right = node;
				node->parent = iternode;
				blist_insert(set->blist, blist_next(set->blist, iternode->bnode), node);
				node->bnode = blist_next(set->blist, iternode->bnode);
				return;
			}
		}
	}
}