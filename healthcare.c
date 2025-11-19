#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define MAX_CONTACT 15
#define MAX_DISEASE 100
#define MAX_SPEC 50
#define MAX_DATE 15

// Patient structure (Linked List Node)
typedef struct Patient {
    int patientID;
    char name[MAX_NAME];
    int age;
    char contact[MAX_CONTACT];
    char disease[MAX_DISEASE];
    struct Patient* next;  // Linked List pointer
} Patient;

// Doctor structure (Linked List Node)
typedef struct Doctor {
    int doctorID;
    char name[MAX_NAME];
    int age;
    char contact[MAX_CONTACT];
    char specialization[MAX_SPEC];
    double consultationFee;
    struct Doctor* next;  // Linked List pointer
} Doctor;

// Appointment structure (Queue Node)
typedef struct Appointment {
    int appointmentID;
    int patientID;
    int doctorID;
    char date[MAX_DATE];
    double billAmount;
    struct Appointment* next;  // Queue pointer
} Appointment;

// Global counters and head pointers
int patientCounter = 0;
int doctorCounter = 0;
int appointmentCounter = 0;

Patient* patientHead = NULL;    // Linked List head
Doctor* doctorHead = NULL;      // Linked List head
Appointment* appointmentFront = NULL;  // Queue front
Appointment* appointmentRear = NULL;   // Queue rear

// Stack for undo operations (stores last deleted patient/doctor)
typedef struct StackNode {
    char type;  // 'P' for Patient, 'D' for Doctor
    void* data;
    struct StackNode* next;
} StackNode;

StackNode* undoStack = NULL;

// Function prototypes
void push(char type, void* data);
void* pop(char* type);
void addPatient();
void addDoctor();
void bookAppointment();
void displayPatients();
void displayDoctors();
void displayAppointments();
void searchPerson();
void clearAllData();
void saveToFiles();
void loadFromFiles();
Patient* searchPatientByID(int id);
Doctor* searchDoctorByID(int id);

// Stack Operations (for undo functionality)
void push(char type, void* data) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    newNode->type = type;
    newNode->data = data;
    newNode->next = undoStack;
    undoStack = newNode;
}

void* pop(char* type) {
    if (undoStack == NULL) {
        return NULL;
    }
    StackNode* temp = undoStack;
    *type = temp->type;
    void* data = temp->data;
    undoStack = undoStack->next;
    free(temp);
    return data;
}

// Add Patient (Linked List Insert at End)
void addPatient() {
    Patient* newPatient = (Patient*)malloc(sizeof(Patient));
    
    printf("\n--- Register New Patient ---\n");
    printf("Enter Name: ");
    getchar();
    fgets(newPatient->name, MAX_NAME, stdin);
    newPatient->name[strcspn(newPatient->name, "\n")] = 0;
    
    printf("Enter Age: ");
    scanf("%d", &newPatient->age);
    getchar();
    
    printf("Enter Contact: ");
    fgets(newPatient->contact, MAX_CONTACT, stdin);
    newPatient->contact[strcspn(newPatient->contact, "\n")] = 0;
    
    printf("Enter Disease/Condition: ");
    fgets(newPatient->disease, MAX_DISEASE, stdin);
    newPatient->disease[strcspn(newPatient->disease, "\n")] = 0;
    
    newPatient->patientID = ++patientCounter;
    newPatient->next = NULL;
    
    // Insert at end of linked list
    if (patientHead == NULL) {
        patientHead = newPatient;
    } else {
        Patient* temp = patientHead;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newPatient;
    }
    
    printf("\nPatient registered successfully! ID: %d\n", newPatient->patientID);
    saveToFiles();
}

// Add Doctor (Linked List Insert at End)
void addDoctor() {
    Doctor* newDoctor = (Doctor*)malloc(sizeof(Doctor));
    
    printf("\n--- Register New Doctor ---\n");
    printf("Enter Name: ");
    getchar();
    fgets(newDoctor->name, MAX_NAME, stdin);
    newDoctor->name[strcspn(newDoctor->name, "\n")] = 0;
    
    printf("Enter Age: ");
    scanf("%d", &newDoctor->age);
    getchar();
    
    printf("Enter Contact: ");
    fgets(newDoctor->contact, MAX_CONTACT, stdin);
    newDoctor->contact[strcspn(newDoctor->contact, "\n")] = 0;
    
    printf("Enter Specialization: ");
    fgets(newDoctor->specialization, MAX_SPEC, stdin);
    newDoctor->specialization[strcspn(newDoctor->specialization, "\n")] = 0;
    
    printf("Enter Consultation Fee: ");
    scanf("%lf", &newDoctor->consultationFee);
    
    newDoctor->doctorID = ++doctorCounter;
    newDoctor->next = NULL;
    
    // Insert at end of linked list
    if (doctorHead == NULL) {
        doctorHead = newDoctor;
    } else {
        Doctor* temp = doctorHead;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newDoctor;
    }
    
    printf("\nDoctor registered successfully! ID: %d\n", newDoctor->doctorID);
    saveToFiles();
}

// Book Appointment (Queue Enqueue)
void bookAppointment() {
    if (patientHead == NULL) {
        printf("\nNo patients registered!\n");
        return;
    }
    if (doctorHead == NULL) {
        printf("\nNo doctors available!\n");
        return;
    }
    
    displayPatients();
    printf("\nEnter Patient ID: ");
    int pID;
    scanf("%d", &pID);
    
    Patient* patient = searchPatientByID(pID);
    if (patient == NULL) {
        printf("Invalid Patient ID!\n");
        return;
    }
    
    displayDoctors();
    printf("\nEnter Doctor ID: ");
    int dID;
    scanf("%d", &dID);
    
    Doctor* doctor = searchDoctorByID(dID);
    if (doctor == NULL) {
        printf("Invalid Doctor ID!\n");
        return;
    }
    
    Appointment* newAppt = (Appointment*)malloc(sizeof(Appointment));
    newAppt->appointmentID = ++appointmentCounter;
    newAppt->patientID = pID;
    newAppt->doctorID = dID;
    newAppt->billAmount = doctor->consultationFee;
    newAppt->next = NULL;
    
    printf("Enter Date (DD-MM-YYYY): ");
    scanf("%s", newAppt->date);
    
    // Enqueue (add to rear of queue)
    if (appointmentRear == NULL) {
        appointmentFront = appointmentRear = newAppt;
    } else {
        appointmentRear->next = newAppt;
        appointmentRear = newAppt;
    }
    
    printf("\nAppointment booked successfully! ID: %d\n", newAppt->appointmentID);
    saveToFiles();
}

// Search Patient by ID (Linear Search)
Patient* searchPatientByID(int id) {
    Patient* temp = patientHead;
    while (temp != NULL) {
        if (temp->patientID == id) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

// Search Doctor by ID (Linear Search)
Doctor* searchDoctorByID(int id) {
    Doctor* temp = doctorHead;
    while (temp != NULL) {
        if (temp->doctorID == id) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

// Display Patients (Linked List Traversal)
void displayPatients() {
    printf("\n--- All Patients ---\n");
    if (patientHead == NULL) {
        printf("No patients registered.\n");
        return;
    }
    
    Patient* temp = patientHead;
    while (temp != NULL) {
        printf("\nPatient ID: %d\n", temp->patientID);
        printf("Name: %s\n", temp->name);
        printf("Age: %d\n", temp->age);
        printf("Contact: %s\n", temp->contact);
        printf("Disease: %s\n", temp->disease);
        temp = temp->next;
    }
}

// Display Doctors (Linked List Traversal)
void displayDoctors() {
    printf("\n--- All Doctors ---\n");
    if (doctorHead == NULL) {
        printf("No doctors registered.\n");
        return;
    }
    
    Doctor* temp = doctorHead;
    while (temp != NULL) {
        printf("\nDoctor ID: %d\n", temp->doctorID);
        printf("Name: Dr. %s\n", temp->name);
        printf("Age: %d\n", temp->age);
        printf("Contact: %s\n", temp->contact);
        printf("Specialization: %s\n", temp->specialization);
        printf("Fee: Rs. %.2f\n", temp->consultationFee);
        temp = temp->next;
    }
}

// Display Appointments (Queue Traversal)
void displayAppointments() {
    printf("\n--- All Appointments ---\n");
    if (appointmentFront == NULL) {
        printf("No appointments scheduled.\n");
        return;
    }
    
    Appointment* temp = appointmentFront;
    while (temp != NULL) {
        printf("\nAppointment ID: %d\n", temp->appointmentID);
        printf("Patient ID: %d\n", temp->patientID);
        printf("Doctor ID: %d\n", temp->doctorID);
        printf("Date: %s\n", temp->date);
        printf("Bill Amount: Rs. %.2f\n", temp->billAmount);
        temp = temp->next;
    }
}

// Search Person (Linear Search in both lists)
void searchPerson() {
    char searchName[MAX_NAME];
    printf("\nEnter name to search: ");
    getchar();
    fgets(searchName, MAX_NAME, stdin);
    searchName[strcspn(searchName, "\n")] = 0;
    
    printf("\n--- Search Results ---\n");
    int found = 0;
    
    // Search in patients
    Patient* pTemp = patientHead;
    while (pTemp != NULL) {
        if (strstr(pTemp->name, searchName) != NULL) {
            printf("\n[PATIENT]\n");
            printf("ID: %d\n", pTemp->patientID);
            printf("Name: %s\n", pTemp->name);
            printf("Age: %d\n", pTemp->age);
            printf("Contact: %s\n", pTemp->contact);
            printf("Disease: %s\n", pTemp->disease);
            found = 1;
        }
        pTemp = pTemp->next;
    }
    
    // Search in doctors
    Doctor* dTemp = doctorHead;
    while (dTemp != NULL) {
        if (strstr(dTemp->name, searchName) != NULL) {
            printf("\n[DOCTOR]\n");
            printf("ID: %d\n", dTemp->doctorID);
            printf("Name: Dr. %s\n", dTemp->name);
            printf("Age: %d\n", dTemp->age);
            printf("Contact: %s\n", dTemp->contact);
            printf("Specialization: %s\n", dTemp->specialization);
            printf("Fee: Rs. %.2f\n", dTemp->consultationFee);
            found = 1;
        }
        dTemp = dTemp->next;
    }
    
    if (!found) {
        printf("No person found with name: %s\n", searchName);
    }
}

// Save to Files
void saveToFiles() {
    // Save patients
    FILE* pFile = fopen("patients.txt", "w");
    if (pFile != NULL) {
        Patient* temp = patientHead;
        while (temp != NULL) {
            fprintf(pFile, "%d|%s|%d|%s|%s\n",
                    temp->patientID, temp->name, temp->age,
                    temp->contact, temp->disease);
            temp = temp->next;
        }
        fclose(pFile);
    }
    
    // Save doctors
    FILE* dFile = fopen("doctors.txt", "w");
    if (dFile != NULL) {
        Doctor* temp = doctorHead;
        while (temp != NULL) {
            fprintf(dFile, "%d|%s|%d|%s|%s|%.2f\n",
                    temp->doctorID, temp->name, temp->age,
                    temp->contact, temp->specialization, temp->consultationFee);
            temp = temp->next;
        }
        fclose(dFile);
    }
    
    // Save appointments
    FILE* aFile = fopen("appointments.txt", "w");
    if (aFile != NULL) {
        Appointment* temp = appointmentFront;
        while (temp != NULL) {
            fprintf(aFile, "%d|%d|%d|%s|%.2f\n",
                    temp->appointmentID, temp->patientID,
                    temp->doctorID, temp->date, temp->billAmount);
            temp = temp->next;
        }
        fclose(aFile);
    }
}

// Load from Files
void loadFromFiles() {
    char line[500];
    
    // Load patients
    FILE* pFile = fopen("patients.txt", "r");
    if (pFile != NULL) {
        while (fgets(line, sizeof(line), pFile)) {
            Patient* p = (Patient*)malloc(sizeof(Patient));
            sscanf(line, "%d|%[^|]|%d|%[^|]|%[^\n]",
                   &p->patientID, p->name, &p->age, p->contact, p->disease);
            p->next = NULL;
            
            if (p->patientID > patientCounter) {
                patientCounter = p->patientID;
            }
            
            if (patientHead == NULL) {
                patientHead = p;
            } else {
                Patient* temp = patientHead;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = p;
            }
        }
        fclose(pFile);
        if (patientHead != NULL) {
            printf("Loaded patients from file.\n");
        }
    }
    
    // Load doctors
    FILE* dFile = fopen("doctors.txt", "r");
    if (dFile != NULL) {
        while (fgets(line, sizeof(line), dFile)) {
            Doctor* d = (Doctor*)malloc(sizeof(Doctor));
            sscanf(line, "%d|%[^|]|%d|%[^|]|%[^|]|%lf",
                   &d->doctorID, d->name, &d->age, d->contact,
                   d->specialization, &d->consultationFee);
            d->next = NULL;
            
            if (d->doctorID > doctorCounter) {
                doctorCounter = d->doctorID;
            }
            
            if (doctorHead == NULL) {
                doctorHead = d;
            } else {
                Doctor* temp = doctorHead;
                while (temp->next != NULL) {
                    temp = temp->next;
                }
                temp->next = d;
            }
        }
        fclose(dFile);
        if (doctorHead != NULL) {
            printf("Loaded doctors from file.\n");
        }
    }
    
    // Load appointments
    FILE* aFile = fopen("appointments.txt", "r");
    if (aFile != NULL) {
        while (fgets(line, sizeof(line), aFile)) {
            Appointment* a = (Appointment*)malloc(sizeof(Appointment));
            sscanf(line, "%d|%d|%d|%[^|]|%lf",
                   &a->appointmentID, &a->patientID, &a->doctorID,
                   a->date, &a->billAmount);
            a->next = NULL;
            
            if (a->appointmentID > appointmentCounter) {
                appointmentCounter = a->appointmentID;
            }
            
            if (appointmentRear == NULL) {
                appointmentFront = appointmentRear = a;
            } else {
                appointmentRear->next = a;
                appointmentRear = a;
            }
        }
        fclose(aFile);
        if (appointmentFront != NULL) {
            printf("Loaded appointments from file.\n");
        }
    }
}

// Clear All Data
void clearAllData() {
    char confirm;
    printf("\n*** WARNING: This will delete ALL data! ***\n");
    printf("Are you sure? (y/n): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        // Free all patients
        while (patientHead != NULL) {
            Patient* temp = patientHead;
            patientHead = patientHead->next;
            free(temp);
        }
        
        // Free all doctors
        while (doctorHead != NULL) {
            Doctor* temp = doctorHead;
            doctorHead = doctorHead->next;
            free(temp);
        }
        
        // Free all appointments
        while (appointmentFront != NULL) {
            Appointment* temp = appointmentFront;
            appointmentFront = appointmentFront->next;
            free(temp);
        }
        
        appointmentRear = NULL;
        patientCounter = doctorCounter = appointmentCounter = 0;
        
        // Clear files
        fopen("patients.txt", "w");
        fopen("doctors.txt", "w");
        fopen("appointments.txt", "w");
        
        printf("\nAll data cleared successfully!\n");
    } else {
        printf("\nOperation cancelled.\n");
    }
}

// Main function
int main() {
    int choice;
    
    loadFromFiles();
    
    printf("\n======================================\n");
    printf("   HOSPITAL MANAGEMENT SYSTEM (C)\n");
    printf("======================================\n");
    
    do {
        printf("\n--- Main Menu ---\n");
        printf("1. Register Patient\n");
        printf("2. Register Doctor\n");
        printf("3. Book Appointment\n");
        printf("4. View All Patients\n");
        printf("5. View All Doctors\n");
        printf("6. View All Appointments\n");
        printf("7. Search Person by Name\n");
        printf("8. Clear All Data\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                addPatient();
                break;
            case 2:
                addDoctor();
                break;
            case 3:
                bookAppointment();
                break;
            case 4:
                displayPatients();
                break;
            case 5:
                displayDoctors();
                break;
            case 6:
                displayAppointments();
                break;
            case 7:
                searchPerson();
                break;
            case 8:
                clearAllData();
                break;
            case 0:
                saveToFiles();
                printf("\nThank you for using the system!\n");
                break;
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    } while (choice != 0);
    
    return 0;
}

/*
DSA CONCEPTS DEMONSTRATED:

1. LINKED LIST:
   - Patient and Doctor data stored as singly linked lists
   - Insert at end operation
   - Traversal for display
   - Dynamic memory allocation

2. QUEUE:
   - Appointments stored as queue (FIFO)
   - Enqueue operation (add appointment)
   - Front and rear pointers

3. STACK:
   - Undo stack structure (for future undo operations)
   - Push and pop operations

4. LINEAR SEARCH:
   - Search patient/doctor by ID
   - Search person by name

5. FILE HANDLING:
   - Save data to text files
   - Load data from text files
   - Persistent storage

6. DYNAMIC MEMORY:
   - malloc() for creating nodes
   - free() for deleting nodes
   - Memory management

FILE FORMAT:
- patients.txt: ID|Name|Age|Contact|Disease
- doctors.txt: ID|Name|Age|Contact|Specialization|Fee
- appointments.txt: ID|PatientID|DoctorID|Date|Amount
*/