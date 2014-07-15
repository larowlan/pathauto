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
 * Case should be left as is in the generated path.
 */
define('PATHAUTO_CASE_LEAVE_ASIS', 0);

/**
 * Case should be lowercased in the generated path.
 */
define('PATHAUTO_CASE_LOWER', 1);

/**
 * "Do nothing. Leave the old alias intact."
 */
define('PATHAUTO_UPDATE_ACTION_NO_NEW', 0);

/**
 * "Create a new alias. Leave the existing alias functioning."
 */
define('PATHAUTO_UPDATE_ACTION_LEAVE', 1);

/**
 * "Create a new alias. Delete the old alias."
 */
define('PATHAUTO_UPDATE_ACTION_DELETE', 2);

/**
 * Remove the punctuation from the alias.
 */
define('PATHAUTO_PUNCTUATION_REMOVE', 0);

/**
 * Replace the punctuation with the separator in the alias.
 */
define('PATHAUTO_PUNCTUATION_REPLACE', 1);

/**
 * Leave the punctuation as it is in the alias.
 */
define('PATHAUTO_PUNCTUATION_DO_NOTHING', 2);

/**
 * Fetches an existing URL alias given a path and optional language.
 *
 * @param string $source
 *   An internal Drupal path.
 * @param string $language
 *   An optional language code to look up the path in.
 *
 * @return bool|array
 *   FALSE if no alias was found or an associative array containing the
 *   following keys:
 *   - pid: Unique path alias identifier.
 *   - alias: The URL alias.
 */
function _pathauto_existing_alias_data($source, $language = Language::LANGCODE_NOT_SPECIFIED) {
  $pid = db_query_range("SELECT pid FROM {url_alias} WHERE source = :source AND langcode IN (:language, :language_none) ORDER BY langcode DESC, pid DESC", 0, 1, array(':source' => $source, ':language' => $language, ':language_none' => Language::LANGCODE_NOT_SPECIFIED))->fetchField();
  return \Drupal::service('path.alias_storage')->load(array('pid' => $pid));
}


/**
 * Check to ensure a path alias is unique and add suffix variants if necessary.
 *
 * Given an alias 'content/test' if a path alias with the exact alias already
 * exists, the function will change the alias to 'content/test-0' and will
 * increase the number suffix until it finds a unique alias.
 *
 * @param string $alias
 *   A string with the alias. Can be altered by reference.
 * @param string $source
 *   A string with the path source.
 * @param string $langcode
 *   A string with a language code.
 *
 * @deprecated Use \Drupal::service('pathauto.alias_uniquifier')->uniquify()
 *   instead.
 */
function pathauto_alias_uniquify(&$alias, $source, $langcode) {
  \Drupal::service('pathauto.alias_uniquifier')->uniquify($alias, $source, $langcode);
}

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
 * Private function for Pathauto to create an alias.
 *
 * @param array $path
 *   An associative array containing the following keys:
 *   - source: The internal system path.
 *   - alias: The URL alias.
 *   - pid: (optional) Unique path alias identifier.
 *   - language: (optional) The language of the alias.
 * @param array|bool $existing_alias
 *   (optional) An associative array of the existing path alias.
 * @param string $op
 *   An optional string with the operation being performed.
 *
 * @return array|bool
 *   The saved path or NULL if the path was not saved.
 */
function _pathauto_set_alias(array $path, $existing_alias = NULL, $op = NULL) {
  $verbose = _pathauto_verbose(NULL, $op);
  $config = \Drupal::configFactory()->get('pathauto.settings');

  // Alert users if they are trying to create an alias
  // that is the same as the internal path.
  if ($path['source'] == $path['alias']) {
    if ($verbose) {
      _pathauto_verbose(t('Ignoring alias %alias because it is the same as the internal path.', array('%alias' => $path['alias'])));
    }
    return;
  }

  // Skip replacing the current alias with an identical alias.
  if (empty($existing_alias) || $existing_alias['alias'] != $path['alias']) {
    $path += array('pathauto' => TRUE, 'original' => $existing_alias, 'pid' => NULL);

    // If there is already an alias, respect some update actions.
    if (!empty($existing_alias)) {
      switch ($config->get('update_action')) {
        case PATHAUTO_UPDATE_ACTION_NO_NEW:
          // Do not create the alias.
          return;

        case PATHAUTO_UPDATE_ACTION_LEAVE:
          // Create a new alias instead of overwriting the existing by leaving
          // $path['pid'] empty.
          break;

        case PATHAUTO_UPDATE_ACTION_DELETE:
          // The delete actions should overwrite the existing alias.
          $path['pid'] = $existing_alias['pid'];
          break;
      }
    }

    // Save the path array.
    \Drupal::service('path.alias_storage')->save($path['source'], $path['alias'], $path['language'], $path['pid']);

    if ($verbose) {
      if (!empty($existing_alias['pid'])) {
        _pathauto_verbose(
          t(
            'Created new alias %alias for %source, replacing %old_alias.',
            array(
              '%alias' => $path['alias'],
              '%source' => $path['source'],
              '%old_alias' => $existing_alias['alias'],
            )
          )
        );
      }
      else {
        _pathauto_verbose(t('Created new alias %alias for %source.', array('%alias' => $path['alias'], '%source' => $path['source'])));
      }
    }

    return $path;
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
