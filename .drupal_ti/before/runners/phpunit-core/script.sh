#!/bin/bash

set -e $DRUPAL_TI_DEBUG

# Run phpunit with core tests.
# @todo Support directly in drupal-ti via phpunit-core runner for Drupal 8.

# Find absolute path to module directory.
cd "$DRUPAL_TI_DRUPAL_DIR"
MODULE_DIR=$(cd "$DRUPAL_TI_MODULES_PATH"; pwd)

# Run core tests
cd core
./vendor/bin/phpunit --verbose --debug "$MODULE_DIR/$DRUPAL_TI_MODULE_NAME"
