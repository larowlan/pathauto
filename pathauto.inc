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

/**
 * Output a helpful message if verbose output is enabled.
 *
 * Verbose output is only enabled when:
 * - The 'pathauto_verbose' setting is enabled.
 * - The current user has the 'notify of path changes' permission.
 * - The $op parameter is anything but 'bulkupdate' or 'return'.
 *
 * @param string $message
 *   An optional string of the verbose message to display. This string should
 *   already be run through t().
 * @param string $op
 *   An optional string with the operation being performed.
 *
 * @return bool
 *   TRUE if verbose output is enabled, or FALSE otherwise.
 */
function _pathauto_verbose($message = NULL, $op = NULL) {
  static $verbose;
  $config = \Drupal::configFactory()->get('pathauto.settings');

  if (!isset($verbose)) {
    $verbose = $config->get('verbose') && \Drupal::currentUser()->hasPermission('notify of path changes');
  }

  if (!$verbose || (isset($op) && in_array($op, array('bulkupdate', 'return')))) {
    return FALSE;
  }

  if ($message) {
    drupal_set_message($message);
  }

  return $verbose;
}

/**
 * Clean tokens so they are URL friendly.
 *
 * @param array $replacements
 *   An array of token replacements
 *   that need to be "cleaned" for use in the URL.
 * @param array $data
 *   An array of objects used to generate the replacements.
 * @param array $options
 *   An array of options used to generate the replacements.
 */
function pathauto_clean_token_values(&$replacements, $data = array(), $options = array()) {
  foreach ($replacements as $token => $value) {
    // Only clean non-path tokens.
    if (!preg_match('/(path|alias|url|url-brief)\]$/', $token)) {
      $replacements[$token] = \Drupal::service('pathauto.manager')->cleanString($value, $options);
    }
  }
}
