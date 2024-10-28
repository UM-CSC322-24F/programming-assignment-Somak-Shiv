#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined macros
#define MAX_CHARACTERS 127
#define MAX_BOATS 120
#define MAX_TAG 6

#define SLIP_RATE 12.50
#define LAND_RATE 14.00
#define TRAILOR_RATE 25.00
#define STORAGE_RATE 11.20

//-------------------------------------------------------------------
// Enums

// Define the enum for the place the boat is stored
typedef enum 
{
    slip,
    land,
    trailor,
    storage,
    no_place
} Place;

// Define functions for enum

// Function to get the place of the boat from a string (declaration)
Place getPlace(char* place);

// Function to get the string of the place of the boat (declaration)
char* getPlaceString(Place place);

//-------------------------------------------------------------------
// Union

// Define the union for the Place enum details
typedef union 
{
    int slipNumber;
    char landLocation;
    char trailorLiscense[MAX_TAG];
    int storageNumber;
} PlaceDetails;

//-------------------------------------------------------------------
// Structs

// Define the struct for the boat
typedef struct 
{
    char name[MAX_CHARACTERS];
    int length;
    Place place;
    PlaceDetails placeDetails;
    double cost;
} Boat;

//-------------------------------------------------------------------
// Function Declarations

// Function to get the Input of the User (declaration)
void getInput(char* input);

// Function to check if the user inputs a correct character (declaration)
int correctInput(char input);

// Function to give the user the function they wanted to use (declaration)
void giveFunction(char input);

// Function to check the csv file (declaration)
void checkCSV(char* csvfile);

// CompareTo Function to compare boat names for sorting (declaration)
int compareTo(const void *boat1, const void *boat2);

// Function to check Inventory (declaration)
void inventory();

// Function to Add a Boat (declaration)
void addBoat();

// Function to Remove a Boat (declaration)
void removeBoat(char* boatName);

// Function for Boat Payment (declaration)
void payment(char* boatName);

// Function to pass a month (declaration)
void passMonth();

// Function to check if a boat exists (declaration)
int checkIfBoatExists(char* boatName);

// Function to save the boats to a file (declaration)
void saveCSV(char* csvfile);

// Function to free the memory of the boats (declaration)
void freeBoats();

//-------------------------------------------------------------------
// Global Variables

Boat *boats[MAX_BOATS];
int nBoats = 0;

//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
// Main 

int main(int argn, char* argv[])
{
    // Welcome message
    printf("Welcome to the Boat Management System\n");
    printf("-------------------------------------\n");

    // load the .csv file, as well as check if the user gave two command line args
    if (argn != 2) {
        printf("ERROR: Invalid amount of args\n");
        return EXIT_FAILURE;
    }
    checkCSV(argv[1]);

    // User input variables
    char input = '\0';
    int validInput = 0;

    // Loop to get the user input
    while(input != 'X' || input != 'x') {
        getInput(&input);
        validInput = correctInput(input);
        if (validInput == 1) {
            giveFunction(input);
        } else if (validInput == 0) {
            break;
        } else {
            printf("Invalid Option %c\n", input);
            input = '\0';
        }
    }
    // Save the boats to the file and free the memory
    saveCSV(argv[1]);
    freeBoats();
    printf("\nExiting the Boat Management System\n");
}

//-------------------------------------------------------------------
//===================================================================
//-------------------------------------------------------------------
// Enum Function Implementations

// Function to get the enum of the place of the boat from a string (implementation)
Place getPlace(char* place)
{
    if (strcmp(place, "slip") == 0) {
        return slip;
    } else if (strcmp(place, "land") == 0) {
        return land;
    } else if (strcmp(place, "trailor") == 0) {
        return trailor;
    } else if (strcmp(place, "storage") == 0) {
        return storage;
    } else {
        return no_place;
    }
}

// Function to get the string of the place of the boat from the enum (implementation)
char* getPlaceString(Place place)
{
    switch(place)
    {
        case slip:
            return "slip";
        case land:
            return "land";
        case trailor:
            return "trailor";
        case storage:
            return "storage";
        case no_place:
            return "no_place";
        default:
            return "unknown";
    }
}

//-------------------------------------------------------------------
// Function Implementations

// Function to get the Input of the User (implementation)
void getInput(char* input)
{
    printf("\n(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
    scanf(" %c", input); 
}

int correctInput(char input)
{
    switch (input) {
        case 'I':
        case 'i':
            return 1;
        case 'A':
        case 'a':
            return 1;
        case 'R':
        case 'r':
            return 1;
        case 'P':
        case 'p':
            return 1;
        case 'M':
        case 'm':
            return 1;
        case 'X':
        case 'x':
            return 0;
        default:
            return -1;
    }
}



// Function to give the user the function they wanted to use (implementation)
void giveFunction(char input)
{
    
    switch (input) {
        case 'I':
        case 'i':
            inventory();
            break;
        case 'A':
        case 'a':
            addBoat();
            break;
        case 'R':
        case 'r': 
        {
            char namer[MAX_CHARACTERS];
            printf("Please enter the boat name                               : ");
            getchar();
            fgets(namer, MAX_CHARACTERS, stdin);
            namer[strcspn(namer, "\n")] = 0; 
            removeBoat(namer);
            break;
        }
        case 'P':
        case 'p':
        {
            char namep[MAX_CHARACTERS];
            printf("Please enter the boat name                               : ");
            getchar();
            fgets(namep, MAX_CHARACTERS, stdin);
            namep[strcspn(namep, "\n")] = 0;
            payment(namep);
            break;
        }
        case 'M':
        case 'm':
            passMonth();
            break;
        default:
            break;
    }
}

// Function to check the csv file (implementation)
void checkCSV(char* csvfile)
{
    // Open the file
    FILE *file = fopen(csvfile, "r");
    if (file == NULL) {
        printf("ERROR OPENING FILE\n");
        exit(EXIT_FAILURE);
    }

    // Read the file to get the boats
    char max[MAX_CHARACTERS + 100];
    while(fgets(max, MAX_CHARACTERS + 100, file)) {
        if (nBoats >= MAX_BOATS) {
            printf("Too many boats\n");
            break;
        }

    // Get the information of the boats to create structs
    Place place;
    char* token = strtok(max, ",");
    int length = atoi(strtok(NULL, ","));
    char* placeString = strtok(NULL, ",");
    place = getPlace(placeString);
    PlaceDetails placeDetails;
    
    // Get the details of the place using a switch
    switch(place) {
        case slip:
            placeDetails.slipNumber = atoi(strtok(NULL, ","));
            if (placeDetails.slipNumber < 0 || placeDetails.slipNumber > 85) {
                printf("Invalid slip number\n");
                return;
            }
            break;
        case land:
            placeDetails.landLocation = strtok(NULL, ",")[0];
            break;
        case trailor:
            strcpy(placeDetails.trailorLiscense, strtok(NULL, ","));
            if (strlen(placeDetails.trailorLiscense) > 6) {
                printf("Invalid trailor license\n");
                return;
            }
            break;
        case storage:
            placeDetails.storageNumber = atoi(strtok(NULL, ","));
            if (placeDetails.storageNumber < 0 || placeDetails.storageNumber > 50) {
                printf("Invalid storage number\n");
                return;
            }
            break;
        case no_place:
            break;
        default:
            break;
    }

    float cost = atof(strtok(NULL, ","));

    // Creating the boat now, since all details of the boat needed are found.
    Boat *boat = (Boat*) malloc(sizeof(Boat)); 
    if (boat == NULL) {
        printf("ERROR ALLOCATING MEMORY \n");
        break;
    }

    strcpy(boat->name, token);
    boat->length = length;
    boat->place = place;
    boat->placeDetails = placeDetails;
    boat->cost = cost;

    // add the new boat to the boats array and increment nBoats
    boats[nBoats] = boat;
    nBoats++;
    qsort(boats, nBoats, sizeof(Boat *), compareTo);
    } // end of While loop
    
    int closeCheck = fclose(file);
    if (closeCheck == EOF) {
        printf("ERROR CLOSING FILE\n");
        exit(EXIT_FAILURE);

    }
} 

int compareTo(const void *boat1, const void *boat2) 
{
  const Boat *boatA = *(const Boat **)boat1;
  const Boat *boatB = *(const Boat **)boat2;
  return strcasecmp(boatA->name, boatB->name);
}

// Function to check Inventory (implementation)
void inventory()
{
    // Sort the boats using compareTo function
    qsort(boats, nBoats, sizeof(Boat *), compareTo);
    
    // Print the boats
    for (int i = 0; i < nBoats; i++)
    {
        if (boats[i] != NULL) {
            // Print name
            printf("%-18s", boats[i]->name);
            // Print length
            printf("%6d'", boats[i]->length);

            // Print place
            char* placeString = getPlaceString(boats[i]->place);
            printf("%13s", placeString);
            
            // Print place details
            switch (boats[i]->place)
            {
                case slip:
                    printf("   ");
                    printf(" # %3d", boats[i]->placeDetails.slipNumber);
                    break;
                case land:
                    printf("   ");
                    printf("%6c", boats[i]->placeDetails.landLocation);
                    break;
                case trailor:
                    printf("   ");
                    printf("%6s", boats[i]->placeDetails.trailorLiscense);
                    break;
                case storage:
                    printf("   ");
                    printf(" # %3d", boats[i]->placeDetails.storageNumber);
                    break;
                default:
                    break;
            }
            // print cost
            printf("   Owes $%4.2f\n", boats[i]->cost);
        }
    }
    
}

// Function to Add a Boat (implementation)
void addBoat() 
{
    // Check if there are too many boats
    if (nBoats >= MAX_BOATS) {
        printf("Too many boats\n");
        return;
    }

    // Prompt user for boat info
    printf("Please enter the boat info in CSV format                 : ");
    char string[MAX_CHARACTERS + 100];
    scanf("%s", string);
    getchar();

    char* token = strtok(string, ",");
    int length = atoi(strtok(NULL, ","));
    char* placeString = strtok(NULL, ",");
    Place place = getPlace(placeString);
    PlaceDetails placeDetails;
    
    // Get the details of the place using a switch
    switch(place) {
        case slip:
            placeDetails.slipNumber = atoi(strtok(NULL, ","));
            if (placeDetails.slipNumber < 0 || placeDetails.slipNumber > 85) {
                printf("Invalid slip number\n");
                return;
            }
            break;
        case land:
            placeDetails.landLocation = strtok(NULL, ",")[0];
            break;
        case trailor:
            strcpy(placeDetails.trailorLiscense, strtok(NULL, ","));
            if (strlen(placeDetails.trailorLiscense) != 6) {
                printf("Invalid trailor license\n");
                return;
            }
            break;
        case storage:
            placeDetails.storageNumber = atoi(strtok(NULL, ","));
            if (placeDetails.storageNumber < 0 || placeDetails.storageNumber > 50) {
                printf("Invalid storage number\n");
                return;
            }
            break;
        case no_place:
            break;
        default:
            break;
    }

    float cost = atof(strtok(NULL, ","));

    // Creating the boat now, since all details of the boat needed are found.
    Boat *boat = (Boat*) malloc(sizeof(Boat)); 
    if (boat == NULL) {
        printf("ERROR ALLOCATING MEMORY \n");
        return;
    }

    strcpy(boat->name, token);
    boat->length = length;
    boat->place = place;
    boat->placeDetails = placeDetails;
    boat->cost = cost;

    // add the new boat to the boats array and increment nBoats
    boats[nBoats] = boat;
    nBoats++;
    qsort(boats, nBoats, sizeof(Boat *), compareTo);
}


// Function to Remove a Boat (implementation)
void removeBoat(char* boatName)
{
    // Check if boat exists
    if (checkIfBoatExists(boatName) == 1) {
        // Remove the boat
        for (int i = 0; i < nBoats; i++)
        {
            if (boats[i] != NULL && strcasecmp(boats[i]->name, boatName) == 0) {
                free(boats[i]);
                boats[i] = boats[nBoats-1];
                boats[nBoats-1] = NULL;
                nBoats--;
                return;
            }
        }
    } else {
        printf("No boat with that name\n");
    }
}

// Function for Boat Payment (implementation)
void payment(char* boatName)
{
    if (checkIfBoatExists(boatName) == 1) {
        // Get the payment amount
        float payment;
        printf("Please enter the amount to be paid                       : ");
        scanf("%f", &payment);

        // Find the boat and update the cost
        for (int i = 0; i < nBoats; i++)
        {
            if (boats[i] != NULL && strcasecmp(boats[i]->name, boatName) == 0) {
                if (payment > boats[i]->cost) {
                    printf("That is more than the amount owed, $%.2f\n", boats[i]->cost);
                    return;
                }
                boats[i]->cost -= payment;
                return;
            }
        }
    } else {
        printf("No boat with that name\n");
    }
}

// Function to pass a month (implementation)
void passMonth()
{
    // Update the cost of the boats
    for (int i = 0; i < nBoats; i++) {
        if (boats[i] != NULL) {
            float length = boats[i]->length;
            float rate;
            switch (boats[i]->place) {
                case slip:
                    rate = SLIP_RATE;
                    break;
                case land:
                    rate = LAND_RATE;
                    break;
                case trailor:
                    rate = TRAILOR_RATE;
                    break;
                case storage:
                    rate = STORAGE_RATE;
                    break;
                default:
                    rate = 0.0;
                    break;
            }

            float amountOwed = length * rate;
            boats[i]->cost += amountOwed;
        }
    }
}

// Function to check if a boat exists (implementation)
int checkIfBoatExists(char* boatName)
{
    for (int i = 0; i < nBoats; i++) {
        if (boats[i] != NULL && strcasecmp(boats[i]->name, boatName) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to save the boats to a file (implementation)
void saveCSV(char* csvfile)
{
    // Open the file to write
    FILE *file = fopen(csvfile, "w");
    if (file == NULL) {
        printf("ERROR OPENING FILE\n");
        exit(EXIT_FAILURE);
    }

    // Write the boats to the file
    for (int i = 0; i < nBoats; i++) {
        if (boats[i] != NULL) {
            fprintf(file, "%s,%d,%s,", boats[i]->name, boats[i]->length, getPlaceString(boats[i]->place));
            switch (boats[i]->place) {
                case slip:
                    fprintf(file, "%d,", boats[i]->placeDetails.slipNumber);
                    break;
                case land:
                    fprintf(file, "%c,", boats[i]->placeDetails.landLocation);
                    break;
                case trailor:
                    fprintf(file, "%s,", boats[i]->placeDetails.trailorLiscense);
                    break;
                case storage:
                    fprintf(file, "%d,", boats[i]->placeDetails.storageNumber);
                    break;
                default:
                    break;
            }
            fprintf(file, "%.2f\n", boats[i]->cost);
        }
    }

    // Close the file
    int closeCheck = fclose(file);
    if (closeCheck == EOF) {
        printf("ERROR CLOSING FILE\n");
        exit(EXIT_FAILURE);
    }
}

// Function to free the memory of the boats (implementation)
void freeBoats()
{
    for (int i = 0; i < nBoats; i++) {
        if (boats[i] != NULL) {
            free(boats[i]);
        }
    }
}

//-------------------------------------------------------------------