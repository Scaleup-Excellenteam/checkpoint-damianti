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
    int level;
    int class;
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

struct names_ordered_studs{
    struct student* stud;
    struct names_ordered_studs* left;
    struct names_ordered_studs* right;
};

static struct max_grades_course* max_grades[NUM_COURSES][NUM_LEVELS];
static struct names_ordered_studs* students_by_name;
static struct school school_data;

void init_DB();
void register_student();
void print_students();
void print_student_data(const struct student* stud);
void menu();
void get_worst_students();
void get_excellent_students();
void get_avg_per_class();
void insert_student_to_school(struct student* new_student);
void free_students();
void free_max_grades();
struct names_ordered_studs* create_names_ordered_stud(struct student *new_student);
void insert_student_to_names_ordered_db_recursive(struct names_ordered_studs** head_ref, struct student *new_student);
struct student*  find_student(const char* first_name, const char* last_name);
void remove_student_by_name();
struct student* find_student_by_name();
void remove_stud_from_ds(struct student* stud);
void remove_student_from_students_by_name(struct student* stud);
void remove_student_from_max_grades(struct student* stud);
void remove_student_from_school(struct student* stud);
struct names_ordered_studs* deleteMaxNode(struct names_ordered_studs* root, struct names_ordered_studs* maxNode);
struct names_ordered_studs* delete_node(struct names_ordered_studs* root, struct student* stud);
struct names_ordered_studs* getMaxNode(struct names_ordered_studs* root);



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
            fprintf(stderr,"Memory allocation error!\n");
            free_students();
            free_max_grades();
            exit(1);
        }

        strcpy(new_student->first_name, first_name);
        strcpy(new_student->last_name, last_name);
        strcpy(new_student->cellphone, cellphone);
        new_student->level = level;
        new_student->class = class;

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


        insert_student_to_school(new_student);
        insert_student_to_names_ordered_db_recursive(&students_by_name, new_student);
    }

    fclose(file);
}
void print_student_data(const struct student* stud){

    printf("Level %d, Class %d\n", stud->level + 1, stud->class + 1);
    printf("First Name: %s\n", stud->first_name);
    printf("Last Name: %s\n", stud->last_name);
    printf("Cellphone: %s\n", stud->cellphone);
    printf("Grades: ");
    for (int i = 0; i < NUM_COURSES; i++) {
        printf("%d ", stud->courses[i]);
    }
    printf("\nAverage grade: %f\n", stud->avg);
}

void print_students() {
    for (int level = 0; level < NUM_LEVELS; level++) {
        for (int class = 0; class < NUM_CLASSES; class++) {
            struct student* current = school_data.DB[level][class];
            while (current != NULL) {
                print_student_data(current);

                current = current->next;
            }
        }
    }
}
void insert_student_to_school(struct student* new_student) {
    struct student* current = school_data.DB[new_student->level - 1][new_student->class - 1];
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
        new_student->next = school_data.DB[new_student->level - 1][new_student->class - 1];
        school_data.DB[new_student->level - 1][new_student->class - 1] = new_student;
    } else {
        // Insert the new student between prev and current
        prev->next = new_student;
        new_student->next = current;
    }
    for (int i = 0; i < NUM_COURSES; i++) {
        curr_max = max_grades[i][new_student->level - 1];

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
            max_grades[i][new_student->level - 1] = new_max_grade;
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
                prev_max = max_grades[i][new_student->level - 1];
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
    printf("Welcome to the menu, pleaseֿ\n");
    printf("please input your option:"
           "0: exit, "
           "1: register, 2: get excellent students, "
           "3: get candidates for departure, 4: Average calculation per course per class\n"
           "5: Find a student by name\n");

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
            case 5:
                find_student_by_name();
                break;
            case 6:
                remove_student_by_name();
            default:
                printf("wrong option\n");
        }
        printf("please input your option:"
               "0: exit, "
               "1: register, 2: get excellent students, "
               "3: get candidates for departure, 4: Average calculation per course per class\n"
               "5: Find a student by name\n, 6: remove a student by name");

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
    scanf("%d", &new_student->level);
    printf("Enter Class Number: ");
    scanf("%d", &new_student->class);

    double sum = 0;
    for (int i = 0; i < NUM_COURSES; i++) {
        printf("Enter Grade for Course %d: ", i + 1);
        scanf("%d", &new_student->courses[i]);
        sum += new_student->courses[i];
    }
    new_student->avg = (double) sum / NUM_COURSES;

    insert_student_to_school(new_student);

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

void insert_student_to_names_ordered_db_recursive(struct names_ordered_studs** head_ref, struct student *new_student) {
    if (*head_ref == NULL) {
        *head_ref = create_names_ordered_stud(new_student);
        return;
    }

    int result = strcmp(new_student->first_name, (*head_ref)->stud->first_name);
    if (result < 0) {
        insert_student_to_names_ordered_db_recursive(&((*head_ref)->left), new_student);
    } else if (result > 0) {

        insert_student_to_names_ordered_db_recursive(&((*head_ref)->right), new_student);
    } else {

        result = strcmp(new_student->last_name, (*head_ref)->stud->last_name);
        if (result < 0) {
            insert_student_to_names_ordered_db_recursive(&((*head_ref)->left), new_student);
        } else {
            insert_student_to_names_ordered_db_recursive(&((*head_ref)->right), new_student);
        }
    }
}


struct names_ordered_studs* create_names_ordered_stud(struct student *new_student) {
    struct names_ordered_studs* new_node = (struct names_ordered_studs*)malloc(sizeof(struct names_ordered_studs));
    if (new_node == NULL) {
        printf("Memory allocation error!\n");
        exit(1);
    }
    new_node->stud = new_student;
    new_node->left = NULL;
    new_node->right = NULL;

    return new_node;
}

struct student* find_student(const char* first_name, const char* last_name) {

    struct names_ordered_studs* current = students_by_name;
    struct student* found_student = NULL;

    while (current != NULL) {
        int result = strcmp(first_name, current->stud->first_name);
        if (result < 0) {
            current = current->left;
        } else if (result > 0) {
            current = current->right;
        } else {
            result = strcmp(last_name, current->stud->last_name);
            if (result < 0) {
                current = current->left;
            } else if (result > 0) {
                current = current->right;
            } else {
                found_student = current->stud;
                break;
            }
        }
    }

    return found_student;
}


void remove_student_by_name() {
    char option;
    struct student* stud = find_student_by_name();
    if (stud)
        remove_stud_from_ds(stud);


}

struct student* find_student_by_name(){

    char first_name[NAMELEN], last_name[NAMELEN];
    printf("Enter the first name of the student: ");
    scanf("%s", first_name);
    printf("Enter the last name of the student: ");
    scanf("%s", last_name);
    struct student* found_student = find_student(first_name, last_name);

    if (found_student != NULL) {
        print_student_data(found_student);
        return found_student;
    } else {
        printf("Could not find a student with the name %s %s\n", first_name, last_name);
        return NULL;
    }

}

void remove_stud_from_ds(struct student* stud){

    remove_student_from_max_grades(stud);
    remove_student_from_students_by_name(stud);
    remove_student_from_school(stud);


}

void remove_student_from_max_grades(struct student* stud){
    int course;
    for (course = 0; course< NUM_COURSES; ++course){

        struct max_grades_course* grade = max_grades[course][stud->level];
        struct max_grades_course* prev = NULL;

        while (grade != NULL){
            if (grade->stud == stud){
                if (prev==NULL){
                    prev = grade;
                    max_grades[course][stud->level] = grade->next;
                }
                else{
                    prev->next = grade->next;
                }
                free(grade);
                break;
            }
            prev = grade;
            grade = grade->next;
        }
    }

}

void remove_student_from_school(struct student* wanted_stud) {

    struct student* stud = school_data.DB[wanted_stud->level-1][wanted_stud->class-1];
    struct student* prev = NULL;

    while (stud != NULL){
        if (stud == wanted_stud){
            printf("FOUND IT!!");
            if (prev == NULL){
                prev = stud;
                school_data.DB[wanted_stud->level-1][wanted_stud->class-1] = stud->next;
            }
            else{
                prev->next = stud->next;
            }
            free(stud);
            break;
        }
        prev = stud;
        stud = stud->next;
    }
}

struct names_ordered_studs* deleteMaxNode(struct names_ordered_studs* root, struct names_ordered_studs* maxNode) {
    if(root == NULL)
        return NULL;

    if(root == maxNode) {
        return root->left;
    }

    root->right = deleteMaxNode(root->right, maxNode);
    return root;
}

struct names_ordered_studs* getMaxNode(struct names_ordered_studs* root) {
    while(root->right != NULL) {
        root = root->right;
    }

    return root;
}

struct names_ordered_studs* delete_node(struct names_ordered_studs* root, struct student* stud) {
    if(root == NULL)
        return NULL;

    // Assume the students are ordered by last name then first name
    int compare = strcmp(stud->last_name, root->stud->last_name);
    if(compare == 0)
        compare = strcmp(stud->first_name, root->stud->first_name);

    if(compare < 0)
        root->left = delete_node(root->left, stud);
    else if(compare > 0)
        root->right = delete_node(root->right, stud);
    else {
        if(root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }
        else if(root->left == NULL) {
            struct names_ordered_studs* temp = root->right;
            free(root);
            return temp;
        }
        else if(root->right == NULL) {
            struct names_ordered_studs* temp = root->left;
            free(root);
            return temp;
        }
        else {
            struct names_ordered_studs* temp = getMaxNode(root->left);
            root->stud = temp->stud;
            root->left = deleteMaxNode(root->left, temp);
        }
    }

    return root;
}

void remove_student_from_students_by_name(struct student* stud) {
    students_by_name = delete_node(students_by_name, stud);
}
