#include <stdio.h>
#include <string.h>

#define max 50
#define file "meds.txt"

struct medicine {
    int id;
    char name[30];
    char time[15];
    int pills;
    int taken;
    int missed;
};

void loaddata(struct medicine *meds, int *count);
void savedata(const struct medicine *meds, int count);
void addmed(struct medicine *meds, int *count);
void showmeds(const struct medicine *meds, int idx, int count);
void searchmed(const struct medicine *meds, int idx, int count, const char *key, int *found);
void checkdose(struct medicine *meds, int count);
void deletemed(struct medicine *meds, int *count);
void clearbuf(void);

int main() {
    struct medicine meds[max];
    int count = 0;
    int choice = 0;

    loaddata(meds, &count);

    do {
        printf("\n=========================================\n");
        printf(" MEDICATION REMINDER SYSTEM \n");
        printf("=========================================\n");
        printf("1. Add Medicine\n");
        printf("2. View All Medicines\n");
        printf("3. Search Medicine by Name\n");
        printf("4. Mark Dose (Taken / Missed)\n");
        printf("5. Delete Medicine\n");
        printf("6. Save & Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearbuf();
            continue;
        }
        clearbuf();

        switch (choice) {
            case 1:
                addmed(meds, &count);
                break;

            case 2:
                if (count == 0) {
                    printf("\nNo medicines found.\n");
                } else {
                    printf("\nID | Name            | Time     | Pills | Taken | Missed\n");
                    printf("------------------------------------------------------------\n");
                    showmeds(meds, 0, count);
                }
                break;

            case 3: {
                if (count == 0) {
                    printf("\nNo medicines to search.\n");
                } else {
                    char key[30];
                    int found = 0;
                    printf("Enter medicine name to search: ");
                    scanf("%29s", key);
                    clearbuf();

                    searchmed(meds, 0, count, key, &found);
                    if (!found) {
                        printf("No medicine found matching '%s'.\n", key);
                    }
                }
                break;
            }

            case 4:
                checkdose(meds, count);
                break;

            case 5:
                deletemed(meds, &count);
                break;

            case 6:
                savedata(meds, count);
                printf("\nAll records saved to '%s'. Exiting...\n", file);
                break;

            default:
                printf("\nInvalid choice! Please select between 1 and 6.\n");
        }
    } while (choice != 6);

    return 0;
}

void clearbuf(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

void loaddata(struct medicine *meds, int *count) {
    FILE *fp = fopen(file, "r");
    if (fp == NULL) {
        *count = 0;
        return;
    }

    if (fscanf(fp, "%d", count) != 1 || *count < 0 || *count > max) {
        *count = 0;
        fclose(fp);
        return;
    }

    for (int i = 0; i < *count; i++) {
        if (fscanf(fp, "%d %29s %14s %d %d %d",
                   &(meds + i)->id,
                   (meds + i)->name,
                   (meds + i)->time,
                   &(meds + i)->pills,
                   &(meds + i)->taken,
                   &(meds + i)->missed) != 6) {
            *count = i;
            break;
        }
    }

    fclose(fp);
}

void savedata(const struct medicine *meds, int count) {
    FILE *fp = fopen(file, "w");
    if (fp == NULL) {
        printf("Error: Could not open '%s' for writing.\n", file);
        return;
    }

    fprintf(fp, "%d\n", count);

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d %s %s %d %d %d\n",
                (meds + i)->id,
                (meds + i)->name,
                (meds + i)->time,
                (meds + i)->pills,
                (meds + i)->taken,
                (meds + i)->missed);
    }

    fclose(fp);
}

void addmed(struct medicine *meds, int *count) {
    if (*count >= max) {
        printf("Storage full! Cannot add more than %d medicines.\n", max);
        return;
    }

    struct medicine temp;
    temp.id = *count + 1;
    temp.taken = 0;
    temp.missed = 0;

    printf("Enter Medicine Name (single word, e.g. Panadol): ");
    scanf("%29s", temp.name);
    clearbuf();

    printf("Enter Scheduled Time (e.g. 08:00AM): ");
    scanf("%14s", temp.time);
    clearbuf();

    printf("Enter Number of Pills: ");
    while (scanf("%d", &temp.pills) != 1 || temp.pills < 0) {
        printf("Please enter a valid non-negative number of pills: ");
        clearbuf();
    }
    clearbuf();

    *(meds + *count) = temp;
    (*count)++;
    savedata(meds, *count);

    printf("Medicine added and saved successfully!\n");
}

void showmeds(const struct medicine *meds, int idx, int count) {
    if (idx >= count) {
        return;
    }

    printf("%-2d | %-15s | %-8s | %-5d | %-5d | %-6d",
           (meds + idx)->id,
           (meds + idx)->name,
           (meds + idx)->time,
           (meds + idx)->pills,
           (meds + idx)->taken,
           (meds + idx)->missed);

    if ((meds + idx)->pills <= 5) {
        printf(" <-- LOW STOCK!");
    }
    printf("\n");

    showmeds(meds, idx + 1, count);
}

void searchmed(const struct medicine *meds, int idx, int count, const char *key, int *found) {
    if (idx >= count) {
        return;
    }

    if (strcmp((meds + idx)->name, key) == 0) {
        *found = 1;
        printf("\nFound -> ID: %d | Name: %s | Time: %s | Pills: %d | Taken: %d | Missed: %d\n",
               (meds + idx)->id,
               (meds + idx)->name,
               (meds + idx)->time,
               (meds + idx)->pills,
               (meds + idx)->taken,
               (meds + idx)->missed);
    }

    searchmed(meds, idx + 1, count, key, found);
}

void checkdose(struct medicine *meds, int count) {
    if (count == 0) {
        printf("\nNo medicines available to track.\n");
        return;
    }

    int id, status, found = 0;
    printf("Enter Medicine ID: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID input.\n");
        clearbuf();
        return;
    }
    clearbuf();

    for (int i = 0; i < count; i++) {
        if ((meds + i)->id == id) {
            found = 1;
            printf("1. Mark as TAKEN\n2. Mark as MISSED\nEnter status: ");
            if (scanf("%d", &status) != 1) {
                printf("Invalid status input.\n");
                clearbuf();
                return;
            }
            clearbuf();

            if (status == 1) {
                (meds + i)->taken++;
                if ((meds + i)->pills > 0) {
                    (meds + i)->pills--;
                }
                printf("Dose marked as TAKEN.\n");
                if ((meds + i)->pills <= 5) {
                    printf("LOW STOCK ALERT: Only %d pills left for %s!\n",
                           (meds + i)->pills, (meds + i)->name);
                }
            } else if (status == 2) {
                (meds + i)->missed++;
                printf("CAREGIVER ALERT: Missed dose for %s scheduled at %s!\n",
                       (meds + i)->name, (meds + i)->time);
            } else {
                printf("Invalid option! Choose 1 or 2.\n");
            }
            break;
        }
    }

    if (!found) {
        printf("Medicine ID %d not found.\n", id);
    } else {
        savedata(meds, count);
    }
}

void deletemed(struct medicine *meds, int *count) {
    if (*count == 0) {
        printf("\nNo medicines available to delete.\n");
        return;
    }

    int id, found = 0;
    printf("Enter Medicine ID to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID input.\n");
        clearbuf();
        return;
    }
    clearbuf();

    for (int i = 0; i < *count; i++) {
        if ((meds + i)->id == id) {
            found = 1;
            for (int j = i; j < *count - 1; j++) {
                *(meds + j) = *(meds + j + 1);
                (meds + j)->id = j + 1;
            }
            (*count)--;
            savedata(meds, *count);
            printf("Medicine deleted and file updated.\n");
            break;
        }
    }

    if (!found) {
        printf("Medicine ID %d not found.\n", id);
    }
}
