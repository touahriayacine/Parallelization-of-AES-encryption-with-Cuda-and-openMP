#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

#define block_size 128
#define char_size 8
#define N (int)sqrt(block_size/char_size)
#define encrypted_file_name "encrypted_file.txt"
#define plain_text_file_name "test.txt"

int seed = 0;

// CTR: fonction qui genere les compteurs
void generate_counter (unsigned char  * counter){
    for (int i = 0; i < block_size/char_size; i++)
    {
        srand(seed);
        counter[i] = rand()%95 + 33;
        seed++;
    }
}

// transformer un nombre decimal en hexadecimal et retourner un tableau des chiffres hexadecimaux de ce nombre
void dectohex(unsigned int num , unsigned char * hex){
    unsigned int temp, remainder;
    unsigned char tmpc;
    int i = 0;
    
    temp = num;

    while (temp != 0) {

        remainder = temp % 16;
        if (remainder < 10) {
            hex[i] = 48 + remainder;
            i++;
        } else {
            hex[i] = 55 + remainder;
            i++;
        }
        temp = temp / 16; 
    }
    if(i == 1) hex[1] = 48;
    tmpc = hex[0];
    hex[0]=hex[1];
    hex[1]=tmpc;
}

// transformer un tableau en matrice carree des hexadecimaux
void get_state(unsigned char * t , unsigned int state [N][N]){
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            state[j][i] = t[N*i +j];
        }
    }
}

// afficher un block
void print_block(unsigned int block [N][N]){
    printf("\n\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%x | ",block[i][j]);
        }
        printf("\n\n");
    }
}

// transformer un chiffre en hexadecimal au nombre decimal
int hextodec(unsigned char hex){
    if(hex >= 65){
        return hex - 55;
    }else{
        return hex - 48;
    }
}

void subBytes(unsigned int block [N][N]){
    unsigned int SBOX[]= {0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16};
    unsigned char hex[2];
    int index;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            dectohex(block[i][j], hex);
            index = hextodec(hex[0])*16 + hextodec(hex[1]);
            block[i][j] = (unsigned int) SBOX[index];
        }
    }
}

void shiftRows(unsigned int block [N][N]){
    unsigned int tmp[N-1];
    for ( int i = 1; i < N; i++)
    {
        for (int j = 0; j < i; j++)
            tmp[j] = block[i][j];
        for (int j = i; j<N; j++)
            block[i][j-i] = block[i][j];
        for (int k = N-i; k < N; k++)
            block[i][k] = tmp[k-N+i];
    }
}

void mixColumns(unsigned int block[N][N]){
    unsigned int tmp [N][N];
    unsigned int MIX_COLUMNS [4][4] = {{2 , 3 , 1 , 1},{1 , 2 , 3 , 1},{1 , 1 , 2 , 3},{3 , 1 , 1 , 2}};
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            unsigned int sum = 0;
            for (int k = 0; k < N; k++)
            {
                sum += block[k][i] * MIX_COLUMNS[j][k];
            }
            tmp[j][i] = sum;
        }
    }
    block = tmp;
}

void addRoundKey(unsigned int block[N][N] , unsigned int IV[N][N]){
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            block[i][j] ^= IV[i][j];
        }
        
    }
    
}

// lire un fichier
char * rsa(unsigned char * IV , int rounds){

    FILE * text_file = fopen(plain_text_file_name, "rb");
    if(text_file ==NULL){
        printf("ERROR !");
        exit(1);
    }
    rewind(text_file);
    FILE * encrypted_file = fopen(encrypted_file_name , "wb+");
    if(encrypted_file == NULL){
        printf("ERROR !");
        exit(1);
    }
    rewind(encrypted_file);

    unsigned char buffer [block_size / char_size];
    unsigned int cipher[N][N];
    unsigned int state [N][N];

    // transformer le vecteur en matrice
    get_state(IV , cipher);
    printf("\n\n ====== cipher =======\n");
    print_block(cipher);
    
    while (!feof(text_file))
    {
        // initialisation du buffer
        for (int i = 0; i < block_size/char_size; i++)
        {
            buffer[i] = 0;
        }

        // lire un bloque
        fread(buffer, block_size / char_size, 1, text_file);

        // transformer le tableau en matrice carree pour faciliter le calcul
        get_state(buffer , state);
        
        // AddRoundKey
        addRoundKey(state , cipher);

        // Iterer pour "rounds" fois
        for (int i = 0; i < rounds; i++)
        {
            subBytes(state);
            shiftRows(state);
            mixColumns(state);
            addRoundKey(state , cipher);
        }

        // dernier tour
        subBytes(state);
        shiftRows(state);
        addRoundKey(state , cipher);

        // ecrire le block apres encryption dans le fichier
        fwrite(buffer, block_size / char_size, 1 ,encrypted_file);
    }
    fclose(text_file);
    fclose(encrypted_file);
}

// lire un fichier
void read_file(char * name){
    FILE * f = fopen(name, "r");
    if(f == NULL){
        printf("ERROR !");
        exit(1);
    }
    rewind(f);
    unsigned char buffer[block_size / char_size];
    while (! feof(f))
    {
        fgets(buffer , block_size / char_size , f);
        printf("%s",buffer);
    }
    fclose(f);
}

int main(void){
    printf("\n========= PLAIN TEXT ==========\n\n");
    read_file(plain_text_file_name);

    unsigned char CTR[block_size/char_size];
    generate_counter(CTR);
    printf("\n\n============ CTR ===============\n\n%s\n\n", CTR);

    int rounds = 2;
    rsa(CTR,rounds);

    printf("\n======== ENCRYPTED TEXT ========\n\n");
    read_file(encrypted_file_name);
    printf("\n\n");

    return 0;
}