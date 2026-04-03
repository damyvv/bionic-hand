# Philips-academy

This project uses a development container running in Ubuntu WSL2 on Windows, providing a consistent development environment for all developers. It includes automatic OpenOCD setup for debugging, where the GDB server runs on Windows while debugging occurs within the container.

## Prerequisites

1. **Windows with WSL2 and Ubuntu**: Ensure you have Windows with WSL2 and Ubuntu installed.
2. **Visual Studio Code**: Install [Visual Studio Code](https://code.visualstudio.com/) with the "Dev Containers" extension.
3. **ST-Link**: Have an ST-Link debugger connected to your system.
4. **Set WSL2 as the default**: Make sure that the WSL2 version running is the one where you have the project code. You can check this with the following command:
   ```bash
   wsl --list --verbose
   ```
   This will show the list of installed WSL distros and their versions. If your desired WSL is not the default, change it with the command:
   ```bash
   wsl --setdefault <distro_name>
   ```
   Replace `<distro_name>` with the name of the WSL distro (e.g., `Ubuntu-20.04`) you want to use.

## Project Setup

1. **Initialize Submodules**:
```bash
git submodule update --init --recursive
```

2. **Open in VS Code**:
```bash
code .
```

3. When prompted, click "Reopen in Container" to start the development container.

## Development Environment

The project uses a development container that provides:
- All necessary build tools and dependencies
- Automatic OpenOCD installation and configuration
- Integrated debugging support

### Automatic OpenOCD Setup

The container automatically:
1. Installs OpenOCD on your Windows system if not present.
2. Sets up monitoring services for debugging communication.
3. Manages the connection between the container and Windows-based GDB server.

## Building and Debugging

### Building the Project

1. Select the `stm32f429zi` configuration in VS Code.
2. Build using either:
   - VS Code's build command (Ctrl+Shift+B)
   - CMake commands in the terminal.

### Debugging

1. Connect your ST-Link debugger.
2. Press F5 to start debugging.
   - The system will automatically:
     - Update the WSL IP address.
     - Start OpenOCD if not running.
     - Connect the debugger.

## Testing

1. Select "Configuration for Host Tooling and Tests" in VS Code.
2. Run tests using the "CMake: Run CTest" command.

## Project Structure

```
.
├── .devcontainer/        # Development container configuration
├── src/                  # Source code
├── tests/                # Test files
├── libs/                 # External libraries and dependencies
└── scripts/              # Utility scripts for development
```

## Technical Details

- **Development Container**: Runs Ubuntu in WSL2.
- **Debug Setup**: 
  - GDB Server: Runs on Windows via OpenOCD.
  - Debug Client: Runs in container.
  - Communication: Automated via monitoring services.
- **Build System**: CMake with presets for different targets.
- **Testing Framework**: Google Test.

## Troubleshooting

If debugging doesn't start:
1. Check if OpenOCD is running (will be started automatically if needed).
2. Verify ST-Link connection.
3. Ensure the WSL IP address is correctly updated.

For other issues, check the VS Code output and debug console for detailed information.
