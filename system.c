/*
 * ======================================================================
 *                    TICKET BOOKING SYSTEM
 * ======================================================================
 * PROJECT: Online Ticket Booking System
 * TEAM MEMBERS:
 *   1. Md. Salah Uddin - UI, HELPERS, AND UTILITY FUNCTIONS
 *   2. Mohammad Bakhtiar - SEAT AND PRIORITY QUEUE FUNCTIONS
 *   3. Sumona Moni - TREE FUNCTIONS FOR SEAT AVAILABILITY
 *   4. Mohammad Rafi - ROUTE GRAPH FUNCTIONS AND MANAGEMENT
 *   5. Md Injamul Haque Titash - USER, ADMIN, CUSTOMER FUNCTIONS AND FILE HANDLING
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
 * SECTION 1: UI, HELPERS, AND UTILITY FUNCTIONS
 * DEVELOPER: Md. Salah Uddin
 * ====================================================================== */

#define MAX_USERS 50
#define MAX_TICKETS 100
#define MAX_BOOKINGS 200
#define MAX_STR 100
#define MAX_SEATS 100
#define MAX_LINE 500
#define MAX_ROUTES 50
#define MAX_WAYPOINTS 20
#define MAX_CITIES 100
#define MAX_HEAP 100
#define INF 999999.0

typedef struct {
    int seat_index;
    char seat_id[4];
    int priority;
    time_t booking_time;
} SeatNode;

typedef struct {
    SeatNode heap[MAX_HEAP];
    int size;
} PriorityQueue;

typedef struct Waypoint {
    char name[50];
    float distance_from_start;
    float time_from_start;
    float price_multiplier;
    int sequence;
} Waypoint;

typedef struct Route {
    int id;
    char route_name[100];
    char vehicle_type[20];
    Waypoint waypoints[MAX_WAYPOINTS];
    int waypoint_count;
    float base_price;
    int active;
    struct Route *next;
} Route;

typedef struct {
    Route *routes[MAX_ROUTES];
    int route_count;
} RouteGraph;

typedef struct SeatBSTNode {
    char seat_id[4];
    int seat_index;
    int is_available;
    float price;
    struct SeatBSTNode *left;
    struct SeatBSTNode *right;
} SeatBST;

typedef struct {
    SeatBST *root;
    int total_seats;
    int available_seats;
} SeatTree;

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
    int route_id;
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
    SeatTree *seat_tree;
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
    char source[50];
    char destination[50];
    float distance_traveled;
} Booking;

User users[MAX_USERS];
Ticket tickets[MAX_TICKETS];
Booking bookings[MAX_BOOKINGS];
RouteGraph route_graph;

int user_count = 0;
int ticket_count = 0;
int booking_count = 0;
int route_id_counter = 1;

User current_user;

/* Helper Functions - Defined First */
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

/* UI Functions */
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
        "Loading Route Data...",
        "Loading Ticket Data...",
        "Loading Booking Data...",
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

void convert_to_24h(char *time_12h, char *time_24h) {
    int hours, minutes;
    char ampm[3];

    sscanf(time_12h, "%d:%d %s", &hours, &minutes, ampm);

    if(str_icmp(ampm, "PM") == 0 && hours != 12) {
        hours += 12;
    }
    if(str_icmp(ampm, "AM") == 0 && hours == 12) {
        hours = 0;
    }

    sprintf(time_24h, "%02d:%02d", hours, minutes);
}

void convert_to_12h(char *time_24h, char *time_12h) {
    int hours, minutes;
    sscanf(time_24h, "%d:%d", &hours, &minutes);

    char ampm[3];
    if(hours >= 12) {
        strcpy(ampm, "PM");
        if(hours > 12) hours -= 12;
    } else {
        strcpy(ampm, "AM");
        if(hours == 0) hours = 12;
    }

    sprintf(time_12h, "%d:%02d %s", hours, minutes, ampm);
}

int parse_time_12h(char *time_str, int *hours, int *minutes) {
    char ampm[3];
    int h, m;

    if(sscanf(time_str, "%d:%d %s", &h, &m, ampm) == 3) {
        *hours = h;
        *minutes = m;
        return 1;
    }

    if(sscanf(time_str, "%d:%d%s", &h, &m, ampm) == 3) {
        *hours = h;
        *minutes = m;
        return 1;
    }

    return 0;
}

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

/* ======================================================================
 * SECTION 2: SEAT AND PRIORITY QUEUE FUNCTIONS
 * DEVELOPER: Mohammad Bakhtiar
 * ====================================================================== */

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

void init_pqueue(PriorityQueue *pq) {
    pq->size = 0;
}

void swap_seats(SeatNode *a, SeatNode *b) {
    SeatNode temp = *a;
    *a = *b;
    *b = temp;
}

void push_seat(PriorityQueue *pq, int seat_index, char *seat_id,
               int priority, time_t booking_time) {
    if(pq->size >= MAX_HEAP) return;

    SeatNode new_node;
    new_node.seat_index = seat_index;
    strcpy(new_node.seat_id, seat_id);
    new_node.priority = priority;
    new_node.booking_time = booking_time;

    pq->heap[pq->size] = new_node;
    int current = pq->size;
    pq->size++;

    while(current > 0) {
        int parent = (current - 1) / 2;

        if(pq->heap[current].priority < pq->heap[parent].priority) {
            swap_seats(&pq->heap[current], &pq->heap[parent]);
            current = parent;
        }
        else if(pq->heap[current].priority == pq->heap[parent].priority) {
            if(pq->heap[current].booking_time < pq->heap[parent].booking_time) {
                swap_seats(&pq->heap[current], &pq->heap[parent]);
                current = parent;
            } else break;
        }
        else break;
    }
}

SeatNode pop_seat(PriorityQueue *pq) {
    SeatNode result = pq->heap[0];
    pq->heap[0] = pq->heap[pq->size - 1];
    pq->size--;

    int current = 0;
    while(current < pq->size) {
        int left = 2 * current + 1;
        int right = 2 * current + 2;
        int smallest = current;

        if(left < pq->size) {
            if(pq->heap[left].priority < pq->heap[smallest].priority)
                smallest = left;
            else if(pq->heap[left].priority == pq->heap[smallest].priority) {
                if(pq->heap[left].booking_time < pq->heap[smallest].booking_time)
                    smallest = left;
            }
        }

        if(right < pq->size) {
            if(pq->heap[right].priority < pq->heap[smallest].priority)
                smallest = right;
            else if(pq->heap[right].priority == pq->heap[smallest].priority) {
                if(pq->heap[right].booking_time < pq->heap[smallest].booking_time)
                    smallest = right;
            }
        }

        if(smallest != current) {
            swap_seats(&pq->heap[current], &pq->heap[smallest]);
            current = smallest;
        } else break;
    }

    return result;
}

/* ======================================================================
 * SECTION 3: TREE FUNCTIONS FOR SEAT AVAILABILITY
 * DEVELOPER: Sumona Moni
 * ====================================================================== */

SeatBST* create_seat_node(char *seat_id, int index, float price) {
    SeatBST *node = (SeatBST*)malloc(sizeof(SeatBST));
    strcpy(node->seat_id, seat_id);
    node->seat_index = index;
    node->is_available = 1;
    node->price = price;
    node->left = node->right = NULL;
    return node;
}

SeatBST* insert_seat_bst(SeatBST *root, char *seat_id, int index, float price) {
    if(root == NULL) {
        return create_seat_node(seat_id, index, price);
    }

    if(strcmp(seat_id, root->seat_id) < 0) {
        root->left = insert_seat_bst(root->left, seat_id, index, price);
    }
    else if(strcmp(seat_id, root->seat_id) > 0) {
        root->right = insert_seat_bst(root->right, seat_id, index, price);
    }

    return root;
}

SeatBST* search_seat_bst(SeatBST *root, char *seat_id) {
    if(root == NULL || strcmp(root->seat_id, seat_id) == 0) {
        return root;
    }

    if(strcmp(seat_id, root->seat_id) < 0) {
        return search_seat_bst(root->left, seat_id);
    }
    return search_seat_bst(root->right, seat_id);
}

void update_seat_availability(SeatBST *root, char *seat_id, int available) {
    SeatBST *seat = search_seat_bst(root, seat_id);
    if(seat) {
        seat->is_available = available;
    }
}

void get_available_seats_bst(SeatBST *root, char result[MAX_SEATS][4], int *count) {
    if(root == NULL) return;

    get_available_seats_bst(root->left, result, count);

    if(root->is_available) {
        strcpy(result[*count], root->seat_id);
        (*count)++;
    }

    get_available_seats_bst(root->right, result, count);
}

void free_seat_tree(SeatBST *root) {
    if(root == NULL) return;
    free_seat_tree(root->left);
    free_seat_tree(root->right);
    free(root);
}

void init_seat_tree(Ticket *ticket) {
    if(ticket->seat_tree) {
        free_seat_tree(ticket->seat_tree->root);
        free(ticket->seat_tree);
    }

    ticket->seat_tree = (SeatTree*)malloc(sizeof(SeatTree));
    ticket->seat_tree->root = NULL;
    ticket->seat_tree->total_seats = ticket->total_seats;
    ticket->seat_tree->available_seats = ticket->available_seats;

    for(int i = 0; i < ticket->total_seats; i++) {
        float price = ticket->price;
        if(i < ticket->total_seats / 5) price *= 1.2;
        else if(i > ticket->total_seats * 0.8) price *= 0.9;

        ticket->seat_tree->root = insert_seat_bst(
            ticket->seat_tree->root, ticket->seats[i], i, price);
    }
}

void display_seat_status_tree(Ticket *ticket) {
    printf("\n  Real-time Seat Availability:\n");
    printf("  Total Seats: %d\n", ticket->seat_tree->total_seats);
    printf("  Available: %d\n", ticket->seat_tree->available_seats);
    printf("  Booked: %d\n", ticket->total_seats - ticket->seat_tree->available_seats);

    printf("\n  Seat Layout (O=Available, X=Booked):\n");
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
                SeatBST *seat = search_seat_bst(ticket->seat_tree->root, ticket->seats[idx]);
                if(seat && seat->is_available)
                    printf(" %s(O) ", ticket->seats[idx]);
                else
                    printf(" %s(X) ", ticket->seats[idx]);
            } else {
                printf("      ");
            }
        }
        printf("\n");
    }
}

/* ======================================================================
 * SECTION 4: ROUTE GRAPH FUNCTIONS AND MANAGEMENT
 * DEVELOPER: Mohammad Rafi
 * ====================================================================== */

void init_route_graph() {
    route_graph.route_count = 0;
    for(int i = 0; i < MAX_ROUTES; i++) {
        route_graph.routes[i] = NULL;
    }
}

int add_route(char *route_name, char *vehicle_type, float base_price) {
    if(route_graph.route_count >= MAX_ROUTES) return -1;

    Route *new_route = (Route*)malloc(sizeof(Route));
    new_route->id = route_id_counter++;
    strcpy(new_route->route_name, route_name);
    strcpy(new_route->vehicle_type, vehicle_type);
    new_route->base_price = base_price;
    new_route->waypoint_count = 0;
    new_route->active = 1;
    new_route->next = NULL;

    route_graph.routes[route_graph.route_count++] = new_route;
    return new_route->id;
}

int add_waypoint(int route_id, char *name, float distance, float time) {
    Route *route = NULL;
    for(int i = 0; i < route_graph.route_count; i++) {
        if(route_graph.routes[i]->id == route_id) {
            route = route_graph.routes[i];
            break;
        }
    }
    if(!route || route->waypoint_count >= MAX_WAYPOINTS) return 0;

    Waypoint *wp = &route->waypoints[route->waypoint_count];
    strcpy(wp->name, name);
    wp->distance_from_start = distance;
    wp->time_from_start = time;

    if(route->waypoint_count == 0) {
        wp->price_multiplier = 1.0;
    } else {
        float avg_distance = distance / route->waypoint_count;
        if(avg_distance > 50) {
            wp->price_multiplier = 0.95;
        } else if(avg_distance < 20) {
            wp->price_multiplier = 1.05;
        } else {
            wp->price_multiplier = 1.0;
        }

        char *premium_points[] = {"DHAKA", "CHITTAGONG", "SYLHET", "RAJSHAHI", "KHULNA"};
        for(int i = 0; i < 5; i++) {
            if(str_icmp(name, premium_points[i]) == 0) {
                wp->price_multiplier = 1.1;
                break;
            }
        }
    }

    wp->sequence = route->waypoint_count;
    route->waypoint_count++;

    return 1;
}

void display_route_details(Route *route) {
    if(!route || !route->active) {
        printf("  Route not found or inactive!\n");
        return;
    }

    printf("\n  ========================================\n");
    printf("  Route: %s\n", route->route_name);
    printf("  Vehicle: %s\n", route->vehicle_type);
    printf("  Base Price: %.2f BDT/KM\n", route->base_price);
    printf("  ========================================\n");
    printf("  %-3s %-20s %-12s %-10s %-12s\n", "#", "Point", "Distance(KM)", "Time(Hrs)", "Price/KM");
    printf("  ----------------------------------------\n");

    for(int i = 0; i < route->waypoint_count; i++) {
        Waypoint *wp = &route->waypoints[i];
        printf("  %-3d %-20s %-12.2f %-10.2f %-12.2f\n",
               i, wp->name, wp->distance_from_start, wp->time_from_start,
               route->base_price * wp->price_multiplier);
    }
    printf("  ========================================\n");
}

void list_all_routes() {
    print_header("ALL ROUTES");
    printf("\n\n");
    center_text("========================================");
    center_text("         ROUTE LIST                   ");
    center_text("========================================");
    printf("\n\n");

    int found = 0;
    for(int i = 0; i < route_graph.route_count; i++) {
        Route *r = route_graph.routes[i];
        if(r && r->active) {
            printf("  Route ID: %d\n", r->id);
            printf("  Name: %s\n", r->route_name);
            printf("  Vehicle: %s\n", r->vehicle_type);
            printf("  Waypoints: %d\n", r->waypoint_count);
            printf("  ----------------------------------------\n");
            found = 1;
        }
    }

    if(!found) {
        printf("  No routes available.\n");
    }
    wait_for_enter();
}

float calculate_distance_between_waypoints(Route *route, int from_idx, int to_idx) {
    if(!route || from_idx < 0 || to_idx < 0 ||
       from_idx >= route->waypoint_count || to_idx >= route->waypoint_count) {
        return 0;
    }

    if(from_idx == to_idx) return 0;

    int start = from_idx < to_idx ? from_idx : to_idx;
    int end = from_idx < to_idx ? to_idx : from_idx;

    float total_distance = route->waypoints[end].distance_from_start -
                          route->waypoints[start].distance_from_start;
    return total_distance;
}

float calculate_time_between_waypoints(Route *route, int from_idx, int to_idx) {
    if(!route || from_idx < 0 || to_idx < 0 ||
       from_idx >= route->waypoint_count || to_idx >= route->waypoint_count) {
        return 0;
    }

    if(from_idx == to_idx) return 0;

    int start = from_idx < to_idx ? from_idx : to_idx;
    int end = from_idx < to_idx ? to_idx : from_idx;

    float total_time = route->waypoints[end].time_from_start -
                       route->waypoints[start].time_from_start;
    return total_time;
}

float calculate_ticket_price(Route *route, int from_idx, int to_idx) {
    if(!route || from_idx < 0 || to_idx < 0 ||
       from_idx >= route->waypoint_count || to_idx >= route->waypoint_count) {
        return 0;
    }

    float distance = calculate_distance_between_waypoints(route, from_idx, to_idx);

    float avg_multiplier = 0;
    int count = 0;
    int start = from_idx < to_idx ? from_idx : to_idx;
    int end = from_idx < to_idx ? to_idx : from_idx;

    for(int i = start; i <= end; i++) {
        avg_multiplier += route->waypoints[i].price_multiplier;
        count++;
    }
    avg_multiplier /= count;

    return distance * route->base_price * avg_multiplier;
}

int find_waypoint_index(Route *route, char *name) {
    if(!route) return -1;

    for(int i = 0; i < route->waypoint_count; i++) {
        if(str_icmp(route->waypoints[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

char* get_departure_time_at_waypoint(Route *route, int waypoint_idx, char *base_time) {
    static char time_str[20];
    if(!route || waypoint_idx < 0 || waypoint_idx >= route->waypoint_count) {
        strcpy(time_str, "N/A");
        return time_str;
    }

    int hours, minutes;
    char ampm[3];

    sscanf(base_time, "%d:%d %s", &hours, &minutes, ampm);

    if(str_icmp(ampm, "PM") == 0 && hours != 12) {
        hours += 12;
    }
    if(str_icmp(ampm, "AM") == 0 && hours == 12) {
        hours = 0;
    }

    float total_hours = route->waypoints[waypoint_idx].time_from_start;
    int add_hours = (int)total_hours;
    int add_minutes = (int)((total_hours - add_hours) * 60);

    hours = (hours + add_hours + (minutes + add_minutes) / 60) % 24;
    minutes = (minutes + add_minutes) % 60;

    if(hours >= 12) {
        strcpy(ampm, "PM");
        if(hours > 12) hours -= 12;
    } else {
        strcpy(ampm, "AM");
        if(hours == 0) hours = 12;
    }

    sprintf(time_str, "%d:%02d %s", hours, minutes, ampm);
    return time_str;
}

int get_route_by_id(int route_id) {
    for(int i = 0; i < route_graph.route_count; i++) {
        if(route_graph.routes[i] && route_graph.routes[i]->id == route_id) {
            return i;
        }
    }
    return -1;
}

int get_route_id_by_name(char *route_name) {
    for(int i = 0; i < route_graph.route_count; i++) {
        if(route_graph.routes[i] && route_graph.routes[i]->active) {
            if(str_icmp(route_graph.routes[i]->route_name, route_name) == 0) {
                return route_graph.routes[i]->id;
            }
        }
    }
    return -1;
}

void view_route_details() {
    int route_id;
    printf("  Enter Route ID: ");
    scanf("%d", &route_id);
    clear_input_buffer();

    Route *route = NULL;
    for(int i = 0; i < route_graph.route_count; i++) {
        if(route_graph.routes[i]->id == route_id) {
            route = route_graph.routes[i];
            break;
        }
    }

    if(!route || !route->active) {
        printf("  Route not found!\n");
        wait_for_enter();
        return;
    }

    display_route_details(route);
    wait_for_enter();
}

void create_new_route() {
    char route_name[100], vehicle_type[20];
    float base_price;
    int waypoint_count;

    print_header("CREATE NEW ROUTE");
    printf("\n\n");
    center_text("========================================");
    center_text("         CREATE ROUTE                 ");
    center_text("========================================");
    printf("\n\n");

    printf("  Route Name: ");
    safe_input(route_name, 100);
    printf("  Vehicle Type (Bus/Train/Flight): ");
    safe_input(vehicle_type, 20);
    printf("  Base Price (BDT per KM): ");
    scanf("%f", &base_price);
    clear_input_buffer();

    int route_id = add_route(route_name, vehicle_type, base_price);
    if(route_id == -1) {
        printf("  Cannot add more routes!\n");
        wait_for_enter();
        return;
    }

    printf("\n  Now add waypoints for this route.\n");
    printf("  Enter number of waypoints (minimum 2): ");
    scanf("%d", &waypoint_count);
    clear_input_buffer();

    if(waypoint_count < 2) {
        printf("  Route needs at least 2 points!\n");
        for(int i = 0; i < route_graph.route_count; i++) {
            if(route_graph.routes[i]->id == route_id) {
                free(route_graph.routes[i]);
                route_graph.routes[i] = NULL;
                break;
            }
        }
        wait_for_enter();
        return;
    }

    float total_distance = 0, total_time = 0;

    for(int i = 0; i < waypoint_count; i++) {
        char name[50];
        float distance, time;

        printf("\n  Waypoint %d:\n", i + 1);
        printf("  Name: ");
        safe_input(name, 50);

        if(i == 0) {
            distance = 0;
            time = 0;
        } else {
            float segment_distance;
            printf("  Distance from previous point (KM): ");
            scanf("%f", &segment_distance);
            clear_input_buffer();

            float segment_time;
            printf("  Time from previous point (hours): ");
            scanf("%f", &segment_time);
            clear_input_buffer();

            distance = total_distance + segment_distance;
            time = total_time + segment_time;
        }

        add_waypoint(route_id, name, distance, time);
        total_distance = distance;
        total_time = time;
    }

    printf("\n  Route created successfully! Route ID: %d\n", route_id);
    Route *route = NULL;
    for(int i = 0; i < route_graph.route_count; i++) {
        if(route_graph.routes[i]->id == route_id) {
            route = route_graph.routes[i];
            break;
        }
    }
    display_route_details(route);
    wait_for_enter();
}

void add_waypoint_to_route() {
    int route_id;
    printf("  Enter Route ID: ");
    scanf("%d", &route_id);
    clear_input_buffer();

    Route *route = NULL;
    for(int i = 0; i < route_graph.route_count; i++) {
        if(route_graph.routes[i]->id == route_id) {
            route = route_graph.routes[i];
            break;
        }
    }

    if(!route || !route->active) {
        printf("  Route not found!\n");
        wait_for_enter();
        return;
    }

    display_route_details(route);

    char name[50];
    float distance, time;
    int insert_after;

    printf("\n  Insert after which waypoint number? (0 for beginning): ");
    scanf("%d", &insert_after);
    clear_input_buffer();

    printf("  New Waypoint Name: ");
    safe_input(name, 50);
    printf("  Distance from start (KM): ");
    scanf("%f", &distance);
    clear_input_buffer();
    printf("  Time from start (hours): ");
    scanf("%f", &time);
    clear_input_buffer();

    if(insert_after < 0 || insert_after > route->waypoint_count) {
        printf("  Invalid position!\n");
        wait_for_enter();
        return;
    }

    for(int i = route->waypoint_count; i > insert_after; i--) {
        route->waypoints[i] = route->waypoints[i - 1];
    }

    Waypoint *wp = &route->waypoints[insert_after];
    strcpy(wp->name, name);
    wp->distance_from_start = distance;
    wp->time_from_start = time;

    if(insert_after == 0) {
        wp->price_multiplier = 1.0;
    } else {
        float avg_distance = distance / route->waypoint_count;
        if(avg_distance > 50) {
            wp->price_multiplier = 0.95;
        } else if(avg_distance < 20) {
            wp->price_multiplier = 1.05;
        } else {
            wp->price_multiplier = 1.0;
        }

        char *premium_points[] = {"DHAKA", "CHITTAGONG", "SYLHET", "RAJSHAHI", "KHULNA"};
        for(int i = 0; i < 5; i++) {
            if(str_icmp(name, premium_points[i]) == 0) {
                wp->price_multiplier = 1.1;
                break;
            }
        }
    }

    wp->sequence = insert_after;
    route->waypoint_count++;

    for(int i = insert_after + 1; i < route->waypoint_count; i++) {
        route->waypoints[i].sequence = i;
    }

    printf("  Waypoint added successfully!\n");
    display_route_details(route);
    wait_for_enter();
}

void edit_waypoint() {
    int route_id, waypoint_idx;
    printf("  Enter Route ID: ");
    scanf("%d", &route_id);
    clear_input_buffer();

    Route *route = NULL;
    for(int i = 0; i < route_graph.route_count; i++) {
        if(route_graph.routes[i]->id == route_id) {
            route = route_graph.routes[i];
            break;
        }
    }

    if(!route || !route->active) {
        printf("  Route not found!\n");
        wait_for_enter();
        return;
    }

    display_route_details(route);

    printf("  Enter waypoint number to edit (0 to %d): ", route->waypoint_count - 1);
    scanf("%d", &waypoint_idx);
    clear_input_buffer();

    if(waypoint_idx < 0 || waypoint_idx >= route->waypoint_count) {
        printf("  Invalid waypoint!\n");
        wait_for_enter();
        return;
    }

    Waypoint *wp = &route->waypoints[waypoint_idx];
    printf("\n  Editing: %s\n", wp->name);
    printf("  New Name (current: %s): ", wp->name);
    char new_name[50];
    safe_input(new_name, 50);
    if(strlen(new_name) > 1) strcpy(wp->name, new_name);

    printf("  New Distance (current: %.2f): ", wp->distance_from_start);
    float new_dist;
    scanf("%f", &new_dist);
    clear_input_buffer();
    if(new_dist >= 0) wp->distance_from_start = new_dist;

    printf("  New Time (current: %.2f): ", wp->time_from_start);
    float new_time;
    scanf("%f", &new_time);
    clear_input_buffer();
    if(new_time >= 0) wp->time_from_start = new_time;

    printf("  Waypoint updated!\n");
    display_route_details(route);
    wait_for_enter();
}

void remove_waypoint() {
    int route_id, waypoint_idx;
    printf("  Enter Route ID: ");
    scanf("%d", &route_id);
    clear_input_buffer();

    Route *route = NULL;
    for(int i = 0; i < route_graph.route_count; i++) {
        if(route_graph.routes[i]->id == route_id) {
            route = route_graph.routes[i];
            break;
        }
    }

    if(!route || !route->active) {
        printf("  Route not found!\n");
        wait_for_enter();
        return;
    }

    if(route->waypoint_count <= 2) {
        printf("  Route must have at least 2 waypoints!\n");
        wait_for_enter();
        return;
    }

    display_route_details(route);

    printf("  Enter waypoint number to remove (0 to %d): ", route->waypoint_count - 1);
    scanf("%d", &waypoint_idx);
    clear_input_buffer();

    if(waypoint_idx < 0 || waypoint_idx >= route->waypoint_count) {
        printf("  Invalid waypoint!\n");
        wait_for_enter();
        return;
    }

    char confirm;
    printf("  Remove waypoint '%s'? (y/n): ", route->waypoints[waypoint_idx].name);
    scanf("%c", &confirm);
    clear_input_buffer();

    if(confirm != 'y' && confirm != 'Y') {
        printf("  Cancelled.\n");
        wait_for_enter();
        return;
    }

    for(int i = waypoint_idx; i < route->waypoint_count - 1; i++) {
        route->waypoints[i] = route->waypoints[i + 1];
    }
    route->waypoint_count--;

    for(int i = 0; i < route->waypoint_count; i++) {
        route->waypoints[i].sequence = i;
    }

    printf("  Waypoint removed!\n");
    display_route_details(route);
    wait_for_enter();
}

void delete_route() {
    int route_id;
    printf("  Enter Route ID to delete: ");
    scanf("%d", &route_id);
    clear_input_buffer();

    Route *route = NULL;
    int route_idx = -1;
    for(int i = 0; i < route_graph.route_count; i++) {
        if(route_graph.routes[i]->id == route_id) {
            route = route_graph.routes[i];
            route_idx = i;
            break;
        }
    }

    if(!route) {
        printf("  Route not found!\n");
        wait_for_enter();
        return;
    }

    char confirm;
    printf("  Delete route '%s'? (y/n): ", route->route_name);
    scanf("%c", &confirm);
    clear_input_buffer();

    if(confirm != 'y' && confirm != 'Y') {
        printf("  Cancelled.\n");
        wait_for_enter();
        return;
    }

    free(route);
    route_graph.routes[route_idx] = NULL;
    for(int i = route_idx; i < route_graph.route_count - 1; i++) {
        route_graph.routes[i] = route_graph.routes[i + 1];
    }
    route_graph.route_count--;

    printf("  Route deleted!\n");
    wait_for_enter();
}

void admin_route_management() {
    int choice;
    while(1) {
        print_header("ROUTE MANAGEMENT");
        printf("\n\n");
        center_text("========================================");
        center_text("         ROUTE MANAGEMENT              ");
        center_text("========================================");
        printf("\n\n");
        center_text("[1] Create New Route");
        center_text("[2] Add Waypoint to Route");
        center_text("[3] Edit Waypoint");
        center_text("[4] Remove Waypoint");
        center_text("[5] Delete Route");
        center_text("[6] View All Routes");
        center_text("[7] View Route Details");
        center_text("[0] Back");
        printf("\n\n");
        center_text("------------------------------------------------------");
        printf("\n\n");

        printf("  Enter choice: ");
        scanf("%d", &choice);
        clear_input_buffer();

        switch(choice) {
            case 1: create_new_route(); break;
            case 2: add_waypoint_to_route(); break;
            case 3: edit_waypoint(); break;
            case 4: remove_waypoint(); break;
            case 5: delete_route(); break;
            case 6: list_all_routes(); break;
            case 7: view_route_details(); break;
            case 0: return;
            default:
                printf("  Invalid choice!\n");
                wait_for_enter();
        }
    }
}

/* ======================================================================
 * SECTION 5: USER, ADMIN, CUSTOMER FUNCTIONS AND FILE HANDLING
 * DEVELOPER: Md Injamul Haque Titash
 * ====================================================================== */

void save_users(void);
void save_routes(void);
void save_tickets(void);
void save_bookings(void);

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

void load_routes() {
    FILE *file = fopen("routes.txt", "r");
    if(!file) {
        init_route_graph();
        int route_id = add_route("Dhaka-Naogaon Route", "Bus", 2.5);
        add_waypoint(route_id, "Dhaka", 0, 0);
        add_waypoint(route_id, "Abdullahpur", 15, 0.5);
        add_waypoint(route_id, "Gazipur", 35, 1.0);
        add_waypoint(route_id, "Tangail", 80, 2.0);
        add_waypoint(route_id, "Bogura", 180, 4.0);
        add_waypoint(route_id, "Naogaon", 230, 5.0);
        add_waypoint(route_id, "Mohadebpur", 260, 5.5);
        add_waypoint(route_id, "Patnitala", 280, 6.0);
        save_routes();
        return;
    }

    init_route_graph();
    char line[MAX_LINE];

    while(fgets(line, sizeof(line), file) && route_graph.route_count < MAX_ROUTES) {
        Route *route = (Route*)malloc(sizeof(Route));
        if(sscanf(line, "%d,%99[^,],%19[^,],%f,%d,%d",
                  &route->id, route->route_name, route->vehicle_type,
                  &route->base_price,
                  &route->waypoint_count, &route->active) == 6) {

            for(int i = 0; i < route->waypoint_count && i < MAX_WAYPOINTS; i++) {
                if(fgets(line, sizeof(line), file)) {
                    sscanf(line, "%49[^,],%f,%f,%f,%d",
                           route->waypoints[i].name,
                           &route->waypoints[i].distance_from_start,
                           &route->waypoints[i].time_from_start,
                           &route->waypoints[i].price_multiplier,
                           &route->waypoints[i].sequence);
                }
            }

            route_graph.routes[route_graph.route_count++] = route;
            if(route->id >= route_id_counter) route_id_counter = route->id + 1;
        }
    }
    fclose(file);
}

void save_routes() {
    FILE *file = fopen("routes.txt", "w");
    if(!file) return;

    for(int i = 0; i < route_graph.route_count; i++) {
        Route *r = route_graph.routes[i];
        if(!r) continue;

        fprintf(file, "%d,%s,%s,%.2f,%d,%d\n",
                r->id, r->route_name, r->vehicle_type,
                r->base_price,
                r->waypoint_count, r->active);

        for(int j = 0; j < r->waypoint_count; j++) {
            Waypoint *wp = &r->waypoints[j];
            fprintf(file, "%s,%.2f,%.2f,%.2f,%d\n",
                    wp->name, wp->distance_from_start,
                    wp->time_from_start, wp->price_multiplier,
                    wp->sequence);
        }
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

        if(sscanf(line, "%d,%d,%49[^,],%49[^,],%14[^,],%9[^,],%f,%d,%d,%d,%d,%d",
                  &t->id, &t->route_id, t->source, t->destination,
                  t->date, t->time, &t->price, &t->total_seats,
                  &t->seats_per_row, &t->total_rows, &t->available_seats,
                  &t->active) == 12) {

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

            t->seat_tree = NULL;
            init_seat_tree(t);

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

        fprintf(file, "%d,%d,%s,%s,%s,%s,%.2f,%d,%d,%d,%d,%d\n",
                t->id, t->route_id, t->source, t->destination,
                t->date, t->time, t->price, t->total_seats,
                t->seats_per_row, t->total_rows, t->available_seats,
                t->active);

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
    while(fscanf(file, "%d,%d,%d,%49[^,],%49[^,],%d,%f,%14[^,],%14[^,],%19[^,],%29[^,],%49[^,],%49[^,],%f\n",
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
                 bookings[booking_count].transaction_id,
                 bookings[booking_count].source,
                 bookings[booking_count].destination,
                 &bookings[booking_count].distance_traveled) == 14) {
        booking_count++;
    }
    fclose(file);
}

void save_bookings() {
    FILE *file = fopen("bookings.txt", "w");
    if(!file) return;

    for(int i = 0; i < booking_count; i++) {
        fprintf(file, "%d,%d,%d,%s,%s,%d,%.2f,%s,%s,%s,%s,%s,%s,%.2f\n",
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
                bookings[i].transaction_id,
                bookings[i].source,
                bookings[i].destination,
                bookings[i].distance_traveled);
    }
    fclose(file);
}

void load_all_data() {
    load_users();
    load_routes();
    load_tickets();
    load_bookings();
}

void save_all_data() {
    save_users();
    save_routes();
    save_tickets();
    save_bookings();
}

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

/* User Management Functions */
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

/* Ticket Management Functions */
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

    printf("  %-6s %-15s %-15s %-12s %-10s %-8s %-8s %-8s\n",
           "ID", "Route ID", "Source", "Destination", "Date", "Time", "Price", "Seats");
    print_separator('-', 95);

    for(int i = 0; i < ticket_count; i++) {
        printf("  %-6d %-8d %-15s %-15s %-12s %-10s %-8.2f %-8d\n",
               tickets[i].id,
               tickets[i].route_id,
               tickets[i].source,
               tickets[i].destination,
               tickets[i].date,
               tickets[i].time,
               tickets[i].price,
               tickets[i].available_seats);
    }

    printf("\n  Total Tickets: %d\n", ticket_count);
    wait_for_enter();
}

void add_ticket_with_route() {
    Ticket new_ticket;
    int route_id;
    int src_num, dest_num;
    char date[15], time_str[20];

    print_header("ADD TICKET WITH ROUTE");
    printf("\n\n");
    center_text("========================================");
    center_text("     ADD TICKET WITH ROUTE            ");
    center_text("========================================");
    printf("\n\n");

    printf("  Available Routes:\n");
    for(int i = 0; i < route_graph.route_count; i++) {
        Route *r = route_graph.routes[i];
        if(r && r->active) {
            printf("  [%d] %s (%s)\n",
                   r->id, r->route_name, r->vehicle_type);
        }
    }

    printf("\n  Select Route ID: ");
    scanf("%d", &route_id);
    clear_input_buffer();

    int route_idx = get_route_by_id(route_id);
    if(route_idx == -1) {
        printf("  Invalid route!\n");
        wait_for_enter();
        return;
    }

    Route *route = route_graph.routes[route_idx];

    printf("\n  Route Waypoints (select by number):\n");
    for(int i = 0; i < route->waypoint_count; i++) {
        printf("  [%d] %s (%.2f KM from start)\n",
               i, route->waypoints[i].name,
               route->waypoints[i].distance_from_start);
    }

    do {
        printf("\n  Enter Source Waypoint Number (0 to %d): ", route->waypoint_count - 1);
        scanf("%d", &src_num);
        clear_input_buffer();
        if(src_num >= 0 && src_num < route->waypoint_count) break;
        printf("  Invalid number! Please enter 0 to %d\n", route->waypoint_count - 1);
    } while(1);

    do {
        printf("  Enter Destination Waypoint Number (0 to %d): ", route->waypoint_count - 1);
        scanf("%d", &dest_num);
        clear_input_buffer();
        if(dest_num >= 0 && dest_num < route->waypoint_count) break;
        printf("  Invalid number! Please enter 0 to %d\n", route->waypoint_count - 1);
    } while(1);

    if(src_num == dest_num) {
        printf("  Source and destination cannot be same!\n");
        wait_for_enter();
        return;
    }

    printf("\n  Enter Travel Date (DD-MM-YYYY): ");
    safe_input(date, 15);

    do {
        printf("  Enter Departure Time (HH:MM AM/PM, e.g., 08:00 AM): ");
        safe_input(time_str, 20);

        int h, m;
        if(parse_time_12h(time_str, &h, &m)) {
            break;
        }
        printf("  Invalid time format! Please use HH:MM AM/PM\n");
    } while(1);

    new_ticket.id = get_next_ticket_id();
    new_ticket.route_id = route_id;
    strcpy(new_ticket.source, route->waypoints[src_num].name);
    strcpy(new_ticket.destination, route->waypoints[dest_num].name);
    strcpy(new_ticket.date, date);
    strcpy(new_ticket.time, time_str);

    float distance = calculate_distance_between_waypoints(route, src_num, dest_num);
    new_ticket.price = calculate_ticket_price(route, src_num, dest_num);

    do {
        printf("\n  Total Seats: ");
        scanf("%d", &new_ticket.total_seats);
        clear_input_buffer();
        if(new_ticket.total_seats > 0 && new_ticket.total_seats <= MAX_SEATS)
            break;
        printf("  Invalid! Enter 1-%d\n", MAX_SEATS);
    } while(1);

    do {
        printf("  Seats per row (e.g., 4): ");
        scanf("%d", &new_ticket.seats_per_row);
        clear_input_buffer();
        if(new_ticket.seats_per_row > 0 && new_ticket.seats_per_row <= 10)
            break;
        printf("  Invalid! Enter 1-10\n");
    } while(1);

    char rows[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int seat_count = 0;
    new_ticket.total_rows = (new_ticket.total_seats + new_ticket.seats_per_row - 1) /
                            new_ticket.seats_per_row;
    if(new_ticket.total_rows > 26) new_ticket.total_rows = 26;

    for(int r = 0; r < new_ticket.total_rows && seat_count < new_ticket.total_seats; r++) {
        for(int c = 1; c <= new_ticket.seats_per_row && seat_count < new_ticket.total_seats; c++) {
            sprintf(new_ticket.seats[seat_count], "%c%d", rows[r], c);
            new_ticket.seat_status[seat_count] = 'O';
            seat_count++;
        }
    }
    new_ticket.total_seats = seat_count;
    new_ticket.available_seats = seat_count;
    new_ticket.active = 1;

    new_ticket.seat_tree = NULL;
    init_seat_tree(&new_ticket);

    tickets[ticket_count++] = new_ticket;
    save_tickets();

    printf("\n  ========================================\n");
    printf("  TICKET ADDED SUCCESSFULLY!\n");
    printf("  ========================================\n");
    printf("  Ticket ID: %d\n", new_ticket.id);
    printf("  Route: %s\n", route->route_name);
    printf("  From: %s -> To: %s\n",
           route->waypoints[src_num].name,
           route->waypoints[dest_num].name);
    printf("  Distance: %.2f KM\n", distance);
    printf("  Price: %.2f BDT\n", new_ticket.price);
    printf("  Date: %s\n", date);
    printf("  Departure Time: %s\n", time_str);
    printf("  Total Seats: %d\n", new_ticket.total_seats);
    printf("  Status: %s\n", new_ticket.active ? "Active" : "Inactive");
    printf("  ========================================\n");

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
    printf("  1. Edit Date\n");
    printf("  2. Edit Time\n");
    printf("  3. Edit Price\n");
    printf("  4. Toggle Active Status\n");
    printf("  0. Cancel\n");
    printf("\n");

    int choice;
    printf("  Enter choice: ");
    scanf("%d", &choice);
    clear_input_buffer();

    switch(choice) {
        case 1:
            printf("  New Date (DD-MM-YYYY): ");
            safe_input(tickets[idx].date, 15);
            save_tickets();
            printf("  Date updated!\n");
            break;
        case 2:
            do {
                printf("  New Time (HH:MM AM/PM): ");
                safe_input(tickets[idx].time, 10);
                int h, m;
                if(parse_time_12h(tickets[idx].time, &h, &m)) {
                    break;
                }
                printf("  Invalid time format!\n");
            } while(1);
            save_tickets();
            printf("  Time updated!\n");
            break;
        case 3:
            printf("  New Price: ");
            scanf("%f", &tickets[idx].price);
            clear_input_buffer();
            save_tickets();
            printf("  Price updated!\n");
            break;
        case 4:
            tickets[idx].active = !tickets[idx].active;
            save_tickets();
            printf("  Ticket status toggled to: %s\n", tickets[idx].active ? "Active" : "Inactive");
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
        if(tickets[idx].seat_tree) {
            free_seat_tree(tickets[idx].seat_tree->root);
            free(tickets[idx].seat_tree);
        }
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
        center_text("[2] Add Ticket with Route");
        center_text("[3] Edit Ticket");
        center_text("[4] Delete Ticket");
        center_text("[5] View Route Details");
        center_text("[0] Back");
        printf("\n\n");
        center_text("------------------------------------------------------");
        printf("\n\n");

        printf("  Enter choice: ");
        scanf("%d", &choice);
        clear_input_buffer();

        switch(choice) {
            case 1: list_tickets(); break;
            case 2: add_ticket_with_route(); break;
            case 3: edit_ticket(); break;
            case 4: delete_ticket(); break;
            case 5: view_route_details(); break;
            case 0: return;
            default:
                printf("  Invalid choice!\n");
                wait_for_enter();
        }
    }
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

    printf("  %-6s %-8s %-20s %-20s %-12s %-8s %-10s\n",
           "ID", "Ticket", "From", "To", "Seats", "Qty", "Amount");
    print_separator('-', 90);

    for(int i = 0; i < booking_count; i++) {
        printf("  %-6d %-8d %-20s %-20s %-12s %-8d %-10.2f\n",
               bookings[i].id,
               bookings[i].ticket_id,
               bookings[i].source,
               bookings[i].destination,
               bookings[i].seat_numbers,
               bookings[i].seats_booked,
               bookings[i].total_amount);
    }

    printf("\n  Total Bookings: %d\n", booking_count);
    wait_for_enter();
}

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
        center_text("[3] Route Management");
        center_text("[4] View All Bookings");
        center_text("[5] View Profile");
        center_text("[6] Change Password");
        center_text("[7] Update Profile");
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
            case 3: admin_route_management(); break;
            case 4: view_all_bookings(); break;
            case 5: view_profile(); break;
            case 6: change_password(); break;
            case 7: update_profile(); break;
            case 0:
                memset(&current_user, 0, sizeof(User));
                return;
            default:
                printf("  Invalid choice!\n");
                wait_for_enter();
        }
    }
}

/* Customer Functions */
void view_available_tickets_with_routes() {
    print_header("AVAILABLE TICKETS WITH ROUTES");
    printf("\n\n");
    center_text("========================================");
    center_text("   AVAILABLE TICKETS & ROUTES        ");
    center_text("========================================");
    printf("\n\n");

    int found = 0;

    for(int i = 0; i < ticket_count; i++) {
        if(!tickets[i].active || tickets[i].available_seats <= 0) continue;

        int route_idx = get_route_by_id(tickets[i].route_id);
        if(route_idx == -1) continue;

        Route *route = route_graph.routes[route_idx];

        printf("  ========================================\n");
        printf("  Ticket ID: %d\n", tickets[i].id);
        printf("  Route: %s\n", route->route_name);
        printf("  Vehicle: %s\n", route->vehicle_type);
        printf("  From: %s -> To: %s\n", tickets[i].source, tickets[i].destination);
        printf("  Date: %s\n", tickets[i].date);
        printf("  Departure: %s\n", tickets[i].time);
        printf("  Price: %.2f BDT\n", tickets[i].price);
        printf("  Available Seats: %d/%d\n", tickets[i].available_seats, tickets[i].total_seats);
        printf("  Status: %s\n", tickets[i].active ? "Active" : "Inactive");
        printf("  ========================================\n\n");
        found = 1;
    }

    if(!found) {
        printf("  No tickets available.\n");
        printf("  Please check if:\n");
        printf("  - Tickets have been added by admin\n");
        printf("  - Tickets are active (not disabled)\n");
        printf("  - Tickets have available seats\n");
    }
    wait_for_enter();
}

void book_ticket_with_route() {
    int ticket_id;
    char seat_input[MAX_STR];
    char selected_seats[MAX_SEATS][4];
    int seat_count;
    char payment_method[20];
    char transaction_id[30];
    int src_num, dest_num;

    view_available_tickets_with_routes();

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
        printf("  Ticket not available! (Status: %s, Seats: %d)\n",
               tickets[idx].active ? "Active" : "Inactive",
               tickets[idx].available_seats);
        wait_for_enter();
        return;
    }

    int route_idx = get_route_by_id(tickets[idx].route_id);
    if(route_idx == -1) {
        printf("  Route not found!\n");
        wait_for_enter();
        return;
    }
    Route *route = route_graph.routes[route_idx];

    print_header("BOOK TICKET WITH ROUTE");
    printf("\n\n");
    center_text("========================================");
    center_text("        BOOK TICKET                  ");
    center_text("========================================");
    printf("\n\n");

    printf("  Ticket ID: %d\n", tickets[idx].id);
    printf("  Route: %s\n", route->route_name);
    printf("  Vehicle: %s\n", route->vehicle_type);
    printf("  Date: %s\n", tickets[idx].date);
    printf("  Base Departure: %s\n", tickets[idx].time);

    printf("\n  Available Waypoints (select by number):\n");
    for(int i = 0; i < route->waypoint_count; i++) {
        printf("  [%d] %s (Arrival Time: %s)\n",
               i, route->waypoints[i].name,
               get_departure_time_at_waypoint(route, i, tickets[idx].time));
    }

    do {
        printf("\n  Enter Source Waypoint Number (0 to %d): ", route->waypoint_count - 1);
        scanf("%d", &src_num);
        clear_input_buffer();
        if(src_num >= 0 && src_num < route->waypoint_count) break;
        printf("  Invalid number! Please enter 0 to %d\n", route->waypoint_count - 1);
    } while(1);

    do {
        printf("  Enter Destination Waypoint Number (0 to %d): ", route->waypoint_count - 1);
        scanf("%d", &dest_num);
        clear_input_buffer();
        if(dest_num >= 0 && dest_num < route->waypoint_count) break;
        printf("  Invalid number! Please enter 0 to %d\n", route->waypoint_count - 1);
    } while(1);

    if(src_num == dest_num) {
        printf("  Source and destination cannot be same!\n");
        wait_for_enter();
        return;
    }

    float distance = calculate_distance_between_waypoints(route, src_num, dest_num);
    float calculated_price = calculate_ticket_price(route, src_num, dest_num);

    printf("\n  ========================================\n");
    printf("  From: %s\n", route->waypoints[src_num].name);
    printf("  To: %s\n", route->waypoints[dest_num].name);
    printf("  Distance: %.2f KM\n", distance);
    printf("  Price: %.2f BDT\n", calculated_price);
    printf("  Estimated Travel Time: %.2f hours\n",
           calculate_time_between_waypoints(route, src_num, dest_num));
    printf("  Departure Time: %s\n", get_departure_time_at_waypoint(route, src_num, tickets[idx].time));
    printf("  Arrival Time: %s\n", get_departure_time_at_waypoint(route, dest_num, tickets[idx].time));
    printf("  ========================================\n");

    display_seat_status_tree(&tickets[idx]);

    printf("\n  Enter seat numbers (e.g., A1, B2, C3): ");
    safe_input(seat_input, MAX_STR);

    if(!parse_seat_numbers(seat_input, selected_seats, &seat_count)) {
        printf("  No valid seats entered!\n");
        wait_for_enter();
        return;
    }

    for(int i = 0; i < seat_count; i++) {
        SeatBST *seat = search_seat_bst(tickets[idx].seat_tree->root, selected_seats[i]);
        if(!seat || !seat->is_available) {
            printf("  Seat %s is not available!\n", selected_seats[i]);
            wait_for_enter();
            return;
        }
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

    if(!process_payment(calculated_price * seat_count, payment_method, transaction_id)) {
        printf("  Payment cancelled.\n");
        wait_for_enter();
        return;
    }

    Booking new_booking;
    new_booking.id = get_next_booking_id();
    new_booking.ticket_id = ticket_id;
    new_booking.user_id = current_user.id;
    strcpy(new_booking.customer_name, current_user.fullname);
    strcpy(new_booking.source, route->waypoints[src_num].name);
    strcpy(new_booking.destination, route->waypoints[dest_num].name);
    new_booking.distance_traveled = distance;

    strcpy(new_booking.seat_numbers, "");
    for(int i = 0; i < seat_count; i++) {
        if(i > 0) strcat(new_booking.seat_numbers, ",");
        strcat(new_booking.seat_numbers, selected_seats[i]);
    }

    new_booking.seats_booked = seat_count;
    new_booking.total_amount = calculated_price * seat_count;
    strcpy(new_booking.booking_date, get_date());
    strcpy(new_booking.status, "confirmed");
    strcpy(new_booking.payment_method, payment_method);
    strcpy(new_booking.transaction_id, transaction_id);

    for(int i = 0; i < seat_count; i++) {
        update_seat_availability(tickets[idx].seat_tree->root, selected_seats[i], 0);
        for(int j = 0; j < tickets[idx].total_seats; j++) {
            if(str_icmp(selected_seats[i], tickets[idx].seats[j]) == 0) {
                tickets[idx].seat_status[j] = 'X';
                break;
            }
        }
    }
    tickets[idx].available_seats -= seat_count;
    tickets[idx].seat_tree->available_seats -= seat_count;

    bookings[booking_count++] = new_booking;
    save_tickets();
    save_bookings();

    print_header("BOOKING SUCCESS");
    printf("\n\n");
    center_text("========================================");
    center_text("     BOOKING CONFIRMED!               ");
    center_text("========================================");
    printf("\n");
    printf("  Booking ID    : %d\n", new_booking.id);
    printf("  Ticket ID     : %d\n", ticket_id);
    printf("  Route         : %s\n", route->route_name);
    printf("  From          : %s\n", route->waypoints[src_num].name);
    printf("  To            : %s\n", route->waypoints[dest_num].name);
    printf("  Distance      : %.2f KM\n", distance);
    printf("  Seats         : %s\n", new_booking.seat_numbers);
    printf("  Seats Booked  : %d\n", seat_count);
    printf("  Total Amount  : %.2f BDT\n", new_booking.total_amount);
    printf("  Payment Method: %s\n", payment_method);
    printf("  Transaction ID: %s\n", transaction_id);
    printf("  Departure Time: %s\n", get_departure_time_at_waypoint(route, src_num, tickets[idx].time));
    printf("  Arrival Time  : %s\n", get_departure_time_at_waypoint(route, dest_num, tickets[idx].time));
    printf("  Status        : %s\n", new_booking.status);
    printf("\n");
    wait_for_enter();
}

void view_my_bookings_with_details() {
    print_header("MY BOOKINGS");
    printf("\n\n");
    center_text("========================================");
    center_text("         MY BOOKINGS                 ");
    center_text("========================================");
    printf("\n\n");

    int found = 0;

    printf("  %-6s %-20s %-20s %-12s %-12s %-8s %-10s\n",
           "ID", "From", "To", "Seats", "Distance", "Qty", "Amount");
    print_separator('-', 100);

    for(int i = 0; i < booking_count; i++) {
        if(bookings[i].user_id == current_user.id) {
            printf("  %-6d %-20s %-20s %-12s %-12.2f %-8d %-10.2f\n",
                   bookings[i].id,
                   bookings[i].source,
                   bookings[i].destination,
                   bookings[i].seat_numbers,
                   bookings[i].distance_traveled,
                   bookings[i].seats_booked,
                   bookings[i].total_amount);
            found = 1;
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
    printf("\n  From: %s -> To: %s", bookings[idx].source, bookings[idx].destination);
    printf("\n  Seats: %s", bookings[idx].seat_numbers);
    printf("\n  Amount: %.2f BDT", bookings[idx].total_amount);
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
            for(int i = 0; i < count; i++) {
                update_seat_availability(tickets[tidx].seat_tree->root, seats[i], 1);
            }
            tickets[tidx].seat_tree->available_seats += count;
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

void customer_menu() {
    int choice;
    while(1) {
        print_header("CUSTOMER DASHBOARD");
        printf("\n\n");
        center_text("========================================");
        center_text("         CUSTOMER PANEL              ");
        center_text("========================================");
        printf("\n\n");
        center_text("[1] View Available Tickets with Routes");
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
            case 1: view_available_tickets_with_routes(); break;
            case 2: book_ticket_with_route(); break;
            case 3: view_my_bookings_with_details(); break;
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

/* ======================================================================
 * MAIN FUNCTION
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
        center_text("  (With Route Management)");
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
