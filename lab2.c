// All Lab 2 Codes here
// Nazmul Haque Omi
// 6 March 2025

// Online C compiler to run C program online
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Books {
    char title[50];
    char author[50];
    char subject[100];
    int book_id;
};

int main(void) {
    // Write C code here
    
    // ------------ Example - 1 ------------
    printf("Hello World\n");
    printf("Hello World");
    
    // ------------ Example - 2 ------------
    // int a, b;
    // int c;
    // float f;
    
    // a = 10;
    // b = 20;
    // c = a + b;
    
    // double cgpa = 3.75;
    
    // printf("Value of C: %d\n", c);
    
    // f = 70.0/3.0;
    // printf("Value of F: %f\n", f);
    
    // printf("Your CGPA is: %0.2lf\n", cgpa);
    
    // ------------ Example - 3 (Taking Inputs) ------------
    // char str[100];
    // int i;
    
    // printf("Enter a value: ");
    // scanf("%s %d", str, &i);
    
    // printf("\n You have entered: %s %d", str, i);
    // int c;
    // printf("Enter a value: ");
    // c = getchar(); // Taking first char from your input stream
    
    // printf("\n You entered: ");
    // putchar(c); // Showing the char output
    
    // ------------ Example - 4 (Arrays in C) ------------
//   int n[10];
//     int i, j;
    
//     for(i = 0; i < 10; i++){
//         n[i] = i + 100;
//     }
    
//     for(j = 0; j < 10; j++){
//         printf("Element[%d] = %d\n", j, n[j]);
//     }

    // ------------ Example - 5 (Multidimentional Arrays) ------------
    // int a[5][2] = {{0, 0} ,{1, 2} ,{2, 4} ,{3, 6} ,{4, 8}};
    // int i, j;
    
    // for(i = 0; i < 5; i++){
    //     for(j = 0; j < 2; j++){
    //         printf("a[%d][%d] = %d\n", i, j, a[i][j]);
    //     }
    // }
    
    // ------------ Example - 6 (Pointer) ------------
    // int *ip;
    // double *dp;
    // float *fp;
    // char *ch;
    
    // int var = 20;
    // int var_1 = 30;
    // int *ip;
    // int  *ip_1;
    
    // ip = &var;
    // ip_1 = &var_1;
    
    // printf("Address of var variable is: %x\n", &var);
    // printf("Address stored in ip variable: %x\n", ip);
    // printf("Value of *ip variable: %d\n", *ip);
    
    // printf("\n-----------------------------\n");
    
    // printf("Address of var variable is: %x\n", &var_1);
    // printf("Address stored in ip variable: %x\n", ip_1);
    // printf("Value of *ip_1 variable: %d\n", *ip_1);
    
    
    // ------------ Example - 7 (Incrementing a Pointer) ------------
    // const int max = 3;
    
    // int var[] = {10, 200, 300};
    // int i, *ptr;
    
    // ptr = var;
    // for(i = 0; i < max; i++){
    //     printf("Address of var[%d] = %x\n", i, ptr);
    //     printf("Value of var[%d] = %d\n", i, *ptr);
    //     ptr++;
    // }
    
    // ------------ Example - 8 (Strings in C) ------------
    // char greeting[6] = {'H', 'E', 'L', 'L', 'O', '\0'};
    // char world[10] = {'W', 'O', 'R', 'L', 'D', '\0'};
    
    // char test_1[10], test_5[10], test_6[10];
    // char test_2[20] = {'O', 'M', 'I', ' ', '\0', };
    
    // // size_t test_3;
    
    
    // printf("Greeting Messege: %s\n", greeting);
    // printf("World Messege: %s\n", world);
    
    // strcpy(test_1, world); // Copies 2nd string to 1st string
    // strcat(test_2, world); // Concatenates 2nd String to 1st string
    // size_t test_3 = strlen(world); // Returns Length of String s1
    // int test_4 = strcmp(greeting, world); // compare
    // // strcpy(test_5, world); // Pointer of the 1st String
    // // strcpy(test_6, world); // pointer to the first of string s2 in s1
    
    // printf("The value of the string Test-1 is: %s\n", test_1);
    // printf("The value of the string Test-2 is: %s\n", test_2);
    // printf("The value of the string Test-3 is: %zu\n", test_3);
    // printf("The value of the string Test-4 is: %d\n", test_4);
    // printf("The value of the string Test-5 is: %s\n", test_5);
    // printf("The value of the string Test-6 is: %s\n", test_6);
    
    // ------------ Example - 9 (Struct in C) ------------
    // struct Books Book1;
    // struct Books Book2;
    
    // strcpy(Book1.title, "C Programming");
    // strcpy(Book1.author, "Nuha Ali");
    // strcpy(Book1.subject, "Computer Science and Engineering");
    // Book1.book_id = 12345;
    
    // strcpy(Book2.title, "Python Programming");
    // strcpy(Book2.author, "Subeen");
    // strcpy(Book2.subject, "Engineering");
    // Book2.book_id = 54321;
    
    // printf("Book 1 title: %s\n", Book1.title);
    // printf("Book 1 author: %s\n", Book1.author);
    // printf("Book 1 subject: %s\n", Book1.subject);
    // printf("Book 1 id: %d\n", Book1.book_id);
    
    // printf("\n ------------------ \n\n");
    
    // printf("Book 2 title: %s\n", Book2.title);
    // printf("Book 2 author: %s\n", Book2.author);
    // printf("Book 2 subject: %s\n", Book2.subject);
    // printf("Book 2 id: %d\n", Book2.book_id);
    
    
    
    return 0;   
}