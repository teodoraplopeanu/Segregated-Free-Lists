**Nume: Teodora-Anca Plopeanu**
**Grupă: 312CA**

## Tema 1 - Segregated Free Lists

### Descriere:

* Programul implementat reprezinta un alocator de memorie.
* Operatiile realizate sunt: **INIT_HEAP, MALLOC, FREE, READ, WRITE, DUMP_MEMORY, DESTROY_HEAP**.

### INIT_HEAP
* Se construieste structura **sfl**, ce reprezinta un heap ce are in coomponenta sa *num_lists* liste, fiecare avand *data_size* o putere a lui 2, din intervalul [2^3,  2^(num_lists + 3)].
* Pentru fiecare nod se retine in *void *data*, ce are cast la structura *sfl_info_t*, adresa de inceput a respectivului nod, relativ la *start_address* a heap-ului.

### MALLOC
* Aloca memorie pentru un bloc de *data_size* bytes (il muta intr-o alta structura de date, numita **allocated_blocks**, ce stocheaza nodurile pentru care a fost alocata memorie, in ordinea crescatoare a adreselor).
* Caut indexul listei din *sfl*, ce are drept *data_size* cea mai mica valoare <= *bytes*.

* Daca nu gasesc niciuna, inseamna ca avem cazul de eroare *Out of memeory*.

	* Daca *data_size* gasit este egal cu *bytes*, elimin nodul din lista corespunzatoare din *sfl* si il adaug in *allocated_blocks*, la pozitia corespunzatoare adresei sale.
	
	* Daca, in schimb, *data_size* > *bytes*, fragmentez nodul:
	
		* Primii *bytes* bytes din nodul gasit sunt transformati intr-un nod de sine statator, ce va fi adaugat in *allocated_blocks*.

		* Restul devin, la randul lor, un nou nod, ce va fi restocat in *sfl*, in lista corespunzatoare noului sau *data_size*.

### FREE
* Daca adresa careia dorim sa ii dam free este 0x0, nu se intampla nimic (echivalentul *free(NULL)* din C).
* Caut in *allocated_blocks* nodul cu adresa respectiva. Daca nu il gasesc, inseamna ca am de a face cu un *Invalid free*.
* Daca il gasesc, elimin nodul din lista *allocated_blocks* si, conform tipului de reconstituire a *sfl*-ului (0 sau 1), il readaug in *sfl*.

### READ
* Daca, incepand de la *address*, nu am *bytes* octeti consecutivi alocati => *Segmentation fault.*
* Altfel, afisez un  anumit numar de octeti din *data->data* al fiecarui nod, pana cand *bytes* devine 0;

### WRITE
* Daca, incepand de la *address*, nu am *bytes* octeti consecutivi alocati => *Segmentation fault.*
* Altfel, aloc memorie si adaug *string*-ul in noduri, fragmentat, in functie de *data_size* al fiecarui nod si lungimea *string*-ului.

### DUMP_MEMORY
* Realizeaza un look-up asupra memoriei curente din program (afiseaza blocurile libere, alocate, adresele lor, numarul de malloc_calls, free_calls, fragmentations, total memory, total free memory, total allocated memory), dupa formule sau functii specifice implementate in fisierul *about_lls.c*.

### DESTROY_HEAP
* Elibereaza toata memoria alocata in program, atat in *sfl*, cat si in *allocated_blocks*.


### Comentarii asupra temei:

* Crezi că ai fi putut realiza o implementare mai bună?
-> Probabil, prin reorganizarea structurilor create.

* Ce ai invățat din realizarea acestei teme?
-> M-am familiarizat cu structuri de date abstracte, mi-am imbunatatit lucrul cu liste, atat simplu, cat si dublu inlantuite. Am inteles magia din spatele functiilor malloc, free etc. de lucru cu memoria, atat de des utilizate in C.

* Alte comentarii
-> O tema foarte interesanta si creativa. Implementarea ei se poate spune ca a fost o provocare, insa m-am distrat facand-o.

### Resurse / Bibliografie:

1. [# Power-of-Two Free Lists Allocators](https://www.geeksforgeeks.org/power-of-two-free-lists-allocators-kernal-memory-allocators/)
