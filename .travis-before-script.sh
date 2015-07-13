#!/bin/bash

set -e $DRUPAL_TI_DEBUG

# Ensure the right Drupal version is installed.
# Note: This function is re-entrant.
drupal_ti_ensure_drupal

# Download token 8.x-1.x
cd "$DRUPAL_TI_DRUPAL_DIR"
cd modules
git clone --depth 1 --branch 8.x-1.x http://git.drupal.org/project/token.git
