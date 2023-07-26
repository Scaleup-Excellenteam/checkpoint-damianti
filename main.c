#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAMELEN 50
#define CELLPHONE_LEN 50
#define NUM_COURSES 10
#define NUM_LEVELS 7
#define NUM_CLASSES 10

struct student {
    char first_name[NAMELEN];
    char last_name[NAMELEN];
    char cellphone[CELLPHONE_LEN];
    int courses[NUM_COURSES];
    struct student* next;
};

struct school {
    struct student* DB[NUM_LEVELS][NUM_CLASSES];
};
static struct school school_data;

void init_DB();
void register_student();
void print_students();
void menu();
void get_worst_students();
void get_excellent_students();
void get_avg_per_class();

int main() {

    init_DB();
//    menu();
//    register_student();
    print_students();


    return 0;
}


void init_DB() {
    // Open the file containing student data
    FILE* file;
    file = fopen("students.txt", "r");
    if (file == 0) {
        printf("Error opening file!\n");
        exit(1);
    }

    for (int level = 0; level < NUM_LEVELS; level++) {
        for (int class = 0; class < NUM_CLASSES; class++) {
            school_data.DB[level][class] = NULL;
        }
    }

    char first_name[NAMELEN], last_name[NAMELEN], cellphone[NAMELEN];
    int level, class, i;


    while (fscanf(file, "%s %s %s %d %d", first_name, last_name, cellphone, &level, &class) == 5) {
        struct student* new_student = (struct student*)malloc(sizeof(struct student));
        if (new_student == NULL) {
            printf("Memory allocation error!\n");
            exit(1);
        }

        strcpy(new_student->first_name, first_name);
        strcpy(new_student->last_name, last_name);
        strcpy(new_student->cellphone, cellphone);


        for (i = 0; i < NUM_COURSES; i++) {
            if (fscanf(file, "%d", &new_student->courses[i]) != 1) {
                printf("Error reading grades for %s %s\n", first_name, last_name);
                free(new_student);
                exit(1);
            }
        }
        new_student->next = NULL;

        // Insert the student into the school_data structure based on level and class
        if (school_data.DB[level - 1][class - 1] == NULL) {
            school_data.DB[level - 1][class - 1] = new_student;
        } else {
            struct student* current = school_data.DB[level - 1][class - 1];
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_student;
        }
    }

    fclose(file);
}


void print_students() {
    for (int level = 0; level < NUM_LEVELS; level++) {
        for (int class = 0; class < NUM_CLASSES; class++) {
            struct student* current = school_data.DB[level][class];
            while (current != NULL) {
                printf("Level %d, Class %d\n", level + 1, class + 1);
                printf("First Name: %s\n", current->first_name);
                printf("Last Name: %s\n", current->last_name);
                printf("Cellphone: %s\n", current->cellphone);
                printf("Grades: ");
                for (int i = 0; i < NUM_COURSES; i++) {
                    printf("%d ", current->courses[i]);
                }
                printf("\n\n");
                current = current->next;
            }
        }
    }
}


void menu(){
    int option;
    printf("Welcome to the menu, please input your option:"
           "0: exit, "
           "1: register, 2: get excellent students, "
           "3: get candidates for departure, 4: Average calculation per course per class");

    scanf("%d", &option);
    while (option != 0){
        switch (option) {
            case 1:
                register_student();
                break;
            case 2:
                get_excellent_students();
                break;
            case 3:
                get_worst_students();
                break;
            case 4:
                get_avg_per_class();
                break;
            default:
                printf("wrong option");
        }
        printf("please input your option:"
               "0: exit, "
               "1: register, 2: get excellent students, "
               "3: get candidates for departure, 4: Average calculation per course per class");

        scanf("%d", &option);
    }
    printf("goodbye, see you around\n");

}

void register_student(){

    printf("welcome to registration. ");



}


void get_worst_students(){


}
void get_excellent_students(){

}
void get_avg_per_class(){

}
