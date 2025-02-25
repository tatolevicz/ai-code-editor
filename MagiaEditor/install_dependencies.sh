#!/bin/bash

DIR=${1:-external}

clone_if_not_exists() {
    local dir=$1
    local repo=$2
    local commit=$3
    local name=$(basename "$dir")

    if [ -d "$dir" ]; then
        echo "$DIR $name dir already exists!"
    else
        echo "$DIR Downloading $name:"
        git clone "$repo" "$dir"
        if [ ! -z "$commit" ]; then
            echo "Checking out $name at commit $commit"
            (cd "$dir" && git checkout "$commit")
        fi
    fi
}

# Clona os repositórios nos commits especificados
clone_if_not_exists "$DIR/scintilla" "https://github.com/elcuco/scintilla-code.git"
clone_if_not_exists "$DIR/lexilla" "https://github.com/elcuco/lexilla.git"
clone_if_not_exists "$DIR/lua" "https://github.com/walterschell/Lua.git"
clone_if_not_exists "$DIR/sol2" "https://github.com/ThePhD/sol2.git"
