#!/bin/bash

OPENOCD_PATH="/mnt/c/OpenOCD"
GITHUB_API_URL="https://api.github.com/repos/openocd-org/openocd/releases"

check_openocd() {
    if [ -f "/mnt/c/OpenOCD/openocd.exe" ]; then
        echo "OpenOCD found at: C:\OpenOCD\openocd.exe"
        return 0
    fi
    return 1
}

get_latest_openocd() {
    echo "Fetching latest OpenOCD release from GitHub..."
    
    # Get the latest non-prerelease version
    RELEASE_INFO=$(curl -s "$GITHUB_API_URL" | \
                  jq -r '[.[] | select(.prerelease==false)][0]')
    
    # Get version number
    VERSION=$(echo "$RELEASE_INFO" | jq -r '.tag_name')
    echo "Latest version: $VERSION"
    
    # Find the Windows asset
    DOWNLOAD_URL=$(echo "$RELEASE_INFO" | \
                  jq -r '.assets[] | select(.name | contains("mingw32")) | .browser_download_url')
    
    if [ -n "$DOWNLOAD_URL" ]; then
        echo "Found download URL: $DOWNLOAD_URL"
        echo "$DOWNLOAD_URL"
        return 0
    else
        echo "Error: Could not find Windows release"
        return 1
    fi
}

install_openocd() {
    echo "OpenOCD not found, installing..."
    
    # Create temp directory
    TEMP_DIR=$(mktemp -d)
    cd "$TEMP_DIR"

    # Get latest version URL and store it in a variable
    DOWNLOAD_URL=$(get_latest_openocd | tail -n 1)
    if [ $? -ne 0 ] || [ -z "$DOWNLOAD_URL" ]; then
        echo "Failed to get latest OpenOCD version"
        cd - > /dev/null
        rm -rf "$TEMP_DIR"
        return 1
    fi
    
    echo "Downloading OpenOCD from: $DOWNLOAD_URL"
    echo "Trying wget..."
    if ! wget --no-verbose "$DOWNLOAD_URL" -O openocd.tar.gz; then
        echo "wget failed, trying curl..."
        if ! curl -L -o openocd.tar.gz "$DOWNLOAD_URL"; then
            echo "Failed to download OpenOCD"
            cd - > /dev/null
            rm -rf "$TEMP_DIR"
            return 1
        fi
    fi

    # Verify download
    if [ ! -s openocd.tar.gz ]; then
        echo "Download appears to be empty"
        cd - > /dev/null
        rm -rf "$TEMP_DIR"
        return 1
    fi

    echo "Extracting..."
    if ! tar xf openocd.tar.gz --strip-components=1; then
        echo "Failed to extract OpenOCD"
        cd - > /dev/null
        rm -rf "$TEMP_DIR"
        return 1
    fi

    echo "Creating installation directory..."
    # Remove old installation if it exists
    rm -rf "$OPENOCD_PATH"
    mkdir -p "$OPENOCD_PATH"

    echo "Moving files..."
    # Move all files to installation directory
    mv ./* "$OPENOCD_PATH/"

    echo "Cleaning up..."
    cd - > /dev/null
    rm -rf "$TEMP_DIR"

    if [ -f "$OPENOCD_PATH/openocd.exe" ]; then
        echo "OpenOCD installed successfully!"
        return 0
    else
        echo "Installation failed: OpenOCD not found at expected location"
        return 1
    fi
}

# Main script
if ! check_openocd; then
    if ! install_openocd; then
        echo "Failed to install OpenOCD"
        exit 1
    fi
fi

check_openocd