#!/usr/bin/env bash

BASE_DIR=$(pwd)

function install_needed_tools {
    # update package manager cache
    sudo apt-get update

    # install node package manager npm
    sudo apt-get install npm curl git

    sudo npm config set prefix /usr/local

    sudo npm install -g n
    sudo n 0.10.33

    # Install grunt
    sudo npm install -g grunt-cli

    # Install handlebars
    sudo npm install -g handlebars@3.0.1

    # install bower
    sudo npm install -g bower
}

function build_mpin {
    APP_TYPE=$1
    cd "$BASE_DIR/$APP_TYPE"

    # Install required modules for Node
    sudo npm install

    # Create settings.json
    cp settings.json_build settings.json

    # Build the app
    grunt build
}

install_needed_tools

# Build browser mpin
build_mpin browser

# Build mobile mpin
build_mpin mobile
