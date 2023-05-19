#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "Treedir.h"

// Variable globale

noeud * n = NULL;

// Test validité du chemin

bool verif_last_case(char * c){
    for (char * i = c; *i != '\0'; i++)
    {
        if((*i == '/' && *(i+1) == '\0') || (*i == '.' && *(i+1) == '.') || (*i == '"' && *(i+1) == '"')){
            return true;
        }
    }
    return false;
}

bool test_validite_chemin(char * c){
    //Verifie que la chaine n'est pas vide, ne commence pas par "/" et ne termine pas par "/"
    if(*c == '\0' || verif_last_case(c)){
        //printf("test %s \n", *c);
        return false;
    }
    return true;
}

// Base

void creer_racine(){
    n = calloc(1, sizeof (noeud));
    n->est_dossier = true;
    n->nom[0] = '\0';
    n->pere = n;
    n->racine = n;
    n->fils = NULL;
}

// fonction ls

void print_fils(liste_noeud * f){
    printf("%s\n", f->no->nom);
    if(f->succ != NULL) print_fils(f->succ);
}

void ls(){
    if(n->fils != NULL) print_fils(n->fils);
}

// fonction cd

void cd_racine(){
    n = n->racine;
}

// fonction cd ..

void cd_pere(){
    n = n->pere;
}

// fonction cd_chem

char *substr(char * src, int pos, int len) {
    if(len > 0){
        char *dest = calloc(len+1, sizeof(char));
        strncat(dest,src+pos,len);
        return dest;
    }
    return NULL;
}

noeud * find_noeud(liste_noeud * l, char * c){
    if (strcmp(l->no->nom, c) == 0) return l->no;
    if (l->succ != NULL) return find_noeud(l->succ, c);
    return NULL;
}

void cd_chem(char * c){
    if(!test_validite_chemin(c)){
        printf("Chemin avec un format non autorise. (%s)\n", c);
        assert(false);
    }
    char * str = dupliquer_char_s(c);
    noeud * res = NULL;
    const char * sep = "/";
    char * strTok = NULL;

    if (*str == *sep) cd_racine(); // si chemin absolut

    res = n;
    strTok = strtok(str, sep);
    while (strTok != NULL && res->fils != NULL){
        res = find_noeud(res->fils,strTok);
        if(res == NULL) break;
        strTok = strtok(NULL, sep);
    }
    free(str);
    if(strTok != NULL && res == NULL){
        printf("Erreur : Le chemin indique n'est pas accessible depuis le noeud actuel ou n'existe pas. (%s)\n",c); 
        assert(false);
    } 
    if(strTok != NULL && res->fils == NULL){
        printf("Erreur : Le chemin indique n'existe pas. (%s)\n",c); 
        assert(false);
    } 
    if(strTok == NULL && !res->est_dossier){
        printf("Erreur : Le chemin indique n'est pas un dossier. (%s)\n",c); 
        assert(false);
    } 
    n = res;
}

// fonction pwd

void pwd_fils(noeud * n){
    if (n != n->racine){
        pwd_fils(n->pere);
        printf("/%s",n->nom);
    }
}

void pwd(){
    if(n == n->racine) printf("/"); // affiche juste "/" si c'est la racine
    pwd_fils(n);
    printf("\n");
}

// Verifie s'il y'a un duplicata de nom

bool verif_existe_dupli(noeud * n, char * c){
    liste_noeud * tmp = n->fils;
    while (tmp != NULL)
    {
        if(strcmp(tmp->no->nom, c) == 0){
            return true;
        }
        tmp = tmp->succ;
    }
    return false;
}

// fonction auxilière de mkdir et touch

noeud * creer_noeud(noeud * pere, char * c, bool b){
    noeud * res = calloc(1, sizeof (noeud));
    res->est_dossier = b;
    for (int i = 0; c[i] != '\0'; ++i) {
        res->nom[i] = c[i];
    }
    res->pere = pere;
    res->racine = pere->racine;
    res->fils = NULL;
    return res;
}

liste_noeud * creer_liste_noeud(noeud * pere, char * c, bool b){
    liste_noeud * res = calloc(1, sizeof (liste_noeud));
    res->no = creer_noeud(pere, c, b);
    res->succ = NULL;
    return res;
}

void creer_fils(noeud * no, char * c, bool b){
    if (no->fils == NULL){
        no->fils = creer_liste_noeud(no, c, b);
    }else{
        if (verif_existe_dupli(no, c))
        {
            printf("Il est existe deja un dossier ou fichier avec ce nom. (%s)\n", c); 
            assert(false);
        }
        
        liste_noeud * tmp = no->fils;
        while (tmp->succ != NULL){
            tmp = tmp->succ;
        }
        tmp->succ = creer_liste_noeud(no, c, b);
    }
}

// fonction mkdir

bool verif_char(char * c){
    int lenght = strlen(c);
    if (lenght > 0 && lenght <= 99){
        for(int i=0;i<lenght;i++){
            if(isalnum( c[i] ) == false) return false;
        }
        return true;
    }
    return false;
}

void mkdir(char * c){
    if (!verif_char(c))
    {
        printf("Nom trop long ou caractère non conforme. (%s)\n", c); 
        assert(false);
    }
    creer_fils(n, c, true);
}

// fonction touch

void touch(char * c){
    if (!verif_char(c))
    {
        printf("Nom trop long ou caractère non conforme. (%s)\n", c); 
        assert(false);
    }
    creer_fils(n, c, false);
}

// fonction rm chem

int get_last_slash(char * c){
    int i = strlen(c);
    while (c[i] != '/' && i != 0){
        i--;
    }
    return i;
}

bool have_slash(char * c){
    for (char* tmp = c; *tmp != '\0' ; tmp++)
    {
        if(*tmp == '/'){
            return true;
        }
    }
    
    return false;
}

void rm_no(noeud * no);

void rm_cut(noeud * no, char * c){
    if(strcmp(no->fils->no->nom, c) == 0) {
        liste_noeud * tmp = no->fils;
        no->fils = no->fils->succ;
        rm_no(tmp->no);
        free(tmp);
    }else{
        liste_noeud * tmp = no->fils;
        char * wait = dupliquer_char_s(tmp->succ->no->nom);
        while (strcmp(wait, c) != 0 && tmp->succ != NULL){
            tmp = tmp->succ;
            free(wait);
            wait = dupliquer_char_s(tmp->succ->no->nom);
        }
        free(wait);
        liste_noeud * tmpln = tmp->succ;
        tmp->succ = tmp->succ->succ;
        rm_no(tmpln->no);
        free(tmpln);
    }
}



void rm_succ(liste_noeud * l){
    if(l != NULL){
        rm_succ(l->succ);
        rm_no(l->no);
        free(l);
    }
}

void rm_no(noeud * no){
    if(no->fils != NULL){
        rm_succ(no->fils);
    }
    free(no);
}

bool verif_arbo(noeud * n_a_supp, noeud * n_actuel);

void rm(char * c){
    if(!test_validite_chemin(c)){
        printf("Chemin avec un format non autorise. (%s)\n",c); 
        assert(false);
    }

    noeud * tmp = n; // noeud de départ

    char * tmpTwo = NULL;
    if(have_slash(c)){
        if(get_last_slash(c) != 0){
            char * tmpOne = substr(c, 0, get_last_slash(c)-1);
            cd_chem(tmpOne);
            free(tmpOne);
        }else{
            cd_racine();
        }
        tmpTwo = substr(c, get_last_slash(c)+1, strlen(c));
    }else{
        tmpTwo = substr(c, 0, strlen(c));
    }

    noeud * rm = n; // noeud du dossier/fichier à supprimer
    if(tmp != rm){
        if(!verif_arbo(tmp, rm)){
            printf("L'emplacement actuel et le chemin indiqué se superposent. (%s)\n",c);
            assert(false);
        }
        
    }
    noeud * verifExist = find_noeud(rm->fils, tmpTwo);
    if(verifExist == NULL){
        printf("L'emplacement à supprimer n'existe pas ou n'est pas accessible depuis le noeud actuel. (%s)\n",c);
        assert(false);
    }

    rm_cut(rm, tmpTwo);
    free(tmpTwo);


    n = tmp;
}

// Verifie si deux arborescence se superpose

bool verif_arbo(noeud * n_a_supp, noeud * n_actuel){
    noeud * tmp = n_a_supp;
    while (tmp != tmp->racine)
    {   
        if(n_actuel == tmp){
            return false;
        }
        tmp = tmp->pere;
    }
    return true;

}

// fonction cp chem1 chem2

void cp_no(noeud * n1, noeud * n2);

void cp_succ(liste_noeud * n, liste_noeud * l){
    liste_noeud * tmp = l;
    liste_noeud * tmpBis = n;
    while (tmp != NULL)
    {
        cp_no(tmpBis->no, tmp->no);
        tmp = tmp->succ;
        tmpBis = tmpBis->succ;
    }
    
}

void cp_no(noeud * n1, noeud * n2){
    liste_noeud * tmp = n2->fils;
    if(n2->fils)
    while(tmp != NULL){
        creer_fils(n1, tmp->no->nom, tmp->no->est_dossier);
        tmp = tmp->succ;
    }
    if(n1->fils != NULL){
        cp_succ(n1->fils, n2->fils);
    }
    
}

void cp(char * c1, char * c2){
    assert(test_validite_chemin(c1) && "Chemin avec un format non autorise. (cp c1)");
    assert(test_validite_chemin(c2) && "Chemin avec un format non autorise. (cp c2)");
    noeud * tmp = n;
    char * tmpOne = NULL;
    char * tmpTwo = dupliquer_char_s(c1);
    printf("\ntest 1\n");
    cd_chem(tmpTwo);
    free(tmpTwo);
    printf("\ntest 2\n");
    noeud * cp = n;
    n = tmp;
    
    
    tmpTwo = dupliquer_char_s(c2);
    if(*tmpTwo == '/'){
        cd_racine();
        if(get_last_slash(tmpTwo) == 0 && have_slash(tmpTwo)){
            free(tmpTwo);
            tmpTwo = NULL;
            tmpOne = substr(c2,1,strlen(c2)+1);
        }else{
            free(tmpTwo);
            tmpTwo = substr(c2,1,get_last_slash(c2)-1);
            tmpOne = substr(c2,get_last_slash(c2)+1,strlen(c2)-get_last_slash(c2)+1);
        } 
    }else{
        if(have_slash(tmpTwo)){
            free(tmpTwo);
            tmpTwo = substr(c2,0,get_last_slash(c2));
            tmpOne = substr(c2,get_last_slash(c2)+1,strlen(c2)-get_last_slash(c2)+1);
        }else{
            free(tmpTwo);
            tmpTwo = NULL;
            tmpOne = substr(c2,0,strlen(c2)+1);      
        }
        
    }
    if(tmpTwo != NULL){
        printf("\nTest %s\n",tmpTwo);
        cd_chem(tmpTwo);
    }
    free(tmpTwo);
    noeud * cmp = n;
    assert(verif_arbo(cmp, cp) && "Les deux chemins se superpose (cp)");
    printf("\ntest 3\n");
    mkdir(tmpOne);
    cd_chem(tmpOne);
    free(tmpOne);
    printf("\ntest 4\n");
    noeud * cl = n;

    cp_no(cl, cp);
    n = tmp;
}

void change_adresse_remove_old(noeud * n1){
    noeud * tmp = n1->pere;
    liste_noeud * tmpln = tmp->fils;
    if(tmpln->no == n1){
        tmp->fils = tmpln->succ;
        free(tmpln);
    }else{
        while(tmpln->succ->no != n1){
            tmpln = tmpln->succ;
        }
        liste_noeud * tmplnbis = tmpln->succ;
        tmpln->succ = tmplnbis->succ;
        free(tmplnbis);
    }
}

void change_adresse_add_new(noeud * n1, noeud * n2){
    assert(!verif_existe_dupli(n2, n1->nom) && "Il est existe deja un dossier ou fichier avec ce nom. (mv)");
    liste_noeud * ln = calloc(1, sizeof(liste_noeud));
    ln->no = n1;
    ln->succ = NULL;
    if(n2->fils == NULL){
        n2->fils = ln;
    }else{
        liste_noeud * tmp = n2->fils;
        while (tmp->succ != NULL)
        {
            tmp = tmp->succ;
        }
        tmp->succ = ln;
    }
    change_adresse_remove_old(n1);
}

// fonction mv chem1 chem2

void mv(char * c1, char * c2){
    assert(test_validite_chemin(c1) && "Chemin avec un format non autorise. (mv c1)");
    assert(test_validite_chemin(c2) && "Chemin avec un format non autorise. (mv c2)");
    noeud * res = NULL;
    noeud * res2 = NULL;
    const char * sep = "/";
    char * strTok = NULL;
    res = n;
    res2 = n;

    char * tmp = substr(c1,0,1);

    if (strcmp(tmp, sep) == 0){
        res = n->racine;
    }
    free(tmp);

    tmp = substr(c2,0,1);
    if (strcmp(tmp, sep) == 0){
        res2 = n->racine;
    }
    free(tmp);

    strTok = strtok(c1, sep);
    while (strTok != NULL && res->fils != NULL){
        res = find_noeud(res->fils,strTok);
        assert(res != NULL && "Ce chemin est innacessible depuis l'emplacement actuel (mv1)");
        strTok = strtok(NULL, sep);
    }
    strTok = strtok(c2, sep);
    while (strTok != NULL && res2->fils != NULL){
        res2 = find_noeud(res2->fils,strTok);
        assert(res2 != NULL && "Ce chemin est innacessible depuis l'emplacement actuel (mv2)");
        strTok = strtok(NULL, sep);
    }
    change_adresse_add_new(res, res2);
    // cp(c1, c2);
    // rm(c1);
}

// fonction printAux

int nb_fils(liste_noeud * l){
    if (l == NULL) return 0;
    return 1 + nb_fils(l->succ);
}

void print_list(liste_noeud * l){
    if (l != NULL){
        printf(" %s (%s)", l->no->nom, l->no->est_dossier ? "D" : "F");
        if(l->succ != NULL) printf(",");
        print_list(l->succ);
    }
}

void print_no(noeud * n){
    char * nom = n->nom;
    char * est_dossier =  n->est_dossier ? "D" : "F";
    char * pere = (n->pere->nom[0] == '\0') ? "/" : n->pere->nom;
    int nb = nb_fils(n->fils);
    if (n->nom[0] == '\0'){
        printf("Noeud / (%s), %d fils ", est_dossier, nb);
    }else{
        printf("Noeud %s (%s), pere : %s, %d fils ", nom, est_dossier, pere, nb);
    }
    if(nb != 0) printf(":");
    print_list(n->fils);
    printf("\n");
}

void printAux(noeud * n);

void print_succ(liste_noeud * l){
    printAux(l->no);
    if (l->succ != NULL) print_succ(l->succ);
}

void printAux(noeud * n){
    print_no(n);
    if (n->fils != NULL) print_succ(n->fils);
}

void print(){
    printf("\n");
    printAux(n->racine);
}

void print_mode_dossier(liste_noeud * tmp, int x);
void print_mode_arbre(noeud * n, int x);
void print_mode_fichier(noeud * n, int x);

void print_mode_arbre(noeud * n, int x){
    print_mode_fichier(n, x);
    if(n->fils != NULL){
        print_mode_dossier(n->fils, x);
    }
}

void print_mode_dossier(liste_noeud * tmp, int x){
    print_mode_arbre(tmp->no, x+1);
    if(tmp->succ != NULL) print_mode_dossier(tmp->succ, x);
}

void print_mode_fichier(noeud * n, int x){
    for (size_t i = 0; i < x; i++){
        printf("--");
    }
    printf("%s", n->nom);
    printf("\n");
}

void fermeture(noeud * n);
void fermetureBis(liste_noeud * n);

void fermeture(noeud * n){
    if(n->fils != NULL){
        fermetureBis(n->fils);
        free(n->fils);
    }
    free(n);
}

void fermetureBis(liste_noeud * n){
    if(n->succ != NULL){
        fermetureBis(n->succ);
        free(n->succ);
    }
    fermeture(n->no);
    
}
