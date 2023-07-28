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
void free_ordered_by_name_studs(struct names_ordered_studs* root);
void free_max_grades();
struct names_ordered_studs* create_names_ordered_stud(struct student *new_student);
void insert_student_to_list_of_grades(struct student *new_student);
void insert_student_to_names_ordered_db_recursive(struct names_ordered_studs** head_ref, struct student *new_student);
struct student*  find_student(const char* first_name, const char* last_name);
void remove_student_by_name();
struct student* find_student_by_name();
void remove_stud_from_ds(struct student* stud);
void remove_student_from_students_by_name(struct student* stud);
void remove_student_from_max_grades(struct student* stud);
void remove_student_from_school(struct student* stud);
void edit_student_data();
void write_students_to_file();
struct names_ordered_studs* deleteMaxNode(struct names_ordered_studs* root, struct names_ordered_studs* maxNode);
struct names_ordered_studs* delete_node(struct names_ordered_studs* root, struct student* stud);
struct names_ordered_studs* getMaxNode(struct names_ordered_studs* root);
struct student* copy_student(struct student* orig);


int main() {

    init_DB();
    menu();
    print_students();
    write_students_to_file();


    free_students();
    free_ordered_by_name_studs(students_by_name);
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
            fclose(file);
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
        insert_student_to_list_of_grades(new_student);
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


    while (current != NULL && current->avg < new_student->avg) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        // If prev is NULL, the new student has the lowest average
        new_student->next = school_data.DB[new_student->level - 1][new_student->class - 1];
        school_data.DB[new_student->level - 1][new_student->class - 1] = new_student;
    } else {
        prev->next = new_student;
        new_student->next = current;
    }


}

void insert_student_to_list_of_grades(struct student *new_student) {

    for (int i = 0; i < NUM_COURSES; i++) {
        struct max_grades_course* new_grade = (struct max_grades_course*)malloc(sizeof(struct max_grades_course));
        if (new_grade == NULL) {
            printf("Memory allocation error!\n");
            exit(1);
        }
        new_grade->stud = new_student;
        struct max_grades_course* current = max_grades[i][new_student->level - 1];
        struct max_grades_course* prev = NULL;

        while (current != NULL && new_student->courses[i] <= current->stud->courses[i]){
            prev = current;
            current = current->next;
        }

        if (prev == NULL){

            new_grade->next = max_grades[i][new_student->level - 1];
            max_grades[i][new_student->level - 1] = new_grade;
        }
        else{
            prev->next = new_grade;
            new_grade->next = current;
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


void menu(){
    int option;
    printf("Welcome to the menu\n");
    printf("please input your option:"
           "\n\t0: exit \n\t1: register a new student\n\t2: get excellent students from school"
           "\n\t3: get candidates to kick out (worst students), \n\t4: Calculate for every class in school"
           "\n\t5: Find a student by name\n\t6: Remove a student from school by name\n. Your option: ");


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
                break;
            case 7:
                edit_student_data();
                break;
            default:
                printf("wrong option\n");
        }
        printf("please input your option:"
               "\n\t0: exit \n\t1: register a new student\n\t2: get excellent students from school"
               "\n\t3: get candidates to kick out (worst students), \n\t4: Calculate for every class in school"
               "\n\t5: Find a student by name\n\t6: Remove a student from school by name"
                "\n\t7: Edit a student's data\n. Your option: ");
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
    insert_student_to_list_of_grades(new_student);
    insert_student_to_names_ordered_db_recursive(&students_by_name, new_student);
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
            int counter = 0;
            while (current != NULL || counter < 10) {
                printf("Student: %s %s, Grade: %d\n",
                       current->stud->first_name, current->stud->last_name,
                       current->stud->courses[i]);
                current = current->next;
                counter++;
            }
        }
    }
}

void get_avg_per_class() {
    for (int level = 0; level < NUM_LEVELS; level++) {
        for (int class = 0; class < NUM_CLASSES; class++) {
            struct student* current = school_data.DB[level][class];
            double sum = 0;
            int count = 0;

            while (current != NULL) {
                sum += current->avg;
                count++;
                current = current->next;
            }

            double avg = count > 0 ? sum / count : 0;

            printf("Level %d, Class %d: Average grade: %f\n", level + 1, class + 1, avg);
        }
    }
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

void free_ordered_by_name_studs(struct names_ordered_studs *root) {

    if (root == NULL)
        return;
    free_ordered_by_name_studs(root->right);
    free_ordered_by_name_studs(root->left);

    free(root);
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

void write_students_to_file() {
    FILE* file = fopen("students_data_out.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    for (int level = 0; level < NUM_LEVELS; level++) {
        for (int class = 0; class < NUM_CLASSES; class++) {
            struct student* current = school_data.DB[level][class];
            while (current != NULL) {
                fprintf(file, "%s %s %s %d %d", current->first_name, current->last_name, current->cellphone, current->level, current->class);
                for (int i = 0; i < NUM_COURSES; i++) {
                    fprintf(file, " %d", current->courses[i]);
                }
                fprintf(file, "\n");

                current = current->next;
            }
        }
    }

    fclose(file);
}

void edit_student_data() {
    struct student* found_student = find_student_by_name();
    if (found_student){

        printf("Which field would you like to edit?\n");
        printf("1. First name\n");
        printf("2. Last name\n");
        printf("3. Cellphone\n");
        printf("4. Level\n");
        printf("5. Class\n");
        printf("6. Grades\n");

        int option;
        scanf("%d", &option);

        switch(option) {
            case 1:
                printf("Enter new first name: ");
                scanf("%s", found_student->first_name);
                break;
            case 2:
                printf("Enter new last name: ");
                scanf("%s", found_student->last_name);
                break;
            case 3:
                printf("Enter new cellphone: ");
                scanf("%s", found_student->cellphone);
                break;
            case 4:
                printf("Enter new level: ");
                scanf("%d", &found_student->level);
                break;
            case 5:
                printf("Enter new class: ");
                scanf("%d", &found_student->class);
                break;
            case 6:
                printf("Enter new grades: ");
                for(int i = 0; i < NUM_COURSES; i++) {
                    scanf("%d", &found_student->courses[i]);
                }

                double sum = 0;
                for(int i = 0; i < NUM_COURSES; i++) {
                    sum += found_student->courses[i];
                }
                found_student->avg = sum / NUM_COURSES;
                break;
            default:
                printf("Invalid option\n");
                return;
        }

        struct student* student_edited = copy_student(found_student);
        remove_stud_from_ds(found_student);
        insert_student_to_school(student_edited);
        insert_student_to_list_of_grades(student_edited);
        insert_student_to_names_ordered_db_recursive(&students_by_name, student_edited);
    }
}


struct student* copy_student(struct student* orig) {
    struct student* copy = (struct student*)malloc(sizeof(struct student));
    if (copy == NULL) {
        printf("Memory allocation error!\n");
        exit(1);
    }

    strcpy(copy->first_name, orig->first_name);
    strcpy(copy->last_name, orig->last_name);
    strcpy(copy->cellphone, orig->cellphone);
    copy->level = orig->level;
    copy->class = orig->class;
    for (int i = 0; i < NUM_COURSES; i++) {
        copy->courses[i] = orig->courses[i];
    }
    copy->avg = orig->avg;

    copy->next = NULL;
    copy->prev = NULL;

    return copy;
}
