#!/bin/bash
docker run -it --rm -v "$PWD":/app -w /app node:16 bash
