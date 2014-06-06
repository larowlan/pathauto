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
 */
function pathauto_alias_uniquify(&$alias, $source, $langcode) {
  $config = \Drupal::configFactory()->get('pathauto.settings');

  if (!pathauto_is_alias_reserved($alias, $source, $langcode)) {
    return;
  }

  // If the alias already exists, generate a new, hopefully unique, variant.
  $maxlength = min($config->get('max_length'), _pathauto_get_schema_alias_maxlength());
  $separator = $config->get('separator');
  $original_alias = $alias;

  $i = 0;
  do {
    // Append an incrementing numeric suffix until we find a unique alias.
    $unique_suffix = $separator . $i;
    $alias = truncate_utf8($original_alias, $maxlength - Unicode::strlen($unique_suffix, TRUE)) . $unique_suffix;
    $i++;
  } while (pathauto_is_alias_reserved($alias, $source, $langcode));
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
  // We need to use a try/catch here because of a core bug which will throw an
  // exception if $path is something like 'node/foo/bar'.
  // @todo Remove when http://drupal.org/node/1003788 is fixed in core.
  /*try {
    $menu = menu_get_item($path);
  }
  catch (Exception $e) {
    return FALSE;
  }*/

  /*try {
    $result = \Drupal::service('router')->match($path);
    $result;
  }
  catch (ResourceNotFoundException $e) {
    //throw new MatchingRouteNotFoundException(sprintf('No matching route could be found for the path "%s"', $path), 0, $e);
    return FALSE;
  }*/

  /*if (isset($menu['path']) && $menu['path'] == $path) {
    return TRUE;
  }
  else*/
  if (is_file(DRUPAL_ROOT . '/' . $path) || is_dir(DRUPAL_ROOT . '/' . $path)) {
    // Do not allow existing files or directories to get assigned an automatic
    // alias. Note that we do not need to use is_link() to check for symbolic
    // links since this returns TRUE for either is_file() or is_dir() already.
    return TRUE;
  }
  return FALSE;
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
    Drupal::service('path.alias_storage')->save($path['source'], $path['alias'], $path['language'], $path['pid']);

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

/**
 * Return an array of arrays for punctuation values.
 *
 * Returns an array of arrays for punctuation values keyed by a name, including
 * the value and a textual description.
 * Can and should be expanded to include "all" non text punctuation values.
 *
 * @return array
 *   An array of arrays for punctuation values keyed by a name, including the
 *   value and a textual description.
 */
function pathauto_punctuation_chars() {
  $punctuation = &drupal_static(__FUNCTION__);

  if (!isset($punctuation)) {

    if (isset($GLOBALS['language'])) {
      $lang = $GLOBALS['language']->language;
    }
    else {
      $lang = Language::LANGCODE_NOT_SPECIFIED;
    }

    $cid = 'pathauto:punctuation:' . $lang;
    if ($cache = \Drupal::cache()->get($cid)) {
      $punctuation = $cache->data;
    }
    else {
      $punctuation = array();
      $punctuation['double_quotes']      = array('value' => '"', 'name' => t('Double quotation marks'));
      $punctuation['quotes']             = array('value' => '\'', 'name' => t("Single quotation marks (apostrophe)"));
      $punctuation['backtick']           = array('value' => '`', 'name' => t('Back tick'));
      $punctuation['comma']              = array('value' => ',', 'name' => t('Comma'));
      $punctuation['period']             = array('value' => '.', 'name' => t('Period'));
      $punctuation['hyphen']             = array('value' => '-', 'name' => t('Hyphen'));
      $punctuation['underscore']         = array('value' => '_', 'name' => t('Underscore'));
      $punctuation['colon']              = array('value' => ':', 'name' => t('Colon'));
      $punctuation['semicolon']          = array('value' => ';', 'name' => t('Semicolon'));
      $punctuation['pipe']               = array('value' => '|', 'name' => t('Vertical bar (pipe)'));
      $punctuation['left_curly']         = array('value' => '{', 'name' => t('Left curly bracket'));
      $punctuation['left_square']        = array('value' => '[', 'name' => t('Left square bracket'));
      $punctuation['right_curly']        = array('value' => '}', 'name' => t('Right curly bracket'));
      $punctuation['right_square']       = array('value' => ']', 'name' => t('Right square bracket'));
      $punctuation['plus']               = array('value' => '+', 'name' => t('Plus sign'));
      $punctuation['equal']              = array('value' => '=', 'name' => t('Equal sign'));
      $punctuation['asterisk']           = array('value' => '*', 'name' => t('Asterisk'));
      $punctuation['ampersand']          = array('value' => '&', 'name' => t('Ampersand'));
      $punctuation['percent']            = array('value' => '%', 'name' => t('Percent sign'));
      $punctuation['caret']              = array('value' => '^', 'name' => t('Caret'));
      $punctuation['dollar']             = array('value' => '$', 'name' => t('Dollar sign'));
      $punctuation['hash']               = array('value' => '#', 'name' => t('Number sign (pound sign, hash)'));
      $punctuation['at']                 = array('value' => '@', 'name' => t('At sign'));
      $punctuation['exclamation']        = array('value' => '!', 'name' => t('Exclamation mark'));
      $punctuation['tilde']              = array('value' => '~', 'name' => t('Tilde'));
      $punctuation['left_parenthesis']   = array('value' => '(', 'name' => t('Left parenthesis'));
      $punctuation['right_parenthesis']  = array('value' => ')', 'name' => t('Right parenthesis'));
      $punctuation['question_mark']      = array('value' => '?', 'name' => t('Question mark'));
      $punctuation['less_than']          = array('value' => '<', 'name' => t('Less-than sign'));
      $punctuation['greater_than']       = array('value' => '>', 'name' => t('Greater-than sign'));
      $punctuation['slash']              = array('value' => '/', 'name' => t('Slash'));
      $punctuation['back_slash']         = array('value' => '\\', 'name' => t('Backslash'));

      // Allow modules to alter the punctuation list and cache the result.
      \Drupal::moduleHandler()->alter('pathauto_punctuation_chars', $punctuation);
      \Drupal::cache()->set($cid, $punctuation);
    }
  }

  return $punctuation;
}

/**
 * Fetch the maximum length of the {url_alias}.alias field from the schema.
 *
 * @return int
 *   An integer of the maximum URL alias length allowed by the database.
 */
function _pathauto_get_schema_alias_maxlength() {
  $maxlength = &drupal_static(__FUNCTION__);
  if (!isset($maxlength)) {
    $schema = drupal_get_schema('url_alias');
    $maxlength = $schema['fields']['alias']['length'];
  }
  return $maxlength;
}
