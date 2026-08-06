# 🎫 Online Ticket Booking System

A comprehensive console-based Ticket Booking System written in **ANSI C (C99)**. This project demonstrates a complete ticket booking workflow with user authentication, seat selection, route management, payment processing, and file-based data persistence.

---

## 📋 Table of Contents

- [Features](#-features)
- [Team Members](#-team-members)
- [Technologies Used](#-technologies-used)
- [Project Structure](#-project-structure)
- [Installation & Setup](#-installation--setup)
- [How to Run](#-how-to-run)
- [Default Login Credentials](#-default-login-credentials)
- [User Guide](#-user-guide)
  - [Admin Panel](#admin-panel)
  - [Customer Panel](#customer-panel)
- [Payment Options](#-payment-options)
- [Data Storage](#-data-storage)
- [Data Structures](#-data-structures)
- [Future Improvements](#-future-improvements)
- [Contributing](#-contributing)
- [License](#-license)

---

## ✨ Features

### 🔐 Authentication
- User Registration with validation
- Secure Login with attempt limit (3 attempts)
- Password Change functionality
- Role-based access (Admin / Customer)

### 👑 Admin Features
- **User Management**: Add, Edit, Delete, List, Toggle User Status
- **Ticket Management**: Add, Edit, Delete, List Tickets with Route integration
- **Route Management**: Create, Edit, Delete Routes with waypoints
- **Seat Configuration**: Set total seats and seats per row with dynamic layout
- **View All Bookings**: Monitor all customer bookings
- **Profile Management**: View and update profile
- **Password Management**: Change password

### 👤 Customer Features
- **View Available Tickets**: Browse all available tickets with route details and seat availability
- **Book Tickets**: 
  - View interactive seating layout with real-time status
  - Select specific seats (e.g., A1, B2, C3)
  - Case-insensitive seat selection (A1 or a1 both work)
  - Multiple seat booking at once
  - Visual seat status: `(O)` = Available, `(X)` = Booked
  - Route-based distance and price calculation
- **Payment Processing**: Multiple payment options with transaction IDs
- **Booking History**: View all past and current bookings with distance traveled
- **Cancel Booking**: Cancel with automatic seat release and refund notification
- **Profile Management**: View and update profile
- **Password Management**: Change password

### 🗺️ Route Management
- **Create Routes**: Define routes with waypoints
- **Waypoint Management**: Add, Edit, Remove waypoints
- **Distance Calculation**: Automatic distance between waypoints
- **Price Calculation**: Dynamic pricing based on distance and multipliers
- **Time Calculation**: Arrival/departure time calculation
- **Vehicle Types**: Support for Bus, Train, Flight

### 💳 Payment Options
- **bKash**: Mobile banking with number and PIN verification
- **Bank Transfer**: Account number and reference
- **Cash**: On-booking cash payment

### 🎨 User Interface
- Professional ASCII-based UI with box drawing
- Splash screen with loading animation and progress bar
- Center-aligned text for professional appearance
- Clean seating layout display with color indicators
- Case-insensitive input handling
- Real-time date and time display
- Interactive menu system

---

## 👥 Team Members

| Name | Role | Section |
|------|------|---------|
| **Md. Salah Uddin** | Developer | UI & Splash Screen Functions, Helper & Utility Functions |
| **Mohammad Bakhtiar** | Developer | Seat Functions, Priority Queue Functions |
| **Sumona Moni** | Developer | Tree Functions for Seat Availability |
| **Mohammad Rafi** | Developer | Route Graph Functions and Management |
| **Md Injamul Haque Titash** | Developer | User, Admin, Customer Functions, File Handling |

---

## 🛠 Technologies Used

- **Language**: ANSI C (C99 Standard)
- **Compiler**: GCC / MinGW
- **IDE**: Code::Blocks / VS Code
- **Libraries**: 
  - Standard C Library (`stdio.h`, `stdlib.h`, `string.h`, `time.h`, `ctype.h`)
  - Windows API (`windows.h`) - for Windows platform
  - POSIX (`unistd.h`) - for Linux/Mac platform

---

## 📁 Project Structure

TicketBooking/
├── cc.c # Main program file (complete system)
├── users.txt # User data (auto-generated)
├── tickets.txt # Ticket data (auto-generated)
├── routes.txt # Route data (auto-generated)
└── bookings.txt # Booking data (auto-generated)

text

### File Details

| File | Description |
|------|-------------|
| `system.c` | Complete source code with all functionalities |
| `users.txt` | Stores user credentials, profile info, and roles |
| `tickets.txt` | Stores ticket details, seat layout, and availability |
| `routes.txt` | Stores route definitions, waypoints, and pricing |
| `bookings.txt` | Stores booking records, seat selection, and payment info |

---

## 🔧 Installation & Setup

### Prerequisites

- **GCC Compiler** (MinGW for Windows, GCC for Linux/Mac)
- **Make** (optional, for build automation)
- **Git** (for cloning the repository)

### Clone the Repository

```bash
git clone https://github.com/salahuddin-saif/Ticket-Booking-System.git
```
```
cd Ticket-Booking-System
```

./ticket_booking
🔑 Default Login Credentials
Role	Username	Password
Admin	admin	admin123
Customer	Register new account	-
⚠️ Note: Change the default admin password after first login for security.

📖 User Guide
Admin Panel
Login
Run the program

Select option [1] Login

Enter username: admin

Enter password: admin123

Access Admin Dashboard

User Management
List Users: View all registered users with their roles

Add User: Create new admin or customer accounts

Edit User: Modify user details and roles

Delete User: Remove user accounts (cannot delete own account)

Toggle User Status: Activate/deactivate users

Ticket Management
Add Ticket with Route:

Select a route from available routes
Choose source and destination waypoints
Enter date and time
Set total seats and seats per row
System automatically calculates price based on distance
Edit Ticket: Modify date, time, price, or toggle status

Delete Ticket: Remove tickets (with seat tree cleanup)

List Tickets: View all tickets with availability

Route Management
Create New Route:

Enter route name and vehicle type
Set base price per KM
Add waypoints with distances and times
System calculates price multipliers
Add Waypoint: Insert waypoints at any position

Edit Waypoint: Modify name, distance, or time

Remove Waypoint: Delete waypoints (minimum 2)

Delete Route: Remove entire route

View Routes: List all routes with waypoints

Booking Management
View All Bookings: Monitor all customer bookings

View booking details including seats, amounts, and status

Profile Management
View Profile: See your user details

Update Profile: Change full name

Change Password: Update password with validation

Customer Panel
Registration
Select option [2] Register

Enter username (3-20 characters, alphanumeric and underscore)

Enter password (6-20 chars, must contain uppercase, lowercase, digit)

Confirm password

Enter full name

Account created successfully

View Available Tickets
Browse all available tickets with:

Route details with vehicle type

Source and destination

Date and departure time

Price

Available seats count

Book Ticket
Select [2] Book Ticket

Enter Ticket ID

View route details and waypoints with arrival times

Select source and destination waypoints

View distance, price, and travel time

View seating layout:

text
Seating Layout (O=Available, X=Booked):
```
       1    2    3    4
     ---------------------
A  |  A1(O) A2(O) A3(O) A4(O)
B  |  B1(O) B2(O) B3(O) B4(O)
C  |  C1(O) C2(O) C3(O) C4(O)
```
Legend: (O) = Available, (X) = Booked
Available Seats: 12 / 12
Enter seat numbers (e.g., A1, B2, C3 or A1 B2 C3)

Confirm booking

Choose payment method

Complete payment

Get booking confirmation with transaction ID

View My Bookings
View all your bookings with:

Booking ID

Source and destination

Seat numbers

Distance traveled

Number of seats

Total amount

Cancel Booking
Enter Booking ID

View booking details

Confirm cancellation

Seats are automatically released

Refund notification displayed

Profile Management
View Profile: See your user details

Update Profile: Change full name

Change Password: Update password with validation

💳 Payment Options
1. bKash Payment
Enter bKash number

Enter PIN

Transaction ID generated automatically

Payment confirmation displayed

2. Bank Transfer
Display bank account details

Enter your account number

Enter reference

Transaction ID generated automatically

3. Cash Payment
Pay on booking

Transaction ID generated automatically

Booking confirmed immediately

💾 Data Storage
users.txt Format
text
1,Administrator,admin,admin123,admin,1
2,John Doe,johndoe,pass123,customer,1
tickets.txt Format
text
1,1,Dhaka,Naogaon,15-12-2025,08:00 AM,575.00,40,4,10,40,1
A1,A2,A3,A4,B1,B2,...
OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
routes.txt Format
text
1,Dhaka-Naogaon Route,Bus,2.50,8,1
Dhaka,0.00,0.00,1.00,0
Abdullahpur,15.00,0.50,1.00,1
...
bookings.txt Format
text
1,1,2,John Doe,A1,B2,C3,3,1725.00,15-12-2025,confirmed,bKash,BKASH-123-456,Dhaka,Naogaon,230.00
🏗️ Data Structures
Core Structures
```
c
// User Management
typedef struct {
    int id;
    char username[30];
    char password[30];
    char fullname[50];
    char role[10];
    int active;
} User;

// Route Management
typedef struct {
    char name[50];
    float distance_from_start;
    float time_from_start;
    float price_multiplier;
    int sequence;
} Waypoint;

typedef struct {
    int id;
    char route_name[100];
    char vehicle_type[20];
    Waypoint waypoints[MAX_WAYPOINTS];
    int waypoint_count;
    float base_price;
    int active;
    struct Route *next;
} Route;

// Seat Management
typedef struct SeatBSTNode {
    char seat_id[4];
    int seat_index;
    int is_available;
    float price;
    struct SeatBSTNode *left;
    struct SeatBSTNode *right;
} SeatBST;

// Ticket Management
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

// Booking Management
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

// Priority Queue
typedef struct {
    SeatNode heap[MAX_HEAP];
    int size;
} PriorityQueue;
```

