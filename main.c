#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAMELEN 50
#define CELLPHONE_LEN 50
#define NUM_COURSES 10
#define NUM_LEVELS 12
#define NUM_CLASSES 10
#define NUM_OF_EXCELLENCE 10

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

struct max_grades_course {
    struct student* stud;
    struct max_grades_course* next;
};


static struct max_grades_course* max_grades[NUM_COURSES][NUM_LEVELS];

static struct school school_data;

void init_DB();
void register_student();
void print_students();
void menu();
void get_worst_students();
void get_excellent_students();
void get_avg_per_class();
void insert_student_to_school(struct student* new_student, int level, int class);
void free_students();
void free_max_grades();

int main() {

    init_DB();
    menu();
    print_students();

    free_students();
    free_max_grades();
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
    struct max_grades_course* curr_max;

    // Find the appropriate position to insert the new student based on the average
    while (current != NULL && current->avg < new_student->avg) {
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
    for (int i = 0; i < NUM_COURSES; i++) {
        curr_max = max_grades[i][level - 1];

        // Add new student if list is empty or if the student's grade is higher than the current max grade.
        if (curr_max == NULL || new_student->courses[i] > curr_max->stud->courses[i]) {
            // Add new student to the front of the list.
            struct max_grades_course* new_max_grade = (struct max_grades_course*)malloc(sizeof(struct max_grades_course));
            if (new_max_grade == NULL) {
                printf("Memory allocation error!\n");
                exit(1);
            }
            new_max_grade->stud = new_student;
            new_max_grade->next = curr_max;
            max_grades[i][level - 1] = new_max_grade;
        } else {
            // Search for the right spot to add the student while keeping track of the list size.
            struct max_grades_course* prev_max = curr_max;
            int list_size = 1;

            while (curr_max->next != NULL && curr_max->next->stud->courses[i] >= new_student->courses[i]) {
                prev_max = curr_max;
                curr_max = curr_max->next;
                list_size++;
            }

            // If the student's grade is high enough, add them to the list.
            if (curr_max->next == NULL || curr_max->next->stud->courses[i] < new_student->courses[i]) {
                struct max_grades_course* new_max_grade = (struct max_grades_course*)malloc(sizeof(struct max_grades_course));
                if (new_max_grade == NULL) {
                    printf("Memory allocation error!\n");
                    exit(1);
                }
                new_max_grade->stud = new_student;
                new_max_grade->next = curr_max->next;
                curr_max->next = new_max_grade;
                list_size++;
            }

            // If the list size is now over 10, remove students from the end.
            while (list_size > NUM_OF_EXCELLENCE) {
                prev_max = max_grades[i][level - 1];
                curr_max = prev_max->next;

                // Find the second to last node.
                while (curr_max->next != NULL) {
                    prev_max = curr_max;
                    curr_max = curr_max->next;
                }

                // Remove the last node.
                free(curr_max);
                prev_max->next = NULL;
                list_size--;
            }
        }
    }
}

void menu(){
    int option;
    printf("Welcome to the menu, please select an option:\n "
           "0: exit\n 1: register \n 2: get excellent students. \n "
           "3: get candidates for departure \n 4: Average calculation per course per class\n");

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
                printf("wrong option\n");
        }
        printf("please input your option:"
               "0: exit, "
               "1: register, 2: get excellent students, "
               "3: get candidates for departure, 4: Average calculation per course per class\n");

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
    fprintf(stdout, "criteria of potential students to leave school: worst student of each class with avg smaller than 70\n");
    int level;
    for (level=0; level<NUM_LEVELS; ++level){
        int course;
        for (course=0; course<NUM_COURSES; ++course){
            struct student* stud = school_data.DB[level][course];
            if (stud != NULL && stud->avg < 70) {
                fprintf(stdout, "level: %d, course: %d, name: %s %s, avg: %f\n", level, course, stud->first_name, stud->last_name, stud->avg);
            }

        }
    }

}
void get_excellent_students() {
    struct max_grades_course* current;

    for (int i = 0; i < NUM_COURSES; i++) {
        for (int j = 0; j < NUM_LEVELS; j++) {
            printf("\nCourse %d, Level %d:\n", i + 1, j + 1);
            current = max_grades[i][j];
            while (current != NULL) {
                printf("Student: %s %s, Grade: %d\n",
                       current->stud->first_name, current->stud->last_name,
                       current->stud->courses[i]);
                current = current->next;
            }
        }
    }
}

void get_avg_per_class(){


}


void free_students(){
    int level;
    for (level = 0; level < NUM_LEVELS; ++level){
        int class;
        for (class = 0; class < NUM_CLASSES; ++class){
            struct student* current = school_data.DB[level][class];
            struct student* next;
            while (current != NULL){
                next = current->next;
                free(current);
                current = next;
            }
        }



    }


}
void free_max_grades(){
    int course;
    for (course = 0; course < NUM_COURSES; ++course){
        int level;
        for (level = 0; level < NUM_LEVELS; ++level){
            struct max_grades_course* current = max_grades[course][level];
            struct max_grades_course* next;
            while (current != NULL){
                next = current->next;
                free(current);
                current = next;
            }

        }
    }
}