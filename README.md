# TFTP Client-Server Implementation

A simple and efficient implementation of the Trivial File Transfer Protocol (TFTP) in C, providing both client and server applications for reliable file transfer over UDP.

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

## Features

### Client Application
- **Connect**: Establish connection to TFTP server
- **Upload (PUT)**: Send files to the server
- **Download (GET)**: Retrieve files from the server
- **Mode Change**: Switch between ASCII and binary transfer modes
- **Help**: Display usage information
- **Interactive Menu**: User-friendly command-line interface

### Server Application
- **Concurrent Handling**: Processes multiple client requests
- **File Operations**: Supports both read (download) and write (upload) operations
- **Error Handling**: Robust error management for network operations
- **Port Configuration**: Runs on standard TFTP port (6000)

## Prerequisites

- GCC compiler (or any C compiler)
- Linux/Unix environment (or Windows with MinGW for socket support)
- Basic understanding of command-line operations

## Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd tftp
   ```

2. Build the client:
   ```bash
   cd client
   make
   ```

3. Build the server:
   ```bash
   cd server
   make
   ```

## Usage

### Running the Server
```bash
cd server
./tftp_server
```
The server will start listening on port 6000 and display a confirmation message.

### Running the Client
```bash
cd client
./tftp_client
```

The client presents an interactive menu with the following options:
1. **CONNECT**: Connect to the TFTP server
2. **PUT FILE**: Upload a file to the server
3. **GET FILE**: Download a file from the server
4. **MODE**: Change transfer mode (ASCII/Binary)
5. **HELP**: Display help information
6. **EXIT**: Quit the application

### Example Usage
1. Start the server in one terminal
2. Start the client in another terminal
3. Select option 1 to connect
4. Use options 2 or 3 to transfer files

## Project Structure

```
tftp/
├── client/
│   ├── main.c          # Client main program
│   ├── connect.c       # Connection handling
│   ├── upload.c        # File upload functionality
│   ├── download.c      # File download functionality
│   ├── mode.c          # Mode change operations
│   ├── help.c          # Help display
│   ├── end.c           # terminate server function
│   ├── main.h          # Client header file
│   ├── makefile        # Build script for client
│   └── text.txt        # Sample text file
└── server/
    ├── main.c          # Server main program
    ├── common.c        # Common utilities
    ├── connect.c       # Connection management
    ├── get_file.c      # File retrieval handling
    ├── put_file.c      # File upload handling
    ├── main.h          # Server header file
    └── makefile        # Build script for server
```
## Sample Execution

| Step | Terminal Output |
|------|-----------------|
| **1. CONNECT** | <img width="1920" height="1080" alt="tftp_1" src="https://github.com/user-attachments/assets/3ac0e2ed-3272-42f7-ad0d-31111032078f" /> |
| **2. PUT FILE** | <img width="1920" height="1080" alt="tftp_2" src="https://github.com/user-attachments/assets/e7b75990-ea63-4ca6-a6ac-9cb495f7912f" /> |
| **3. GET FILE** | <img width="1920" height="1080" alt="tftp_3" src="https://github.com/user-attachments/assets/e09c257f-8483-444e-b224-674fe7d52354" /> |
| **4. GET FILE** | <img width="1920" height="1080" alt="tftp_4" src="https://github.com/user-attachments/assets/9beb28b9-a27e-413a-96c0-bcf44bb475b2" /> |
| **5. MODE_CHANGE & HELP** | <img width="1920" height="1080" alt="tftp_5" src="https://github.com/user-attachments/assets/a326a03d-0310-41a4-bef9-0539b3936c6b" /> |
| **6. EXIT** | <img width="1920" height="1080" alt="tftp_6" src="https://github.com/user-attachments/assets/b0244806-6dc8-4a5b-9e77-d208edd01daf" /> |
| **7. SERVER** | <img width="1920" height="1080" alt="tftp_7" src="https://github.com/user-attachments/assets/b21a2e55-6a19-4838-ad5d-8886eb2a5377" /> |

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

**Note**: This implementation is for educational purposes and may not include all features of the full TFTP specification. Use in production environments is not recommended without further testing and security enhancements.
