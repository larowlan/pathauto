<?php

/**
 * @file
 * Miscellaneous functions for Pathauto.
 *
 * This also contains some constants giving human readable names to some numeric
 * settings; they're included here as they're only rarely used outside this file
 * anyway. Use module_load_include('inc', 'pathauto') if the constants need to
 * be available.
 *
 * @ingroup pathauto
 */
use Drupal\Component\Utility\Unicode;
use Drupal\Core\Language\Language;
use Symfony\Component\Routing\Exception\ResourceNotFoundException;

/**
 * Verify if the given path is a valid menu callback.
 *
 * Taken from menu_execute_active_handler().
 *
 * @param string $path
 *   A string containing a relative path.
 *
 * @return bool
 *   TRUE if the path already exists.
 */
function _pathauto_path_is_callback($path) {

  if (is_file(DRUPAL_ROOT . '/' . $path) || is_dir(DRUPAL_ROOT . '/' . $path)) {
    // Do not allow existing files or directories to get assigned an automatic
    // alias. Note that we do not need to use is_link() to check for symbolic
    // links since this returns TRUE for either is_file() or is_dir() already.
    return TRUE;
  }

  try {
    \Drupal::service('router')->match('/' . $path);
    return TRUE;
  }
  catch (ResourceNotFoundException $e) {
    return FALSE;
  }
}

