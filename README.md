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

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

**Note**: This implementation is for educational purposes and may not include all features of the full TFTP specification. Use in production environments is not recommended without further testing and security enhancements.
