#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAMELEN 50
#define CELLPHONE_LEN 50
#define NUM_COURSES 10
#define NUM_LEVELS 12
#define NUM_CLASSES 10

struct student {
    char first_name[NAMELEN];
    char last_name[NAMELEN];
    char cellphone[CELLPHONE_LEN];
    int courses[NUM_COURSES];
    double avg;
    struct student* next;
    struct student* prev;
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
void insert_student_to_school(struct student* new_student, int level, int class);

int main() {

    init_DB();
//    menu();
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

        double sum = 0;

        for (i = 0; i < NUM_COURSES; i++) {

            if (fscanf(file, "%d", &new_student->courses[i]) != 1) {
                printf("Error reading grades for %s %s\n", first_name, last_name);
                free(new_student);
                exit(1);
            }
            sum += new_student->courses[i];
        }
        new_student->avg = (double) sum / NUM_COURSES;
        new_student->next = NULL;
        new_student->prev = NULL;


        insert_student_to_school(new_student, level, class);


        // Insert the student into the school_data structure based on level and class
//        if (school_data.DB[level - 1][class - 1] == NULL) {
//            school_data.DB[level - 1][class - 1] = new_student;
//        } else {
//            struct student* current = school_data.DB[level - 1][class - 1];
//            while (current->next != NULL) {
//                current = current->next;
//            }
//            current->next = new_student;
//        }
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
                printf("\nAverage grade: %f\n", current->avg);
                printf("\n\n");
                current = current->next;
            }
        }
    }
}
void insert_student_to_school(struct student* new_student, int level, int class) {
    struct student* current = school_data.DB[level - 1][class - 1];
    struct student* prev = NULL;

    // Find the appropriate position to insert the new student based on the average
    while (current != NULL && current->avg >= new_student->avg) {
        prev = current;
        current = current->next;
    }

    // Insert the new student in the linked list
    if (prev == NULL) {
        // If prev is NULL, the new student has the highest average,
        // so it becomes the new head of the linked list.
        new_student->next = school_data.DB[level - 1][class - 1];
        school_data.DB[level - 1][class - 1] = new_student;
    } else {
        // Insert the new student between prev and current
        prev->next = new_student;
        new_student->next = current;
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
    struct student* new_student = (struct student*)malloc(sizeof(struct student));
    if (new_student == NULL) {
        printf("Memory allocation error!\n");
        exit(1);
    }
    int class_num, level_num;
    printf("welcome to registration. ");
    // Read data from the user
    printf("Enter First Name: ");
    scanf("%s", new_student->first_name);
    printf("Enter Last Name: ");
    scanf("%s", new_student->last_name);
    printf("Enter Cellphone: ");
    scanf("%s", new_student->cellphone);
    printf("Enter level Number: ");
    scanf("%d", &class_num);
    printf("Enter Class Number: ");
    scanf("%d", &level_num);

    double sum = 0;
    for (int i = 0; i < NUM_COURSES; i++) {
        printf("Enter Grade for Course %d: ", i + 1);
        scanf("%d", &new_student->courses[i]);
        sum += new_student->courses[i];
    }
    new_student->avg = (double) sum / NUM_COURSES;

    insert_student_to_school(new_student, level_num, class_num);

}


void get_worst_students(){


}
void get_excellent_students(){

}
void get_avg_per_class(){

}
