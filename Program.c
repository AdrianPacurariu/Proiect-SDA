#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define fisierRetete "retete.txt"
#define fisierStoc "stoc.txt"

#define errMalloc { printf("\nEroare: nu se poate aloca memorie.\n"); exit(1); }
#define errFile { printf("\nEroare: fisierul nu exista sau nu poate fi creat.\n"); exit(1); }

typedef struct listaMedicamente
{
	char* nume;
	int cantitate;
	float pret;
	struct listaMedicamente* urm;
}listaMedicamente;

typedef struct sublistaRetete
{
	char* medicament;
	int cantitate;
	struct sublistaRetete* urm;
}sublistaRetete;

typedef struct listaRetete
{
	char* reteta;
	struct listaRetete* urm;
	struct sublistaRetete* prim;
}listaRetete;

listaMedicamente* AdaugareMedicamente(listaMedicamente* lista, listaMedicamente* deAdaugat); /* Functia de adaugare in lista de medicamente. */
listaMedicamente* AdaugMedicament(listaMedicamente* lista, char* s); /* Functia de adaugare a medicament citit de la tastatura in lista simplu inlantuita. Inserarea se face alfabetic. */
listaMedicamente* StergMedicament(listaMedicamente* lista, char* s); /* Functia de stergere a unui medicament */
listaMedicamente* ModificareMedicament(listaMedicamente* lista, char* s);  /* Functia de modificare a datelor unui medicament */

listaRetete* AdaugareRetete(listaRetete* lista, listaRetete* deAdaugat, sublistaRetete* sublista); /* Functia de adaugare in lista retetelor respectiv sublista corespunzatoare. */
listaRetete* AdaugReteta(listaRetete* lista, char* reteta, char* medicament, int cantitate); /* Functia de adaugare a unei retete citite de la tastatura. */
listaRetete* StergereReteta(listaRetete* lista, char* s); /* Functia de stergere a unei retete citite de la tastatura. */

sublistaRetete* AdaugareLaReteta(sublistaRetete* lista, sublistaRetete* deAdaugat); /* Functia de adaugare in sublista aferenta fiecarei retete, medicamentul + cantitatea citite din fisier. */
sublistaRetete* AdaugareDate(sublistaRetete* lista, char* medicament, int cantitate); /* Functia de adaugare a unei noi subliste (care contine un nume de medicament + cantitate) pentru o reteta citita de la tastatura. */

void StergereParametriReteta(sublistaRetete* lista); /* Functia de stergere a tuturor sublistelor unei retete precizate la functia StergereReteta. */
void IncarcareRetete(); /* Functia de incarcare din fisier a retetelor intr-o lista cu subliste. */
void IncarcareMedicamente(); /* Functie de incarcare din fisier a medicamentelor intr-o lista simplu inlantuita.*/
void AfisareMedicamente(listaMedicamente* lista); /* Functie de afisare a medicamentelor. */
void AfisareRetete(listaRetete* lista); /* Functie de afisare a retetelor. */
void CautareReteta(listaRetete* lista, listaMedicamente* lista2, char* s); /* Functie de cautare a unei retete in lista cu subliste si afisare a pretului retetei in cazul in care, pentru acea reteta, medicamentele se afla in stoc. */
void SalvareRetete(listaRetete* lista); /* Functia de salvare a modificarilor facute asupra retetelor in fisier. */
void SalvareStoc(listaMedicamente* lista); /* Functia de salvare a modificarilor facute asupra medicamentelor in fisier. */

listaMedicamente* Medicamente = NULL; /* Radacina listei simplu inlantuite. */
listaRetete* Retete = NULL; /* Radacina listei cu subliste. */

listaMedicamente* AdaugareMedicamente(listaMedicamente* lista, listaMedicamente* deAdaugat)
{
	listaMedicamente* q1, * q2;
	for (q1 = q2 = lista; q1 != NULL && strcmp(q1->nume, deAdaugat->nume) < 0; q2 = q1, q1 = q1->urm); /* Inserarea se face alfabetic. */
	if (q1 == q2) { /* Inlantuire la inceputul listei. */
		deAdaugat->urm = lista;
		lista = deAdaugat;
	}
	else { /* Inlantuire pe parcursul listei. */
		q2->urm = deAdaugat;
		deAdaugat->urm = q1;
	}
	return lista;
}

sublistaRetete* AdaugareLaReteta(sublistaRetete* lista, sublistaRetete* deAdaugat)
{
	sublistaRetete* q1, * q2;
	for (q1 = q2 = lista; q1 != NULL; q2 = q1, q1 = q1->urm);
	if (q1 == q2) {
		deAdaugat->urm = lista;
		lista = deAdaugat;
	}
	else {
		q2->urm = deAdaugat;
		deAdaugat->urm = q1;
	}
	return lista;
}

listaRetete* AdaugareRetete(listaRetete* lista, listaRetete* deAdaugat, sublistaRetete* sublista)
{
	listaRetete* q1, * q2;
	for (q1 = q2 = lista; q1 != NULL && strcmp(q1->reteta, deAdaugat->reteta) < 0; q2 = q1, q1 = q1->urm);
	if (q1 != NULL && !strcmp(q1->reteta, deAdaugat->reteta)) /* Daca reteta se afla deja in lista.. */
		q1->prim = AdaugareLaReteta(q1->prim, sublista); /* Trec la adaugarea in sublista. */
	else {
		if (q1 == q2) {
			deAdaugat->urm = lista;
			lista = deAdaugat;
		}
		else {
			q2->urm = deAdaugat;
			deAdaugat->urm = q1;
		}
		deAdaugat->prim = AdaugareLaReteta(deAdaugat->prim, sublista); /* Adaugam o alta reteta. */
	}
	return lista;
}

sublistaRetete* AdaugareDate(sublistaRetete* lista, char* medicament, int cantitate)
{
	sublistaRetete* q1, * q2, * deAdaugat;
	deAdaugat = (sublistaRetete*)malloc(sizeof(sublistaRetete));
	if ((deAdaugat->medicament = (char*)malloc(strlen(medicament) + 1 * sizeof(char))) == NULL)
		errMalloc;
	strcpy(deAdaugat->medicament, medicament);
	deAdaugat->cantitate = cantitate;
	deAdaugat->urm = NULL;
	for (q1 = q2 = lista; q1 != NULL; q2 = q1, q1 = q1->urm);
	//deAdaugat->urm = q1;
	if (q1 == q2) {
		deAdaugat->urm = lista;
		lista = deAdaugat;
	}
	else {
		q2->urm = deAdaugat;
		deAdaugat->urm = q1;
	}
	return lista;
}


listaRetete* AdaugReteta(listaRetete* lista, char* reteta, char* medicament, int cantitate)
{
	char* med;
	int c = cantitate;
	listaRetete* q1, * q2, * deAdaugat;
	deAdaugat = (listaRetete*)malloc(sizeof(listaRetete));
	if ((deAdaugat->reteta = (char*)malloc(sizeof(reteta) + 1 * sizeof(char))) == NULL)
		errMalloc;
	if ((med = (char*)malloc(strlen(medicament) + 1 * sizeof(char))) == NULL)
		errMalloc;
	strcpy(med, medicament);
	strcpy(deAdaugat->reteta, reteta);
	deAdaugat->urm = NULL;
	deAdaugat->prim = NULL;
	for (q1 = q2 = lista; q1 != NULL && strcmp(q1->reteta, deAdaugat->reteta) < 0; q2 = q1, q1 = q1->urm);
	if (q1 != NULL && !strcmp(q1->reteta, deAdaugat->reteta)) /* Daca avem deja reteta in lista... */
		q1->prim = AdaugareDate(q1->prim, med, c); /* Trecem la adaugarea in sublista. */
	else {
		if (q1 == q2) {
			deAdaugat->urm = lista;
			lista = deAdaugat;
		}
		else {
			q2->urm = deAdaugat;
			deAdaugat->urm = q1;
		}
		deAdaugat->prim = AdaugareDate(deAdaugat->prim, med, c); /* Pentru reteta introdusa de noi, trecem la adaugarea in sublista. */
	}
	return lista;
}

void StergereParametriReteta(sublistaRetete* lista)
{
	sublistaRetete* q1, * aux;
	q1 = lista;
	while (q1->urm) {
		aux = q1;
		q1 = q1->urm;
		free(aux);
	}
}

listaRetete* StergereReteta(listaRetete* lista, char* s)
{
	listaRetete* q1, * q2, * aux;
	for (q1 = q2 = lista; q1 != NULL && strcmp(q1->reteta, s) != 0; q2 = q1, q1 = q1->urm);
	if (q1 != NULL && !strcmp(q1->reteta, s)) {
		aux = q1;
		if (q1 == q2) { /* Daca se sterge prima reteta, se modifica radacina. */
			lista = q1->urm; /* Noua radacina va fi urmatoarea reteta. */
			StergereParametriReteta(aux->prim);
			free(aux);
			return lista;
		}
		else { /* Stergerea se realizeaza pe parcursul listei. */
			q2->urm = q1->urm; /* Legam reteta predecesoare de reteta urmatoare celei de sters. */
			StergereParametriReteta(aux->prim);
			free(aux);
			return lista;
		}
	}
	else {
		printf("\nReteta %s nu exista.\n", s);
		return lista;
	}
}

listaMedicamente* StergMedicament(listaMedicamente* lista, char* s)
{
	int ok = 0;
	listaMedicamente* q1, * q2;
	for (q1 = q2 = lista; q1 != NULL && strcmp(q1->nume, s) != 0; q2 = q1, q1 = q1->urm);
	if (q1 != NULL && !strcmp(q1->nume, s)) {
		ok = 1;
		if (q1 == q2)
			lista = lista->urm;
		else
			q2->urm = q1->urm;
		free(q1);
	}
	if (!ok)
		printf("\n\nMedicamentul %s nu se afla in lista cu medicamente.\n", s);
	else
		printf("\n\nMedicamentul %s a fost sters.\n", s);
	return lista;
}

listaMedicamente* AdaugMedicament(listaMedicamente* lista, char* s)
{
	listaMedicamente* q1, * q2, * aux;
	int cantitate, ok = 0;
	float pret;
	for (q1 = lista; q1 != NULL; q1 = q1->urm)
		if (!strcmp(q1->nume, s))
			ok = 1;
	if (ok) {
		printf("\n\nMedicamentul %s exista deja in baza de date.\n\n", s);
		return lista;
	}
	else {
		aux = (listaMedicamente*)malloc(sizeof(listaMedicamente)); /* Alocam spatiu pentru un nou medicament in lista. */
		if ((aux->nume = (char*)malloc(strlen(s) + 1 * sizeof(char))) == NULL)
			errMalloc;
		strcpy(aux->nume, s);
		for (q1 = q2 = lista; q1 != NULL && strcmp(q1->nume, aux->nume) < 0; q2 = q1, q1 = q1->urm);
		{
			printf("Dati cantitatea: "); scanf("%d", &cantitate);
			printf("Si pretul: "); scanf("%f", &pret);
			aux->cantitate = cantitate;
			aux->pret = pret;
			aux->urm = NULL;
			if (q1 == q2) {
				aux->urm = lista;
				lista = aux;
			}
			else {
				q2->urm = aux;
				aux->urm = q1;
			}
			return lista;
		}
	}
}

listaMedicamente* ModificareMedicament(listaMedicamente* lista, char* s)
{
	listaMedicamente* q1;
	int cantitate, optiune, ok = 0;
	float pret;
	for (q1 = lista; q1 != NULL; q1 = q1->urm)
		if (!strcmp(q1->nume, s))
			ok = 1;
	if (ok) {
		do {
			printf("\nAti introdus medicamentul %s.\nCe doriti sa modificati? (0-cantitate, 1-pret, 2-ambele)\nOptiune: ", s);
			scanf("%d", &optiune);
		} while (optiune != 0 && optiune != 1 && optiune != 2);
		switch (optiune) {
		case 0: printf("Ati ales sa modificati cantitatea.\nIntroduceti noua cantitate: ");
			scanf("%d", &cantitate);
			for (q1 = lista; q1 != NULL; q1 = q1->urm)
				if (!strcmp(q1->nume, s))
					q1->cantitate = cantitate;
			return lista;
			break;
		case 1: printf("Ati ales sa modificati pretul.\nIntroduceti noul pret: ");
			scanf("%f", &pret);
			for (q1 = lista; q1 != NULL; q1 = q1->urm)
				if (!strcmp(q1->nume, s))
					q1->pret = pret;
			return lista;
			break;
		case 2: printf("Ati ales sa modificati ambele particularitati.\nIntroduceti noua cantitate: ");
			scanf("%d", &cantitate);
			printf("Si noul pret: ");
			scanf("%f", &pret);
			for (q1 = lista; q1 != NULL; q1 = q1->urm)
				if (!strcmp(q1->nume, s)) {
					q1->cantitate = cantitate;
					q1->pret = pret;
				}
			return lista;
			break;
		}
	}
	else {
		printf("\n\nMedicamentul %s nu exista in stoc.\n\n", s);
		return lista;
	}
}

void IncarcareRetete()
{
	FILE* f_retete;
	sublistaRetete* radSublista;
	listaRetete* deAdaugat;
	char reteta[30] = { '\0' }, medicament[30] = { '\0' };
	int cantitate;
	if ((f_retete = fopen(fisierRetete, "rt")) == NULL)
		errFile;
	while (!feof(f_retete)) {
		fscanf(f_retete, "%s%s%d", reteta, medicament, &cantitate);
		radSublista = (sublistaRetete*)malloc(sizeof(sublistaRetete)); /* Aloc memorie pentru sublista unei retete. */
		if (!radSublista) errMalloc;
		radSublista->medicament = (char*)malloc(strlen(medicament) + 1 * sizeof(char));
		if (!radSublista->medicament) errMalloc;
		strcpy(radSublista->medicament, medicament);
		radSublista->cantitate = cantitate;
		deAdaugat = (listaRetete*)malloc(sizeof(listaRetete)); /* Aloc spatiu pentru reteta in sine. */
		if (!deAdaugat) errMalloc;
		deAdaugat->reteta = (char*)malloc(strlen(reteta) + 1 * sizeof(char));
		if (!deAdaugat->reteta) errMalloc;
		strcpy(deAdaugat->reteta, reteta);
		deAdaugat->prim = NULL;
		Retete = AdaugareRetete(Retete, deAdaugat, radSublista);
	}
}

void IncarcareMedicamente()
{
	FILE* f_stoc;
	listaMedicamente* deAdaugat;
	char buffer[40] = { '\0' };
	if ((f_stoc = fopen(fisierStoc, "rt")) == NULL)
		errFile;
	while (!feof(f_stoc)) {
		if ((deAdaugat = (listaMedicamente*)malloc(sizeof(listaMedicamente))) == NULL)
			errMalloc;
		fscanf(f_stoc, "%s%d%f", buffer, &deAdaugat->cantitate, &deAdaugat->pret);
		if ((deAdaugat->nume = (char*)malloc(strlen(buffer) + 1 * sizeof(char))) == NULL)
			errMalloc;
		strcpy(deAdaugat->nume, buffer);
		Medicamente = AdaugareMedicamente(Medicamente, deAdaugat);
	}
	fclose(f_stoc);
}

void AfisareMedicamente(listaMedicamente* lista)
{
	listaMedicamente* q;
	for (q = lista; q != NULL; q = q->urm)
		printf("Medicament: %s - Cantitate: %d - Pret: %g\n", q->nume, q->cantitate, q->pret);
	printf("\n\n");
}

void AfisareRetete(listaRetete* lista)
{
	listaRetete* q;
	sublistaRetete* p;
	for (q = lista; q != NULL; q = q->urm) {
		printf("\nReteta %s:\n", q->reteta);
		for (p = q->prim; p != NULL; p = p->urm)
			printf("Medicament: %s - Cantitate: %d\n", p->medicament, p->cantitate);
	}
	printf("\n\n");
}

void CautareReteta(listaRetete* lista, listaMedicamente* lista2, char* s)
{
	int ok = 0, ok2 = 0, stop = 0;
	float suma = 0;
	listaRetete* q; /* Pointer pt parcurgerea listei retetelor. */
	listaMedicamente* p; /* Pointer pt parcurgerea listei medicamentelor. */
	sublistaRetete* r; /* Pointer pentru parcurgerea sublistei retetelor pentru a vedea cantitatea. */
	for (q = lista; q != NULL; q = q->urm) {
		if (!strcmp(q->reteta, s))
		{
			ok = 1;
			printf("\nReteta %s:\n", s);
			for (p = lista2; p != NULL && !stop; p = p->urm) { /* Parcurgem lista medicamentelor... */
				for (r = q->prim; r != NULL && !stop; r = r->urm) { /* Verificam ce cantitate din medicamentul respectiv s-a prescris in reteta.. */
					if (p->cantitate != 0 && !strcmp(r->medicament, p->nume)) { /* Daca exista medicamentul in stoc si au acelasi nume. */
						ok2 = 1;
						printf("Medicament: %s - Cantitate in stoc: %d - Pret: %g\n", p->nume, p->cantitate, p->pret);
						suma = suma + (r->cantitate) * p->pret;
					}
					else if (strcmp(r->medicament, p->nume) == 0 && p->cantitate == 0) { /* S-a gasit un medicament care nu este pe stoc.. */
						printf("Medicament: %s - Cantitate in stoc: %d - Pret: %g\n", p->nume, p->cantitate, p->pret);
						stop = 1;
						suma = 0;
						ok2 = 0;
					}
				}
			}
		}
	}
	if (!ok)
		printf("\nReteta %s nu se afla in baza de date.\n", s);
	else if (ok && !ok2)
		printf("\nPentru reteta %s nu exista toate medicamentele in stoc.\n", s);
	else
		printf("\nPentru reteta %s, pretul ei total este de %g lei.\n", s, suma);
}

void SalvareStoc(listaMedicamente* lista)
{
	FILE* f_stoc;
	listaMedicamente* q1;
	
	f_stoc = fopen(fisierStoc, "rt");
	if (!f_stoc) 
		errFile;
	for (q1 = lista; q1 != NULL; q1 = q1->urm)
		fprintf(f_stoc, "%s %d %g\n", q1->nume, q1->cantitate, q1->pret);
	fclose(f_stoc);
}

void SalvareRetete(listaRetete* lista)
{
	FILE* f_retete;
	listaRetete* q;
	sublistaRetete* p;

	if ((f_retete = fopen(fisierRetete, "wt")) == NULL)
		errFile;

	for (q = lista; q != NULL; q = q->urm) {
		for (p = q->prim; p != NULL; p = p->urm)
			fprintf(f_retete, "%s %s %d\n", q->reteta, p->medicament, p->cantitate);
	}
	fclose(f_retete);
}

int main()
{
	char* nume_reteta, * med, buffer[30];

	int optiune, cantitate;

	enum { iesire, incarcareDate, afisAlfRetete, verificareReteta, afiseazaListaMed, adaugareStergere, adaugareStergereModificare, salvare } opt;

	printf("Proiect SDA - Liste cu subliste.\n");

	for (;;) {
		printf("\n1. Incarcare date din fisiere.\n");
		printf("2. Afiseaza lista retetelor in ordine alfabetica.\n");
		printf("3. Verifica pentru o reteta citita de la tastatura daca exista medicamentele in stoc si care este pretul ei in acest caz.\n");
		printf("4. Afiseaza lista medicamentelor din stoc in ordine alfabetica cu informatiile legate despre ele.\n");
		printf("5. Adauga/sterge o reteta in/din lista.\n");
		printf("6. Adauga/sterge/modifica un medicament in/din lista.\n");
		printf("7. Salvarea modificarilor inapoi in fisierele aferente ale retetelor si medicamentelor.\n");
		printf("\n0. Iesire\n");
		printf("\nDati optiunea: "); while (scanf("%d", &opt) != 1 && getchar() != '\n');

		switch (opt)
		{
		case iesire: printf("Parasire program...\n"); return 0;
			break;

		case incarcareDate:
			if (Retete && Medicamente)
				printf("\nDatele au fost deja incarcate.\n\n");
			else {
				IncarcareMedicamente();
				IncarcareRetete();
				printf("\nDatele au fost incarcate cu succes.\n\n");
			}
			break;

		case afisAlfRetete:
			if (!Retete && !Medicamente)
				printf("\nIncarcati datele intai.\n\n");
			else {
				printf("\nLista retetelor in ordine alfabetica:\n\n");
				AfisareRetete(Retete);
			}
			break;

		case verificareReteta:
			if (!Retete && !Medicamente)
				printf("\nIncarcati datele intai.\n\n");
			else {
				printf("\nDati numele retetei pe care o cautati: "); getchar();
				fgets(buffer, 30, stdin);
				buffer[strlen(buffer) - 1] = '\0';
				if ((nume_reteta = (char*)malloc(strlen(buffer) + 1 * sizeof(char))) == NULL)
					errMalloc;
				strcpy(nume_reteta, buffer);
				CautareReteta(Retete, Medicamente, nume_reteta);
				free(nume_reteta);
			}
			break;

		case afiseazaListaMed:
			if (!Retete && !Medicamente)
				printf("\nIncarcati datele intai.\n\n");
			else {
				printf("\nLista medicamentelor in ordine alfabetica:\n\n");
				AfisareMedicamente(Medicamente);
			}
			break;

		case adaugareStergere:
			if (!Retete && !Medicamente)
				printf("\nIncarcati datele intai.\n\n");
			else {
				do {
					printf("\nPentru a introduce o reteta tastati 1 iar pentru a sterge o reteta tastati 2.\nOptiune: ");
					scanf("%d", &optiune);
				} while (optiune != 1 && optiune != 2);
				switch (optiune) {
				case 1: printf("Dati numele retetei pe care doriti sa o adaugati: "); getchar();
					fgets(buffer, 30, stdin);
					buffer[strlen(buffer) - 1] = '\0';
					if ((nume_reteta = (char*)malloc(strlen(buffer) + 1 * sizeof(char))) == NULL)
						errMalloc;
					strcpy(nume_reteta, buffer);
					printf("Dati medicamentul prescris de reteta: ");
					fgets(buffer, 30, stdin);
					buffer[strlen(buffer) - 1] = '\0';
					if ((med = (char*)malloc(strlen(buffer) + 1 * sizeof(char))) == NULL)
						errMalloc;
					strcpy(med, buffer);
					printf("Dati cantitatea: ");
					scanf("%d", &cantitate);
					Retete = AdaugReteta(Retete, nume_reteta, med, cantitate);
					free(nume_reteta);
					free(med);
					break;

				case 2: printf("\nDati numele retetei pe care doriti sa o stergeti: "); getchar();
					fgets(buffer, 30, stdin);
					buffer[strlen(buffer) - 1] = '\0';
					if ((nume_reteta = (char*)malloc(strlen(buffer) + 1 * sizeof(char))) == NULL)
						errMalloc;
					strcpy(nume_reteta, buffer);
					Retete = StergereReteta(Retete, nume_reteta);
					AfisareRetete(Retete);
					free(nume_reteta);
					break;

				default: printf("\nOptiune eronata.\n");
					break;
				}
			}
			break;

		case adaugareStergereModificare:
			if (!Retete && !Medicamente)
				printf("\nIncarcati datele intai.\n\n");
			else
			{
				do {
					printf("\nPentru a introduce un medicament tastati 1.\nPentru a sterge un medicament tastati 2.\nIar pentru a modifica proprietatile unui medicament tastati 3.\nOptiune: ");
					scanf("%d", &optiune);
				} while (optiune != 1 && optiune != 2 && optiune != 3);
				switch (optiune)
				{
				case 1:
					printf("\nDati numele medicamentului pe care doriti sa il introduceti: "); getchar();
					fgets(buffer, 30, stdin);
					buffer[strlen(buffer) - 1] = '\0';
					if ((med = (char*)malloc(strlen(buffer) + 1 * sizeof(char))) == NULL)
						errMalloc;
					strcpy(med, buffer);
					Medicamente = AdaugMedicament(Medicamente, med);
					AfisareMedicamente(Medicamente);
					free(med);
					break;

				case 2: printf("\nDati numele medicamentului pe care doriti sa il stergeti: "); getchar();
					fgets(buffer, 30, stdin);
					buffer[strlen(buffer) - 1] = '\0';
					if ((med = (char*)malloc(strlen(buffer) + 1 * sizeof(char))) == NULL)
						errMalloc;
					strcpy(med, buffer);
					Medicamente = StergMedicament(Medicamente, med);
					AfisareMedicamente(Medicamente);
					free(med);
					break;

				case 3: printf("\nDati numele medicamentului pe care doriti sa il modificati: "); getchar();
					fgets(buffer, 30, stdin);
					buffer[strlen(buffer) - 1] = '\0';
					if ((med = (char*)malloc(strlen(buffer) + 1 * sizeof(char))) == NULL)
						errMalloc;
					strcpy(med, buffer);
					Medicamente = ModificareMedicament(Medicamente, med);
					AfisareMedicamente(Medicamente);
					free(med);
					break;

				default: printf("\nOptiune eronata.\n");
					break;
				}
			}
			break;

		case salvare:
			if (!Retete && !Medicamente)
				printf("\nIncarcati datele intai.\n\n");
			else {
				SalvareStoc(Medicamente);
				SalvareRetete(Retete);
				printf("\nFisierele au fost actualizate.\n\n");
			}
			break;

		default: printf("\nOptiune incorecta.\n");
			break;
		}
	}
}
