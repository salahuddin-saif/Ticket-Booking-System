/*
 * ======================================================================
 *                 TICKET BOOKING SYSTEM WITH CLEAN SEAT DISPLAY
 * ======================================================================
 * PROJECT: Online Ticket Booking System
 * TEAM MEMBERS:
 *   1. Md. Salah Uddin - Team Lead, Core Architecture
 *   2. Sadia Akter - User Management & Authentication
 *   3. Rakib Hasan - Ticket & Seat Management
 *   4. Tania Sultana - Booking & Payment System
 *   5. Imran Hossain - File Handling & Reports
 * ======================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN "cls"
#define SLEEP(ms) Sleep(ms)
#else
#include <unistd.h>
#define CLEAR_SCREEN "clear"
#define SLEEP(ms) usleep((ms) * 1000)
#endif

/* ======================================================================
 * SECTION 1: CONSTANTS, DATA STRUCTURES & GLOBAL VARIABLES
 * WRITTEN BY: Md. Salah Uddin 
 * ====================================================================== */

#define MAX_USERS 50
#define MAX_TICKETS 100
#define MAX_BOOKINGS 200
#define MAX_STR 100
#define MAX_SEATS 100
#define MAX_LINE 500

/* Data Structures */
typedef struct {
    int id;
    char username[30];
    char password[30];
    char fullname[50];
    char role[10];
    int active;
} User;

typedef struct {
    int id;
    char source[50];
    char destination[50];
    char date[15];
    char time[10];
    float price;
    int total_seats;
    int total_rows;
    int seats_per_row;
    int available_seats;
    char seats[MAX_SEATS][4];
    char seat_status[MAX_SEATS];
    int active;
} Ticket;

typedef struct {
    int id;
    int ticket_id;
    int user_id;
    char customer_name[50];
    char seat_numbers[MAX_SEATS * 4];
    int seats_booked;
    float total_amount;
    char booking_date[15];
    char status[15];
    char payment_method[20];
    char transaction_id[30];
} Booking;

/* Global Variables */
User users[MAX_USERS];
Ticket tickets[MAX_TICKETS];
Booking bookings[MAX_BOOKINGS];

int user_count = 0;
int ticket_count = 0;
int booking_count = 0;

User current_user;

/* ======================================================================
 * SECTION 2: UI & SPLASH SCREEN FUNCTIONS
 * WRITTEN BY: NAME-2
 * ====================================================================== */

void gotoxy(short x, short y) {
#ifdef _WIN32
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#else
    printf("\033[%d;%dH", y, x);
#endif
}

void splash_screen(void) {
    const int total = 30;
    const int startX = 18;
    const int startY = 8;
    
    system(CLEAR_SCREEN);
    
    gotoxy(startX, startY);
    printf("=========================================");
    
    gotoxy(startX, startY + 1);
    printf("      TICKET BOOKING SYSTEM LOADING");
    
    gotoxy(startX, startY + 2);
    printf("=========================================");
    
    gotoxy(startX, startY + 4);
    printf("Loading.");
    
    gotoxy(startX, startY + 5);
    printf("[                              ]   0%%");
    
    gotoxy(startX, startY + 6);
    printf("Please wait...");
    
    const char *messages[] = {
        "Initializing System...",
        "Loading User Data...",
        "Loading Ticket Data...",
        "Loading Booking Data...",
        "Connecting to Database...",
        "Starting Services...",
        "Almost Ready..."
    };
    
    for (int i = 0; i <= total; i++) {
        gotoxy(startX, startY + 4);
        printf("Loading");
        
        int dots = (i % 3) + 1;
        for (int j = 0; j < dots; j++)
            printf(".");
        for (int j = dots; j < 3; j++)
            printf(" ");
        printf("   ");
        
        gotoxy(startX, startY + 5);
        printf("[");
        
        for (int j = 0; j < total; j++) {
            if (j < i)
                printf("#");
            else
                printf(" ");
        }
        
        printf("] %3d%%", (i * 100) / total);
        
        gotoxy(startX, startY + 6);
        int msg_idx = (i * 7) / total;
        if (msg_idx > 6) msg_idx = 6;
        printf("%-25s", messages[msg_idx]);
        
        fflush(stdout);
        SLEEP(100);
    }
    
    gotoxy(startX, startY + 7);
    printf("System Ready! Press Enter to continue...");
    getchar();
    
    system(CLEAR_SCREEN);
}

void clear_screen() {
    system(CLEAR_SCREEN);
}

void print_header(const char *title) {
    clear_screen();
    printf("\n");
    print_separator('=', 70);
    center_text("TICKET BOOKING SYSTEM");
    print_separator('=', 70);
    printf("\n");
    printf("  User: %-30s Role: %s\n", 
           strlen(current_user.fullname) > 0 ? current_user.fullname : "Guest",
           strlen(current_user.role) > 0 ? current_user.role : "Not Logged In");
    printf("  Date: %-30s Time: %s\n", get_date(), get_time());
    printf("  Page: %s\n", title);
    print_separator('=', 70);
    printf("\n");
}

void print_separator(char ch, int len) {
    int i;
    for(i = 0; i < len; i++)
        printf("%c", ch);
    printf("\n");
}

void center_text(const char *text) {
    int len = strlen(text);
    int padding = (70 - len) / 2;
    if(padding < 0) padding = 0;
    printf("%*s%s\n", padding, "", text);
}

void wait_for_enter() {
    printf("\n  Press Enter to continue...");
    getchar();
}

void safe_input(char *buffer, int size) {
    fgets(buffer, size, stdin);
    int len = strlen(buffer);
    if(len > 0 && buffer[len-1] == '\n')
        buffer[len-1] = '\0';
    else
        while(getchar() != '\n');
}

void clear_input_buffer() {
    while(getchar() != '\n');
}

/* ======================================================================
 * SECTION 3: HELPER, SEAT & VALIDATION FUNCTIONS
 * WRITTEN BY: NAME-3
 * ====================================================================== */

char* get_date() {
    static char date_str[15];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(date_str, 15, "%d-%m-%Y", tm_info);
    return date_str;
}

char* get_time() {
    static char time_str[15];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(time_str, 15, "%I:%M %p", tm_info);
    return time_str;
}

int find_user_by_username(const char *username) {
    for(int i = 0; i < user_count; i++) {
        if(strcmp(users[i].username, username) == 0)
            return i;
    }
    return -1;
}

int find_user_by_id(int id) {
    for(int i = 0; i < user_count; i++) {
        if(users[i].id == id)
            return i;
    }
    return -1;
}

int find_ticket_by_id(int id) {
    for(int i = 0; i < ticket_count; i++) {
        if(tickets[i].id == id)
            return i;
    }
    return -1;
}

int find_booking_by_id(int id) {
    for(int i = 0; i < booking_count; i++) {
        if(bookings[i].id == id)
            return i;
    }
    return -1;
}

int get_next_user_id() {
    int max_id = 0;
    for(int i = 0; i < user_count; i++) {
        if(users[i].id > max_id)
            max_id = users[i].id;
    }
    return max_id + 1;
}

int get_next_ticket_id() {
    int max_id = 0;
    for(int i = 0; i < ticket_count; i++) {
        if(tickets[i].id > max_id)
            max_id = tickets[i].id;
    }
    return max_id + 1;
}

int get_next_booking_id() {
    int max_id = 0;
    for(int i = 0; i < booking_count; i++) {
        if(bookings[i].id > max_id)
            max_id = bookings[i].id;
    }
    return max_id + 1;
}

/* Case-Insensitive Functions */
void str_to_upper(char *str) {
    for(int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int str_icmp(const char *a, const char *b) {
    char temp_a[MAX_STR], temp_b[MAX_STR];
    strcpy(temp_a, a);
    strcpy(temp_b, b);
    str_to_upper(temp_a);
    str_to_upper(temp_b);
    return strcmp(temp_a, temp_b);
}

/* Seat Functions */
void initialize_seats(Ticket *ticket) {
    char rows[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int seat_count = 0;
    
    ticket->total_rows = (ticket->total_seats + ticket->seats_per_row - 1) / ticket->seats_per_row;
    if(ticket->total_rows > 26) ticket->total_rows = 26;
    
    for(int r = 0; r < ticket->total_rows && seat_count < ticket->total_seats; r++) {
        for(int c = 1; c <= ticket->seats_per_row && seat_count < ticket->total_seats; c++) {
            sprintf(ticket->seats[seat_count], "%c%d", rows[r], c);
            ticket->seat_status[seat_count] = 'O';
            seat_count++;
        }
    }
    ticket->total_seats = seat_count;
    ticket->available_seats = seat_count;
}

void display_seating_layout(const Ticket *ticket) {
    printf("\n  Seating Layout (O=Available, X=Booked):\n");
    printf("        ");
    for(int c = 1; c <= ticket->seats_per_row; c++)
        printf("  %d  ", c);
    printf("\n");
    printf("      ");
    print_separator('-', ticket->seats_per_row * 5 + 1);
    
    for(int r = 0; r < ticket->total_rows; r++) {
        printf("  %c | ", 'A' + r);
        for(int c = 0; c < ticket->seats_per_row; c++) {
            int idx = r * ticket->seats_per_row + c;
            if(idx < ticket->total_seats) {
                if(ticket->seat_status[idx] == 'O')
                    printf(" %s(O) ", ticket->seats[idx]);
                else
                    printf(" %s(X) ", ticket->seats[idx]);
            } else {
                printf("      ");
            }
        }
        printf("\n");
    }
    
    printf("\n  Legend: (O) = Available, (X) = Booked\n");
    printf("  Available Seats: %d / %d\n", ticket->available_seats, ticket->total_seats);
    printf("  (You can enter seats in any case, e.g., A1 or a1)\n");
}

int parse_seat_numbers(const char *input, char seats[MAX_SEATS][4], int *count) {
    char temp[MAX_STR];
    strcpy(temp, input);
    *count = 0;
    
    char *token = strtok(temp, " ,");
    while(token != NULL && *count < MAX_SEATS) {
        strcpy(seats[*count], token);
        str_to_upper(seats[*count]);
        (*count)++;
        token = strtok(NULL, " ,");
    }
    return (*count > 0);
}

int validate_seat_selection(const Ticket *ticket, char seats[MAX_SEATS][4], int count) {
    for(int i = 0; i < count; i++) {
        int found = 0;
        for(int j = 0; j < ticket->total_seats; j++) {
            if(str_icmp(seats[i], ticket->seats[j]) == 0) {
                found = 1;
                if(ticket->seat_status[j] == 'X') {
                    printf("  Seat %s is already booked!\n", seats[i]);
                    return 0;
                }
                break;
            }
        }
        if(!found) {
            printf("  Seat %s does not exist!\n", seats[i]);
            return 0;
        }
    }
    return 1;
}

void book_seats(Ticket *ticket, char seats[MAX_SEATS][4], int count) {
    for(int i = 0; i < count; i++) {
        for(int j = 0; j < ticket->total_seats; j++) {
            if(str_icmp(seats[i], ticket->seats[j]) == 0) {
                ticket->seat_status[j] = 'X';
                ticket->available_seats--;
                break;
            }
        }
    }
}

void release_seats(Ticket *ticket, char seats[MAX_SEATS][4], int count) {
    for(int i = 0; i < count; i++) {
        for(int j = 0; j < ticket->total_seats; j++) {
            if(str_icmp(seats[i], ticket->seats[j]) == 0) {
                ticket->seat_status[j] = 'O';
                ticket->available_seats++;
                break;
            }
        }
    }
}

/* Validation Functions */
int validate_username(const char *username) {
    int len = strlen(username);
    if(len < 3 || len > 20) return 0;
    for(int i = 0; i < len; i++) {
        if(!isalnum(username[i]) && username[i] != '_')
            return 0;
    }
    return 1;
}

int validate_password(const char *password) {
    int len = strlen(password);
    int has_upper = 0, has_lower = 0, has_digit = 0;
    if(len < 6 || len > 20) return 0;
    for(int i = 0; i < len; i++) {
        if(isupper(password[i])) has_upper = 1;
        if(islower(password[i])) has_lower = 1;
        if(isdigit(password[i])) has_digit = 1;
    }
    return (has_upper && has_lower && has_digit);
}

/* ======================================================================
 * SECTION 4: FILE HANDLING & PAYMENT FUNCTIONS
 * WRITTEN BY: NAME-4
 * ====================================================================== */

void load_users() {
    FILE *file = fopen("users.txt", "r");
    if(!file) {
        user_count = 0;
        users[0].id = 1;
        strcpy(users[0].username, "admin");
        strcpy(users[0].password, "admin123");
        strcpy(users[0].fullname, "Administrator");
        strcpy(users[0].role, "admin");
        users[0].active = 1;
        user_count = 1;
        save_users();
        return;
    }
    user_count = 0;
    while(fscanf(file, "%d,%49[^,],%29[^,],%49[^,],%9[^,],%d\n",
                 &users[user_count].id,
                 users[user_count].fullname,
                 users[user_count].username,
                 users[user_count].password,
                 users[user_count].role,
                 &users[user_count].active) == 6) {
        user_count++;
    }
    fclose(file);
}

void save_users() {
    FILE *file = fopen("users.txt", "w");
    if(!file) return;
    for(int i = 0; i < user_count; i++) {
        fprintf(file, "%d,%s,%s,%s,%s,%d\n",
                users[i].id,
                users[i].fullname,
                users[i].username,
                users[i].password,
                users[i].role,
                users[i].active);
    }
    fclose(file);
}

void load_tickets() {
    FILE *file = fopen("tickets.txt", "r");
    if(!file) {
        ticket_count = 0;
        return;
    }
    
    ticket_count = 0;
    char line[MAX_LINE];
    
    while(fgets(line, sizeof(line), file) && ticket_count < MAX_TICKETS) {
        Ticket *t = &tickets[ticket_count];
        
        if(sscanf(line, "%d,%49[^,],%49[^,],%14[^,],%9[^,],%f,%d,%d,%d,%d,%d",
                  &t->id, t->source, t->destination, t->date, t->time,
                  &t->price, &t->total_seats, &t->seats_per_row,
                  &t->total_rows, &t->available_seats, &t->active) == 11) {
            
            if(fgets(line, sizeof(line), file)) {
                char *token = strtok(line, ",");
                int i = 0;
                while(token && i < t->total_seats && i < MAX_SEATS) {
                    strcpy(t->seats[i], token);
                    token = strtok(NULL, ",");
                    i++;
                }
            }
            
            if(fgets(line, sizeof(line), file)) {
                for(int j = 0; j < t->total_seats && j < MAX_SEATS; j++) {
                    if(j < (int)strlen(line) && line[j] != '\n' && line[j] != '\r')
                        t->seat_status[j] = line[j];
                    else
                        t->seat_status[j] = 'O';
                }
            }
            
            ticket_count++;
        }
    }
    fclose(file);
}

void save_tickets() {
    FILE *file = fopen("tickets.txt", "w");
    if(!file) return;
    
    for(int i = 0; i < ticket_count; i++) {
        Ticket *t = &tickets[i];
        
        fprintf(file, "%d,%s,%s,%s,%s,%.2f,%d,%d,%d,%d,%d\n",
                t->id, t->source, t->destination, t->date, t->time,
                t->price, t->total_seats, t->seats_per_row,
                t->total_rows, t->available_seats, t->active);
        
        for(int j = 0; j < t->total_seats; j++) {
            fprintf(file, "%s", t->seats[j]);
            if(j < t->total_seats - 1)
                fprintf(file, ",");
        }
        fprintf(file, "\n");
        
        for(int j = 0; j < t->total_seats; j++) {
            fprintf(file, "%c", t->seat_status[j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void load_bookings() {
    FILE *file = fopen("bookings.txt", "r");
    if(!file) {
        booking_count = 0;
        return;
    }
    booking_count = 0;
    while(fscanf(file, "%d,%d,%d,%49[^,],%49[^,],%d,%f,%14[^,],%14[^,],%19[^,],%29[^\n]\n",
                 &bookings[booking_count].id,
                 &bookings[booking_count].ticket_id,
                 &bookings[booking_count].user_id,
                 bookings[booking_count].customer_name,
                 bookings[booking_count].seat_numbers,
                 &bookings[booking_count].seats_booked,
                 &bookings[booking_count].total_amount,
                 bookings[booking_count].booking_date,
                 bookings[booking_count].status,
                 bookings[booking_count].payment_method,
                 bookings[booking_count].transaction_id) == 11) {
        booking_count++;
    }
    fclose(file);
}

void save_bookings() {
    FILE *file = fopen("bookings.txt", "w");
    if(!file) return;
    for(int i = 0; i < booking_count; i++) {
        fprintf(file, "%d,%d,%d,%s,%s,%d,%.2f,%s,%s,%s,%s\n",
                bookings[i].id,
                bookings[i].ticket_id,
                bookings[i].user_id,
                bookings[i].customer_name,
                bookings[i].seat_numbers,
                bookings[i].seats_booked,
                bookings[i].total_amount,
                bookings[i].booking_date,
                bookings[i].status,
                bookings[i].payment_method,
                bookings[i].transaction_id);
    }
    fclose(file);
}

void load_all_data() {
    load_users();
    load_tickets();
    load_bookings();
}

void save_all_data() {
    save_users();
    save_tickets();
    save_bookings();
}

/* Payment Functions */
int process_payment(float amount, char *method, char *transaction_id) {
    int choice;
    char input[50];
    
    print_header("PAYMENT");
    printf("\n\n");
    center_text("========================================");
    center_text("       PAYMENT OPTIONS                ");
    center_text("========================================");
    printf("\n\n");
    printf("  Total Amount: %.2f BDT\n", amount);
    printf("\n");
    printf("  Select Payment Method:\n");
    printf("  [1] bKash\n");
    printf("  [2] Bank Transfer\n");
    printf("  [3] Cash (On Booking)\n");
    printf("  [0] Cancel\n");
    printf("\n");
    
    printf("  Enter choice: ");
    scanf("%d", &choice);
    clear_input_buffer();
    
    if(choice == 0) return 0;
    
    switch(choice) {
        case 1:
            strcpy(method, "bKash");
            printf("\n  bKash Payment:\n");
            printf("  Enter bKash Number: ");
            safe_input(input, 50);
            printf("  Enter PIN: ");
            safe_input(input, 50);
            printf("  bKash payment of %.2f BDT processed!\n", amount);
            sprintf(transaction_id, "BKASH-%d-%ld", rand() % 10000, time(NULL) % 10000);
            break;
            
        case 2:
            strcpy(method, "Bank Transfer");
            printf("\n  Bank Transfer:\n");
            printf("  Account Number: 1234567890\n");
            printf("  Bank: Sonali Bank, Dhaka\n");
            printf("  Enter your Account Number: ");
            safe_input(input, 50);
            printf("  Enter Reference: ");
            safe_input(input, 50);
            printf("  Bank transfer of %.2f BDT initiated!\n", amount);
            sprintf(transaction_id, "BANK-%d-%ld", rand() % 10000, time(NULL) % 10000);
            break;
            
        case 3:
            strcpy(method, "Cash");
            printf("\n  Cash payment of %.2f BDT recorded!\n", amount);
            sprintf(transaction_id, "CASH-%d-%ld", rand() % 10000, time(NULL) % 10000);
            break;
            
        default:
            printf("  Invalid choice!\n");
            return 0;
    }
    
    printf("\n  Transaction ID: %s\n", transaction_id);
    printf("  Payment successful!\n");
    wait_for_enter();
    return 1;
}

/* ======================================================================
 * SECTION 5: USER, ADMIN & CUSTOMER FUNCTIONS
 * WRITTEN BY: NAME-5
 * ====================================================================== */

/* User Functions */
int user_login() {
    char username[30], password[30];
    int attempts = 0;
    
    print_header("LOGIN");
    printf("\n\n");
    center_text("========================================");
    center_text("         LOGIN                      ");
    center_text("========================================");
    printf("\n\n");
    
    while(attempts < 3) {
        printf("  Username: ");
        safe_input(username, 30);
        printf("  Password: ");
        safe_input(password, 30);
        
        int found = find_user_by_username(username);
        if(found != -1 && strcmp(users[found].password, password) == 0 && users[found].active) {
            current_user = users[found];
            print_header("LOGIN SUCCESS");
            printf("\n\n");
            center_text("========================================");
            printf("  WELCOME BACK, %s!\n", current_user.fullname);
            center_text("========================================");
            wait_for_enter();
            return 1;
        }
        attempts++;
        printf("\n  Invalid credentials! Attempt %d of 3\n", attempts);
        wait_for_enter();
    }
    printf("\n  Maximum login attempts exceeded.\n");
    wait_for_enter();
    return 0;
}

void user_register() {
    User new_user;
    char confirm_password[30];
    
    print_header("REGISTER");
    printf("\n\n");
    center_text("========================================");
    center_text("       CREATE ACCOUNT                ");
    center_text("========================================");
    printf("\n\n");
    
    new_user.id = get_next_user_id();
    
    do {
        printf("  Username (3-20 chars): ");
        safe_input(new_user.username, 30);
        if(!validate_username(new_user.username)) {
            printf("  Invalid username!\n");
            continue;
        }
        if(find_user_by_username(new_user.username) != -1) {
            printf("  Username already exists!\n");
            continue;
        }
        break;
    } while(1);
    
    do {
        printf("  Password (6-20 chars, upper, lower, digit): ");
        safe_input(new_user.password, 30);
        if(!validate_password(new_user.password)) {
            printf("  Invalid password!\n");
            continue;
        }
        printf("  Confirm password: ");
        safe_input(confirm_password, 30);
        if(strcmp(new_user.password, confirm_password) != 0) {
            printf("  Passwords do not match!\n");
            continue;
        }
        break;
    } while(1);
    
    printf("  Full Name: ");
    safe_input(new_user.fullname, 50);
    
    strcpy(new_user.role, "customer");
    new_user.active = 1;
    
    users[user_count++] = new_user;
    save_users();
    
    print_header("REGISTRATION SUCCESS");
    printf("\n\n");
    center_text("========================================");
    center_text("  ACCOUNT CREATED SUCCESSFULLY!     ");
    center_text("========================================");
    printf("\n");
    printf("  User ID   : %d\n", new_user.id);
    printf("  Username  : %s\n", new_user.username);
    printf("  Full Name : %s\n", new_user.fullname);
    printf("\n");
    wait_for_enter();
}

void change_password() {
    char old_pass[30], new_pass[30], confirm[30];
    int user_idx = find_user_by_id(current_user.id);
    
    if(user_idx == -1) {
        printf("  User not found!\n");
        wait_for_enter();
        return;
    }
    
    print_header("CHANGE PASSWORD");
    printf("\n\n");
    center_text("========================================");
    center_text("       CHANGE PASSWORD               ");
    center_text("========================================");
    printf("\n\n");
    
    printf("  Current Password: ");
    safe_input(old_pass, 30);
    
    if(strcmp(users[user_idx].password, old_pass) != 0) {
        printf("  Incorrect current password!\n");
        wait_for_enter();
        return;
    }
    
    do {
        printf("  New Password (6-20 chars, upper, lower, digit): ");
        safe_input(new_pass, 30);
        if(!validate_password(new_pass)) {
            printf("  Invalid password!\n");
            continue;
        }
        printf("  Confirm New Password: ");
        safe_input(confirm, 30);
        if(strcmp(new_pass, confirm) != 0) {
            printf("  Passwords do not match!\n");
            continue;
        }
        break;
    } while(1);
    
    strcpy(users[user_idx].password, new_pass);
    strcpy(current_user.password, new_pass);
    save_users();
    
    print_header("PASSWORD CHANGED");
    printf("\n\n");
    center_text("========================================");
    center_text("  PASSWORD CHANGED SUCCESSFULLY!    ");
    center_text("========================================");
    wait_for_enter();
}

void view_profile() {
    int user_idx = find_user_by_id(current_user.id);
    if(user_idx == -1) {
        printf("  User not found!\n");
        wait_for_enter();
        return;
    }
    
    print_header("MY PROFILE");
    printf("\n\n");
    center_text("========================================");
    center_text("         USER PROFILE                ");
    center_text("========================================");
    printf("\n\n");
    printf("  User ID   : %d\n", users[user_idx].id);
    printf("  Username  : %s\n", users[user_idx].username);
    printf("  Full Name : %s\n", users[user_idx].fullname);
    printf("  Role      : %s\n", users[user_idx].role);
    printf("  Status    : %s\n", users[user_idx].active ? "Active" : "Inactive");
    printf("\n");
    wait_for_enter();
}

void update_profile() {
    int user_idx = find_user_by_id(current_user.id);
    if(user_idx == -1) {
        printf("  User not found!\n");
        wait_for_enter();
        return;
    }
    
    print_header("UPDATE PROFILE");
    printf("\n\n");
    center_text("========================================");
    center_text("       UPDATE PROFILE                ");
    center_text("========================================");
    printf("\n\n");
    
    printf("  Current Full Name: %s\n", users[user_idx].fullname);
    printf("  New Full Name (or press Enter to keep): ");
    char new_name[50];
    safe_input(new_name, 50);
    if(strlen(new_name) > 1) {
        strcpy(users[user_idx].fullname, new_name);
        strcpy(current_user.fullname, new_name);
        save_users();
        printf("  Profile updated successfully!\n");
    } else {
        printf("  No changes made.\n");
    }
    wait_for_enter();
}

/* Admin Functions */
void admin_menu() {
    int choice;
    while(1) {
        print_header("ADMIN DASHBOARD");
        printf("\n\n");
        center_text("========================================");
        center_text("       ADMINISTRATOR PANEL           ");
        center_text("========================================");
        printf("\n\n");
        center_text("[1] User Management");
        center_text("[2] Ticket Management");
        center_text("[3] View All Bookings");
        center_text("[4] View Profile");
        center_text("[5] Change Password");
        center_text("[6] Update Profile");
        center_text("[0] Logout");
        printf("\n\n");
        center_text("------------------------------------------------------");
        printf("\n\n");
        
        printf("  Enter choice: ");
        scanf("%d", &choice);
        clear_input_buffer();
        
        switch(choice) {
            case 1: manage_users(); break;
            case 2: manage_tickets(); break;
            case 3: view_all_bookings(); break;
            case 4: view_profile(); break;
            case 5: change_password(); break;
            case 6: update_profile(); break;
            case 0: 
                memset(&current_user, 0, sizeof(User));
                return;
            default:
                printf("  Invalid choice!\n");
                wait_for_enter();
        }
    }
}

void manage_users() {
    int choice;
    while(1) {
        print_header("USER MANAGEMENT");
        printf("\n\n");
        center_text("========================================");
        center_text("         USER MANAGEMENT              ");
        center_text("========================================");
        printf("\n\n");
        center_text("[1] List All Users");
        center_text("[2] Add User");
        center_text("[3] Edit User");
        center_text("[4] Delete User");
        center_text("[5] Toggle User Status");
        center_text("[0] Back");
        printf("\n\n");
        center_text("------------------------------------------------------");
        printf("\n\n");
        
        printf("  Enter choice: ");
        scanf("%d", &choice);
        clear_input_buffer();
        
        switch(choice) {
            case 1: list_users(); break;
            case 2: add_user(); break;
            case 3: edit_user(); break;
            case 4: delete_user(); break;
            case 5: toggle_user_status(); break;
            case 0: return;
            default:
                printf("  Invalid choice!\n");
                wait_for_enter();
        }
    }
}

void list_users() {
    print_header("ALL USERS");
    printf("\n\n");
    center_text("========================================");
    center_text("         USER LIST                   ");
    center_text("========================================");
    printf("\n\n");
    
    if(user_count == 0) {
        printf("  No users found.\n");
        wait_for_enter();
        return;
    }
    
    printf("  %-8s %-15s %-25s %-10s %-8s\n", 
           "ID", "Username", "Full Name", "Role", "Status");
    print_separator('-', 70);
    
    for(int i = 0; i < user_count; i++) {
        printf("  %-8d %-15s %-25s %-10s %-8s\n",
               users[i].id,
               users[i].username,
               users[i].fullname,
               users[i].role,
               users[i].active ? "Active" : "Inactive");
    }
    
    printf("\n  Total Users: %d\n", user_count);
    wait_for_enter();
}

void add_user() {
    User new_user;
    char confirm_password[30];
    
    print_header("ADD USER");
    printf("\n\n");
    center_text("========================================");
    center_text("         ADD NEW USER                ");
    center_text("========================================");
    printf("\n\n");
    
    new_user.id = get_next_user_id();
    
    do {
        printf("  Username: ");
        safe_input(new_user.username, 30);
        if(!validate_username(new_user.username)) {
            printf("  Invalid username!\n");
            continue;
        }
        if(find_user_by_username(new_user.username) != -1) {
            printf("  Username already exists!\n");
            continue;
        }
        break;
    } while(1);
    
    do {
        printf("  Password: ");
        safe_input(new_user.password, 30);
        if(!validate_password(new_user.password)) {
            printf("  Invalid password!\n");
            continue;
        }
        printf("  Confirm Password: ");
        safe_input(confirm_password, 30);
        if(strcmp(new_user.password, confirm_password) != 0) {
            printf("  Passwords do not match!\n");
            continue;
        }
        break;
    } while(1);
    
    printf("  Full Name: ");
    safe_input(new_user.fullname, 50);
    printf("  Role (admin/customer): ");
    safe_input(new_user.role, 10);
    if(strcmp(new_user.role, "admin") != 0 && strcmp(new_user.role, "customer") != 0)
        strcpy(new_user.role, "customer");
    
    new_user.active = 1;
    users[user_count++] = new_user;
    save_users();
    
    printf("\n  User added successfully! User ID: %d\n", new_user.id);
    wait_for_enter();
}

void edit_user() {
    int id;
    printf("  Enter User ID to edit: ");
    scanf("%d", &id);
    clear_input_buffer();
    
    int idx = find_user_by_id(id);
    if(idx == -1) {
        printf("  User not found!\n");
        wait_for_enter();
        return;
    }
    
    if(users[idx].id == current_user.id) {
        printf("  Cannot edit your own account here!\n");
        wait_for_enter();
        return;
    }
    
    print_header("EDIT USER");
    printf("\n\n");
    printf("  Editing user: %s (ID: %d)\n", users[idx].fullname, users[idx].id);
    printf("\n");
    printf("  1. Edit Full Name\n");
    printf("  2. Change Role\n");
    printf("  0. Cancel\n");
    printf("\n");
    
    int choice;
    printf("  Enter choice: ");
    scanf("%d", &choice);
    clear_input_buffer();
    
    switch(choice) {
        case 1:
            printf("  New Full Name: ");
            safe_input(users[idx].fullname, 50);
            save_users();
            printf("  Full name updated!\n");
            break;
        case 2:
            printf("  New Role (admin/customer): ");
            safe_input(users[idx].role, 10);
            if(strcmp(users[idx].role, "admin") != 0 && strcmp(users[idx].role, "customer") != 0)
                strcpy(users[idx].role, "customer");
            save_users();
            printf("  Role updated!\n");
            break;
        case 0:
            printf("  Cancelled.\n");
            break;
        default:
            printf("  Invalid choice!\n");
    }
    wait_for_enter();
}

void delete_user() {
    int id;
    printf("  Enter User ID to delete: ");
    scanf("%d", &id);
    clear_input_buffer();
    
    if(id == current_user.id) {
        printf("  Cannot delete your own account!\n");
        wait_for_enter();
        return;
    }
    
    int idx = find_user_by_id(id);
    if(idx == -1) {
        printf("  User not found!\n");
        wait_for_enter();
        return;
    }
    
    char confirm;
    printf("  Are you sure you want to delete user %s? (y/n): ", users[idx].fullname);
    scanf("%c", &confirm);
    clear_input_buffer();
    
    if(confirm == 'y' || confirm == 'Y') {
        for(int i = idx; i < user_count - 1; i++)
            users[i] = users[i + 1];
        user_count--;
        save_users();
        printf("  User deleted successfully!\n");
    } else {
        printf("  Deletion cancelled.\n");
    }
    wait_for_enter();
}

void toggle_user_status() {
    int id;
    printf("  Enter User ID to toggle status: ");
    scanf("%d", &id);
    clear_input_buffer();
    
    if(id == current_user.id) {
        printf("  Cannot change your own status!\n");
        wait_for_enter();
        return;
    }
    
    int idx = find_user_by_id(id);
    if(idx == -1) {
        printf("  User not found!\n");
        wait_for_enter();
        return;
    }
    
    users[idx].active = !users[idx].active;
    save_users();
    printf("  User status toggled to: %s\n", users[idx].active ? "Active" : "Inactive");
    wait_for_enter();
}

void manage_tickets() {
    int choice;
    while(1) {
        print_header("TICKET MANAGEMENT");
        printf("\n\n");
        center_text("========================================");
        center_text("         TICKET MANAGEMENT            ");
        center_text("========================================");
        printf("\n\n");
        center_text("[1] List All Tickets");
        center_text("[2] Add Ticket");
        center_text("[3] Edit Ticket");
        center_text("[4] Delete Ticket");
        center_text("[0] Back");
        printf("\n\n");
        center_text("------------------------------------------------------");
        printf("\n\n");
        
        printf("  Enter choice: ");
        scanf("%d", &choice);
        clear_input_buffer();
        
        switch(choice) {
            case 1: list_tickets(); break;
            case 2: add_ticket(); break;
            case 3: edit_ticket(); break;
            case 4: delete_ticket(); break;
            case 0: return;
            default:
                printf("  Invalid choice!\n");
                wait_for_enter();
        }
    }
}

void list_tickets() {
    print_header("ALL TICKETS");
    printf("\n\n");
    center_text("========================================");
    center_text("         TICKET LIST                 ");
    center_text("========================================");
    printf("\n\n");
    
    if(ticket_count == 0) {
        printf("  No tickets found.\n");
        wait_for_enter();
        return;
    }
    
    printf("  %-6s %-15s %-15s %-12s %-8s %-8s %-8s %-8s\n",
           "ID", "Source", "Destination", "Date", "Time", "Price", "Available", "Status");
    print_separator('-', 95);
    
    for(int i = 0; i < ticket_count; i++) {
        printf("  %-6d %-15s %-15s %-12s %-8s %-8.2f %-8d %-8s\n",
               tickets[i].id,
               tickets[i].source,
               tickets[i].destination,
               tickets[i].date,
               tickets[i].time,
               tickets[i].price,
               tickets[i].available_seats,
               tickets[i].active ? "Active" : "Inactive");
    }
    
    printf("\n  Total Tickets: %d\n", ticket_count);
    wait_for_enter();
}

void add_ticket() {
    Ticket new_ticket;
    
    print_header("ADD TICKET");
    printf("\n\n");
    center_text("========================================");
    center_text("         ADD NEW TICKET               ");
    center_text("========================================");
    printf("\n\n");
    
    new_ticket.id = get_next_ticket_id();
    
    printf("  Source: ");
    safe_input(new_ticket.source, 50);
    printf("  Destination: ");
    safe_input(new_ticket.destination, 50);
    printf("  Date (DD-MM-YYYY): ");
    safe_input(new_ticket.date, 15);
    printf("  Time (HH:MM): ");
    safe_input(new_ticket.time, 10);
    printf("  Price (BDT): ");
    scanf("%f", &new_ticket.price);
    clear_input_buffer();
    
    do {
        printf("  Total Seats: ");
        scanf("%d", &new_ticket.total_seats);
        clear_input_buffer();
        if(new_ticket.total_seats > 0 && new_ticket.total_seats <= MAX_SEATS)
            break;
        printf("  Invalid! Enter 1-%d\n", MAX_SEATS);
    } while(1);
    
    do {
        printf("  Seats per row (e.g., 4 = A1-A4, B1-B4): ");
        scanf("%d", &new_ticket.seats_per_row);
        clear_input_buffer();
        if(new_ticket.seats_per_row > 0 && new_ticket.seats_per_row <= 10)
            break;
        printf("  Invalid! Enter 1-10\n");
    } while(1);
    
    initialize_seats(&new_ticket);
    new_ticket.active = 1;
    
    tickets[ticket_count++] = new_ticket;
    save_tickets();
    
    printf("\n  Ticket added successfully! Ticket ID: %d\n", new_ticket.id);
    printf("  Total Seats: %d\n", new_ticket.total_seats);
    printf("  Rows: %d, Seats per row: %d\n", new_ticket.total_rows, new_ticket.seats_per_row);
    wait_for_enter();
}

void edit_ticket() {
    int id;
    printf("  Enter Ticket ID to edit: ");
    scanf("%d", &id);
    clear_input_buffer();
    
    int idx = find_ticket_by_id(id);
    if(idx == -1) {
        printf("  Ticket not found!\n");
        wait_for_enter();
        return;
    }
    
    print_header("EDIT TICKET");
    printf("\n\n");
    printf("  Editing ticket: %s -> %s (ID: %d)\n", 
           tickets[idx].source, tickets[idx].destination, tickets[idx].id);
    printf("\n");
    printf("  1. Edit Source\n");
    printf("  2. Edit Destination\n");
    printf("  3. Edit Date\n");
    printf("  4. Edit Time\n");
    printf("  5. Edit Price\n");
    printf("  0. Cancel\n");
    printf("\n");
    
    int choice;
    printf("  Enter choice: ");
    scanf("%d", &choice);
    clear_input_buffer();
    
    switch(choice) {
        case 1:
            printf("  New Source: ");
            safe_input(tickets[idx].source, 50);
            save_tickets();
            printf("  Source updated!\n");
            break;
        case 2:
            printf("  New Destination: ");
            safe_input(tickets[idx].destination, 50);
            save_tickets();
            printf("  Destination updated!\n");
            break;
        case 3:
            printf("  New Date (DD-MM-YYYY): ");
            safe_input(tickets[idx].date, 15);
            save_tickets();
            printf("  Date updated!\n");
            break;
        case 4:
            printf("  New Time (HH:MM): ");
            safe_input(tickets[idx].time, 10);
            save_tickets();
            printf("  Time updated!\n");
            break;
        case 5:
            printf("  New Price: ");
            scanf("%f", &tickets[idx].price);
            clear_input_buffer();
            save_tickets();
            printf("  Price updated!\n");
            break;
        case 0:
            printf("  Cancelled.\n");
            break;
        default:
            printf("  Invalid choice!\n");
    }
    wait_for_enter();
}

void delete_ticket() {
    int id;
    printf("  Enter Ticket ID to delete: ");
    scanf("%d", &id);
    clear_input_buffer();
    
    int idx = find_ticket_by_id(id);
    if(idx == -1) {
        printf("  Ticket not found!\n");
        wait_for_enter();
        return;
    }
    
    char confirm;
    printf("  Are you sure you want to delete ticket %s -> %s? (y/n): ", 
           tickets[idx].source, tickets[idx].destination);
    scanf("%c", &confirm);
    clear_input_buffer();
    
    if(confirm == 'y' || confirm == 'Y') {
        for(int i = idx; i < ticket_count - 1; i++)
            tickets[i] = tickets[i + 1];
        ticket_count--;
        save_tickets();
        printf("  Ticket deleted successfully!\n");
    } else {
        printf("  Deletion cancelled.\n");
    }
    wait_for_enter();
}

void view_all_bookings() {
    print_header("ALL BOOKINGS");
    printf("\n\n");
    center_text("========================================");
    center_text("         BOOKING LIST                ");
    center_text("========================================");
    printf("\n\n");
    
    if(booking_count == 0) {
        printf("  No bookings found.\n");
        wait_for_enter();
        return;
    }
    
    printf("  %-6s %-8s %-8s %-20s %-12s %-8s %-10s %-12s %-10s\n",
           "ID", "Ticket", "User", "Customer", "Seats", "Qty", "Amount", "Date", "Status");
    print_separator('-', 110);
    
    for(int i = 0; i < booking_count; i++) {
        printf("  %-6d %-8d %-8d %-20s %-12s %-8d %-10.2f %-12s %-10s\n",
               bookings[i].id,
               bookings[i].ticket_id,
               bookings[i].user_id,
               bookings[i].customer_name,
               bookings[i].seat_numbers,
               bookings[i].seats_booked,
               bookings[i].total_amount,
               bookings[i].booking_date,
               bookings[i].status);
    }
    
    printf("\n  Total Bookings: %d\n", booking_count);
    wait_for_enter();
}

/* Customer Functions */
void customer_menu() {
    int choice;
    while(1) {
        print_header("CUSTOMER DASHBOARD");
        printf("\n\n");
        center_text("========================================");
        center_text("         CUSTOMER PANEL              ");
        center_text("========================================");
        printf("\n\n");
        center_text("[1] View Available Tickets");
        center_text("[2] Book Ticket");
        center_text("[3] View My Bookings");
        center_text("[4] Cancel Booking");
        center_text("[5] View Profile");
        center_text("[6] Update Profile");
        center_text("[7] Change Password");
        center_text("[0] Logout");
        printf("\n\n");
        center_text("------------------------------------------------------");
        printf("\n\n");
        
        printf("  Enter choice: ");
        scanf("%d", &choice);
        clear_input_buffer();
        
        switch(choice) {
            case 1: view_tickets(); break;
            case 2: book_ticket(); break;
            case 3: view_my_bookings(); break;
            case 4: cancel_booking(); break;
            case 5: view_profile(); break;
            case 6: update_profile(); break;
            case 7: change_password(); break;
            case 0:
                memset(&current_user, 0, sizeof(User));
                return;
            default:
                printf("  Invalid choice!\n");
                wait_for_enter();
        }
    }
}

void view_tickets() {
    print_header("AVAILABLE TICKETS");
    printf("\n\n");
    center_text("========================================");
    center_text("         AVAILABLE TICKETS            ");
    center_text("========================================");
    printf("\n\n");
    
    int found = 0;
    printf("  %-6s %-15s %-15s %-12s %-8s %-8s %-8s\n",
           "ID", "Source", "Destination", "Date", "Time", "Price", "Seats");
    print_separator('-', 85);
    
    for(int i = 0; i < ticket_count; i++) {
        if(tickets[i].active && tickets[i].available_seats > 0) {
            printf("  %-6d %-15s %-15s %-12s %-8s %-8.2f %-8d\n",
                   tickets[i].id,
                   tickets[i].source,
                   tickets[i].destination,
                   tickets[i].date,
                   tickets[i].time,
                   tickets[i].price,
                   tickets[i].available_seats);
            found = 1;
        }
    }
    
    if(!found) {
        printf("\n  No tickets available.\n");
    }
    printf("\n");
    wait_for_enter();
}

void book_ticket() {
    int ticket_id;
    char seat_input[MAX_STR];
    char selected_seats[MAX_SEATS][4];
    int seat_count;
    char payment_method[20];
    char transaction_id[30];
    
    view_tickets();
    
    printf("  Enter Ticket ID to book: ");
    scanf("%d", &ticket_id);
    clear_input_buffer();
    
    int idx = find_ticket_by_id(ticket_id);
    if(idx == -1) {
        printf("  Ticket not found!\n");
        wait_for_enter();
        return;
    }
    
    if(!tickets[idx].active || tickets[idx].available_seats <= 0) {
        printf("  Ticket not available!\n");
        wait_for_enter();
        return;
    }
    
    print_header("SEAT SELECTION");
    printf("\n\n");
    center_text("========================================");
    center_text("       SELECT YOUR SEATS              ");
    center_text("========================================");
    printf("\n\n");
    
    printf("  Ticket: %s -> %s\n", tickets[idx].source, tickets[idx].destination);
    printf("  Date: %s, Time: %s\n", tickets[idx].date, tickets[idx].time);
    printf("  Price: %.2f BDT per seat\n", tickets[idx].price);
    
    display_seating_layout(&tickets[idx]);
    
    printf("\n  Enter seat numbers (e.g., A1, B2, C3) - Case insensitive: ");
    safe_input(seat_input, MAX_STR);
    
    if(!parse_seat_numbers(seat_input, selected_seats, &seat_count)) {
        printf("  No valid seats entered!\n");
        wait_for_enter();
        return;
    }
    
    if(!validate_seat_selection(&tickets[idx], selected_seats, seat_count)) {
        wait_for_enter();
        return;
    }
    
    char confirm;
    printf("\n  Confirm booking for %d seat(s)? (y/n): ", seat_count);
    scanf("%c", &confirm);
    clear_input_buffer();
    
    if(confirm != 'y' && confirm != 'Y') {
        printf("  Booking cancelled.\n");
        wait_for_enter();
        return;
    }
    
    float total = tickets[idx].price * seat_count;
    if(!process_payment(total, payment_method, transaction_id)) {
        printf("  Payment cancelled.\n");
        wait_for_enter();
        return;
    }
    
    Booking new_booking;
    new_booking.id = get_next_booking_id();
    new_booking.ticket_id = ticket_id;
    new_booking.user_id = current_user.id;
    strcpy(new_booking.customer_name, current_user.fullname);
    
    strcpy(new_booking.seat_numbers, "");
    for(int i = 0; i < seat_count; i++) {
        if(i > 0) strcat(new_booking.seat_numbers, ",");
        strcat(new_booking.seat_numbers, selected_seats[i]);
    }
    
    new_booking.seats_booked = seat_count;
    new_booking.total_amount = total;
    strcpy(new_booking.booking_date, get_date());
    strcpy(new_booking.status, "confirmed");
    strcpy(new_booking.payment_method, payment_method);
    strcpy(new_booking.transaction_id, transaction_id);
    
    book_seats(&tickets[idx], selected_seats, seat_count);
    bookings[booking_count++] = new_booking;
    save_tickets();
    save_bookings();
    
    print_header("BOOKING SUCCESS");
    printf("\n\n");
    center_text("========================================");
    center_text("  BOOKING CONFIRMED!                 ");
    center_text("========================================");
    printf("\n");
    printf("  Booking ID    : %d\n", new_booking.id);
    printf("  Ticket        : %s -> %s\n", tickets[idx].source, tickets[idx].destination);
    printf("  Seats         : %s\n", new_booking.seat_numbers);
    printf("  Seats Booked  : %d\n", seat_count);
    printf("  Total Amount  : %.2f BDT\n", total);
    printf("  Payment Method: %s\n", payment_method);
    printf("  Transaction ID: %s\n", transaction_id);
    printf("  Status        : %s\n", new_booking.status);
    printf("\n");
    wait_for_enter();
}

void view_my_bookings() {
    print_header("MY BOOKINGS");
    printf("\n\n");
    center_text("========================================");
    center_text("         MY BOOKINGS                 ");
    center_text("========================================");
    printf("\n\n");
    
    int found = 0;
    
    printf("  %-6s %-15s %-15s %-12s %-12s %-8s %-10s %-10s\n",
           "ID", "Source", "Destination", "Date", "Seats", "Qty", "Amount", "Status");
    print_separator('-', 100);
    
    for(int i = 0; i < booking_count; i++) {
        if(bookings[i].user_id == current_user.id) {
            int tidx = find_ticket_by_id(bookings[i].ticket_id);
            if(tidx != -1) {
                printf("  %-6d %-15s %-15s %-12s %-12s %-8d %-10.2f %-10s\n",
                       bookings[i].id,
                       tickets[tidx].source,
                       tickets[tidx].destination,
                       bookings[i].booking_date,
                       bookings[i].seat_numbers,
                       bookings[i].seats_booked,
                       bookings[i].total_amount,
                       bookings[i].status);
                found = 1;
            }
        }
    }
    
    if(!found) {
        printf("\n  No bookings found.\n");
    }
    printf("\n");
    wait_for_enter();
}

void cancel_booking() {
    int booking_id;
    printf("  Enter Booking ID to cancel: ");
    scanf("%d", &booking_id);
    clear_input_buffer();
    
    int idx = find_booking_by_id(booking_id);
    if(idx == -1) {
        printf("  Booking not found!\n");
        wait_for_enter();
        return;
    }
    
    if(bookings[idx].user_id != current_user.id && strcmp(current_user.role, "admin") != 0) {
        printf("  You don't have permission to cancel this booking!\n");
        wait_for_enter();
        return;
    }
    
    if(strcmp(bookings[idx].status, "cancelled") == 0) {
        printf("  Booking is already cancelled!\n");
        wait_for_enter();
        return;
    }
    
    printf("\n  Booking Details:");
    printf("\n  Booking ID: %d", bookings[idx].id);
    printf("\n  Seats: %s", bookings[idx].seat_numbers);
    printf("\n  Amount: %.2f BDT", bookings[idx].total_amount);
    printf("\n  Payment: %s", bookings[idx].payment_method);
    printf("\n");
    
    char confirm;
    printf("\n  Are you sure you want to cancel? (y/n): ");
    scanf("%c", &confirm);
    clear_input_buffer();
    
    if(confirm == 'y' || confirm == 'Y') {
        char seats[MAX_SEATS][4];
        int count;
        parse_seat_numbers(bookings[idx].seat_numbers, seats, &count);
        
        int tidx = find_ticket_by_id(bookings[idx].ticket_id);
        if(tidx != -1) {
            release_seats(&tickets[tidx], seats, count);
            save_tickets();
        }
        
        strcpy(bookings[idx].status, "cancelled");
        save_bookings();
        printf("\n  Booking cancelled successfully!\n");
        printf("  Refund will be processed within 3-5 business days.\n");
    } else {
        printf("  Cancellation cancelled.\n");
    }
    wait_for_enter();
}

/* ======================================================================
 * MAIN FUNCTION
 * WRITTEN BY: Md. Salah Uddin 
 * ====================================================================== */

int main() {
    int choice;
    
    splash_screen();
    load_all_data();
    
    while(1) {
        clear_screen();
        print_header("WELCOME");
        printf("\n");
        center_text("========================================");
        center_text("  TICKET BOOKING SYSTEM  ");
        center_text("========================================");
        printf("\n\n");
        center_text("[1] Login");
        center_text("[2] Register");
        center_text("[0] Exit");
        printf("\n\n");
        center_text("========================================");
        printf("\n\n");
        
        printf("  Enter choice: ");
        scanf("%d", &choice);
        clear_input_buffer();
        
        switch(choice) {
            case 1:
                if(user_login()) {
                    if(strcmp(current_user.role, "admin") == 0)
                        admin_menu();
                    else
                        customer_menu();
                }
                break;
            case 2:
                user_register();
                break;
            case 0:
                save_all_data();
                clear_screen();
                print_header("GOODBYE");
                printf("\n\n");
                center_text("========================================");
                center_text("  THANK YOU FOR USING OUR SYSTEM  ");
                center_text("========================================");
                printf("\n\n");
                exit(0);
            default:
                printf("\n  Invalid choice!\n");
                wait_for_enter();
        }
    }
    return 0;
}