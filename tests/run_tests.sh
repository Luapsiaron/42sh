#!/bin/sh

VENV_DIR=".venv"

if [ ! -d "$VENV_DIR" ]; then
    echo "Creating python environment..."
    python3 -m venv "$VENV_DIR"
    . "$VENV_DIR/bin/activate" # activate to install temporarly
    pip install pytest pytest-sugar
fi

. "$VENV_DIR/bin/activate"

export PYTHONPATH="$PYTHONPATH:." # add current folder to python search

pytest step*/test_*.py # add -v for more details

