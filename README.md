
# 🎫 Online Ticket Booking System

A comprehensive console-based Ticket Booking System written in **ANSI C (C99)**. This project demonstrates a complete ticket booking workflow with user authentication, seat selection, payment processing, and file-based data persistence.

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
- [Payment Options](#-payment-options)
- [Data Storage](#-data-storage)
- [Screenshots](#-screenshots)
- [Future Improvements](#-future-improvements)
- [Contributing](#-contributing)
- [License](#-license)

---

## ✨ Features

### 🔐 Authentication
- User Registration with validation
- Secure Login with attempt limit (3 attempts)
- Password Change & Forgot Password functionality
- Role-based access (Admin / Customer)

### 👑 Admin Features
- **User Management**: Add, Edit, Delete, List, Toggle User Status
- **Ticket Management**: Add, Edit, Delete, List Tickets
- **Seat Configuration**: Set total seats and seats per row
- **View All Bookings**: Monitor all customer bookings
- **Profile Management**: View and update profile
- **Password Management**: Change password

### 👤 Customer Features
- **View Available Tickets**: Browse all available tickets with seat availability
- **Book Tickets**: 
  - View interactive seating layout
  - Select specific seats (e.g., A1, B2, C3)
  - Case-insensitive seat selection (A1 or a1 both work)
  - Multiple seat booking at once
  - Visual seat status: `(O)` = Available, `(X)` = Booked
- **Payment Processing**: Multiple payment options
- **Booking History**: View all past and current bookings
- **Cancel Booking**: Cancel with automatic seat release
- **Profile Management**: View and update profile
- **Invoice Generation**: Print booking invoices

### 💳 Payment Options
- **bKash**: Mobile banking with number and PIN verification
- **Bank Transfer**: Account number and reference
- **Cash**: On-booking cash payment

### 🎨 User Interface
- Professional ASCII-based UI
- Splash screen with loading animation
- Center-aligned text
- Clean seating layout display
- Case-insensitive input handling

---

## 👥 Team Members

| Name | Role | Section |
|------|------|---------|
| **Md. Salah Uddin** | Developer | Core Architecture, Main Function |
|  | Developer | UI & Splash Screen Functions |
|  | Developer | Helper, Seat & Validation Functions |
|  | Developer | File Handling & Payment Functions |
|  | Developer | User, Admin & Customer Functions |

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
├── system.c # Main program file (complete system)
├── users.txt # User data (auto-generated)
├── tickets.txt # Ticket data (auto-generated)
└── bookings.txt # Booking data (auto-generated)


### File Details

| File | Description |
|------|-------------|
| `system.c` | Complete source code with all functionalities |
| `users.txt` | Stores user credentials, profile info, and roles |
| `tickets.txt` | Stores ticket details, seat layout, and availability |
| `bookings.txt` | Stores booking records, seat selection, and payment info |

---

## 🔧 Installation & Setup

### Prerequisites

- **GCC Compiler** (MinGW for Windows, GCC for Linux/Mac)
- **Make** (optional, for build automation)
- **Git** (for cloning the repository)

### Clone the Repository

```bash
git clone https://github.com/yourusername/ticket-booking-system.git
cd ticket-booking-system
```

🔑 Default Login Credentials
Role	Username	Password
Admin	admin	admin123
⚠️ Note: Change the default admin password after first login for security.

📖 User Guide
Admin Panel
Login with admin credentials

User Management:

List all users

Add new users (admin/customer)

Edit user details

Delete users

Toggle user active/inactive status

Ticket Management:

Add new tickets with:

Source & Destination

Date & Time

Price

Total seats

Seats per row

Edit existing tickets

Delete tickets

List all tickets

Booking Management:

View all bookings

View booking details

Cancel bookings

Profile Management:

View profile

Update profile

Change password

Customer Panel
Login with customer credentials

View Available Tickets: See all available tickets

Book Ticket:

Select a ticket

View seating layout:
  Seating Layout (O=Available, X=Booked):
        1    2    3    4
      ---------------------
  A |  A1(O) A2(X) A3(O) A4(O)
  B |  B1(O) B2(O) B3(X) B4(O)
  C |  C1(O) C2(O) C3(O) C4(O)

    Seating Layout (O=Available, X=Booked):
        1    2    3    4
      ---------------------
  A |  A1(O) A2(X) A3(O) A4(O)
  B |  B1(O) B2(O) B3(X) B4(O)
  C |  C1(O) C2(O) C3(O) C4(O)
