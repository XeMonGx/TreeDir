#include <printf.h>
#include "Treedir.h"

extern noeud * n;

int main(){

    creer_racine();


    FILE * f = fopen("/Users/guillaume/Dev_Code/Cours/TreeDir/test.txt", "r");
    read_file(f);
    fclose(f);

    /*
    mkdir(dupliquer_char_s("Cours"));
    cd_chem(dupliquer_char_s("Cours"));
    mkdir(dupliquer_char_s("ProjetC"));
    mkdir(dupliquer_char_s("Anglais"));
    cd_racine();
    touch(dupliquer_char_s("edt"));
    cp(dupliquer_char_s("Cours"), dupliquer_char_s("/Td"));
    print(n->racine);
    printf("\n");

    rm(dupliquer_char_s("/Td/ProjetC"));
    print(n->racine);
    printf("\n");

    rm(dupliquer_char_s("/Td/Anglais"));
    print(n->racine);
    printf("\n");

    cd_chem(dupliquer_char_s("Td"));
    mkdir(dupliquer_char_s("td1"));
    mkdir(dupliquer_char_s("td2"));
    print(n->racine);
    printf("\n");

    cp(dupliquer_char_s("/Cours/ProjetC"), dupliquer_char_s("CopieProjetC"));
    print(n->racine);
    */


    /*

    mkdir("Cours");
    mkdir("Td");
    touch("edt");

    cd_chem("Cours");
    mkdir("ProjetC");
    mkdir("Anglais");
    cd_pere();
    cd_chem("Td");
    touch("td1");
    touch(n, "td2");
    cd_pere();
    cd_chem("Cours");
    cd_chem("ProjetC");

    print(n->n);
     */
    return 0;
}