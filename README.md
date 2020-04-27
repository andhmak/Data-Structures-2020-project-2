![run-tests](../../workflows/run-tests/badge.svg)

## 2020 Project 2

Εκφώνηση: https://k08.chatzi.org/projects/project2/


### Προσωπικά στοιχεία

__Όνομα__: Δημάκης Αντώνιος

__Α.Μ.__: sdi1900047


### Documentation

Συμπληρώστε εδώ __όσο documentation χρειάζεται__ ώστε οι βαθμολογητές να
κατανοήσουν πλήρως τις λύσεις σας και να τις βαθμολογήσουν ανάλογα. Αυτό θα
πρέπει να γίνει ανεξάρτητα με το αν ο κώδικάς σας είναι καλά σχολιασμένος,
πράγμα που συνιστάται.<br>
Στα βήματα μπορεί να υπαρχουν μικρές διαφορές γιατί ο ορισμός δεν είναι απόλυτα σαφής (η τελευταία επανάληψη μιας επανάληψης μπορεί να θεωρηθεί ως ένα βήμα μαζί με τις εντολές έξω από την επανάληψη).<br>
vector_insert_last:
Η πραγματική πολυπλοκότητα είναι Θ(1) όταν υπάρχει κενός χώρος στο vector, και Θ(n) αλλιώς. Για αυτόν τον λόγο εμφανίζεται μια οριζόντια ευθεία και ασυνέχειες που αποτελούν σημεία της παράστασης της συνάρτησης f(n) = n + 1, που είναι Θ(n).<br>
![Real steps](https://github.com/chatziko-k08/2020-project-2-andhmak/tree/master/images/real.png)<br>
Το ότι μόνο σε πολύ λίγες περιπτώσεις η πολυπλοκότητα είναι Ο(n) (επειδή το πλήθος του κενού χώρου που δεσμεύεται είναι πολλαπλάσιος του μεγέθους) φαίνεται μέσω της amortized πολυπλοκότητας (μέσου όρου), που είναι Ο(1) (τα βήματα είναι σταθερά 1 ή 2, και δεν αυξάνονται συναρτήσει του μεγέθους).<br>
![Amortized steps](https://github.com/chatziko-k08/2020-project-2-andhmak/tree/master/images/amort.png)<br>
list_insert_next:<br>
Η συνάρτηση είναι πάντα Θ(1), άρα τα βήματα είναι σταθερά ένα (και real και amortized).<br>
![Real steps](https://github.com/chatziko-k08/2020-project-2-andhmak/tree/master/images/real_list.png)<br>
![Amortized steps](https://github.com/chatziko-k08/2020-project-2-andhmak/tree/master/images/amortized_list.png)<br>