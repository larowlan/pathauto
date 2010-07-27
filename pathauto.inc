<?php
// $Id$

/**
 * @file
 * Miscellaneous functions for Pathauto.
 *
 * @ingroup pathauto
 */

/**
 * Check to see if there is already an alias pointing to a different item.
 *
 * @param $alias
 *   A string alias.
 * @param $source
 *   A string that is the internal path.
 * @param $language
 *   A string indicating the path's language.
 * @return
 *   TRUE if an alias exists, FALSE if not.
 */
function _pathauto_alias_exists($alias, $source, $language = '') {
  $pid = db_result(db_query_range("SELECT pid FROM {url_alias} WHERE src <> '%s' AND dst = '%s' AND language IN ('%s', '') ORDER BY language DESC, pid DESC", $source, $alias, $language, 0, 1));

  if (module_exists('path_redirect') && function_exists('path_redirect_delete_multiple')) {
    // Delete from path_redirect the exact same alias to the same node.
    path_redirect_delete_multiple(NULL, array('source' => $alias, 'redirect' => $source));

    // If there still is this alias used in path_redirect, then create a different alias.
    $redirects = path_redirect_load_multiple(NULL, array('source' => $alias));
  }

  if ($pid || !empty($redirects)) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Returns old alias and pid if there is already an alias
 * pointing to a different item.
 *
 * @param $source
 *   A string that is the internal path.
 * @param $language
 *   A string containing the language code to check against.
 * @return
 *   An array with the keys "pid" and "old_alias" containing
 *   the "pid" and old "alias", respectively, of the old alias.
 */
function _pathauto_existing_alias_data($source, $language = '') {
  $output = array(
    'pid' => '',
    'old_alias' => ''
  );

  $existing = db_fetch_object(db_query_range("SELECT pid, dst FROM {url_alias} WHERE src = '%s' AND language IN ('%s', '') ORDER BY language DESC, pid DESC", $source, $language, 0, 1));
  if ($existing) {
    // The item is already aliased, check what to do...
    switch (variable_get('pathauto_update_action', 2)) {
      case 2:
      case 3:
        // Replace old alias - remember the pid to update
        $output['pid'] = $existing->pid;
      case 1:
        // Add new alias in addition to old one
        $output['old_alias'] = $existing->dst;
        break;
      case 0:
      default:
        // Do nothing
        break;
    }
  }

  return $output;
}

/**
 * Clean up a string value provided by a module.
 *
 * Resulting string contains only alphanumerics and separators.
 *
 * @param $string
 *   A string to clean.
 * @param $clean_slash
 *   Whether to clean slashes from the given string.
 * @return
 *   The cleaned string.
 */
function pathauto_cleanstring($string, $clean_slash = TRUE) {
  $output = $string;

  // Optionally remove accents and transliterate
  if (variable_get('pathauto_transliterate', FALSE)) {
    static $i18n_loaded = false;
    static $translations = array();

    if (!$i18n_loaded) {
      $path = drupal_get_path('module', 'pathauto');
      if (is_file($path .'/i18n-ascii.txt')) {
        $translations = drupal_parse_info_file($path .'/i18n-ascii.txt');
      }
      $i18n_loaded = true;
    }

    $output = strtr($output, $translations);
  }

  // Replace or drop punctuation based on user settings
  $separator = variable_get('pathauto_separator', '-');
  $punctuation = pathauto_punctuation_chars();
  foreach ($punctuation as $name => $details) {
    $action = variable_get('pathauto_punctuation_'. $name, 0);
    // 2 is the action for "do nothing" with the punctuation
    if ($action != 2) {
      // Slightly tricky inline if which either replaces with the separator or nothing
      $output = str_replace($details['value'], ($action ? $separator : ''), $output);
    }
  }

  // If something is already urlsafe then don't remove slashes
  if ($clean_slash) {
    $output = str_replace('/', '', $output);
  }

  // Reduce strings to letters and numbers
  if (variable_get('pathauto_reduce_ascii', FALSE)) {
    $pattern = '/[^a-zA-Z0-9\/]+/ ';
    $output = preg_replace($pattern, $separator, $output);
  }

  // Calculate and statically cache the ignored words regex expression.
  static $ignore_words_regex;
  if (!isset($ignore_words_regex)) {
    $ignore_words = array(
      'a', 'an', 'as', 'at', 'before', 'but', 'by', 'for', 'from', 'is', 'in',
      'into', 'like', 'of', 'off', 'on', 'onto', 'per', 'since', 'than', 'the',
      'this', 'that', 'to', 'up', 'via', 'with',
    );
    $ignore_words = variable_get('pathauto_ignore_words', $ignore_words);
    $ignore_words_regex = preg_replace(array('/^[,\s]+|[,\s]+$/', '/[,\s]+/'), array('', '\b|\b'), $ignore_words);
    if ($ignore_words_regex) {
      $ignore_words_regex = '\b' . $ignore_words_regex . '\b';
    }
  }

  // Get rid of words that are on the ignore list
  if ($ignore_words_regex) {
    if (function_exists('mb_eregi_replace')) {
      $words_removed = mb_eregi_replace($ignore_words_regex, '', $output);
    }
    else {
      $words_removed = preg_replace("/$ignore_words_regex/i", '', $output);
    }
    if (drupal_strlen(trim($words_removed)) > 0) {
      $output = $words_removed;
    }
  }

  // Always replace whitespace with the separator.
  $output = preg_replace('/\s+/', $separator, $output);

  // Trim duplicates and remove trailing and leading separators.
  $output = _pathauto_clean_separators($output, $separator);

  // Enforce the maximum component length
  $maxlength = min(variable_get('pathauto_max_component_length', 100), _pathauto_get_schema_alias_maxlength());
  $output = drupal_substr($output, 0, $maxlength);

  return $output;
}

/**
 * Clean path separators from a given string.
 *
 * Trims duplicates and strips leading and trailing separators.
 *
 * @param $string
 *   The string to clean path separators from.
 * @param $separator
 *   The path separator to use when cleaning.
 * @return
 *   The cleaned version of the string.
 *
 * @see pathauto_cleanstring()
 * @see pathauto_clean_alias()
 */
function _pathauto_clean_separators($string, $separator = NULL) {
  $output = $string;

  // Clean duplicate or trailing separators.
  if (isset($separator) && strlen($separator)) {
    // Escape the separator.
    $seppattern = preg_quote($separator, '/');

    // Trim any leading or trailing separators.
    $output = preg_replace("/^$seppattern+|$seppattern+$/", '', $output);

    // Replace trailing separators around slashes.
    $output = preg_replace("/$seppattern+\/|\/$seppattern+/", "/", $output);

    // Replace multiple separators with a single one.
    $output = preg_replace("/$seppattern+/", $separator, $output);
  }

  // Optionally convert to lower case.
  if (variable_get('pathauto_case', 1)) {
    $output = drupal_strtolower($output);
  }

  return $output;
}

/**
 * Clean up an alias.
 *
 * This strips leading and trailing separators from the alias, and invokes
 * hook_pathauto_clean_alias() for other modules to have their chance.
 *
 * @param $alias
 *   A path alias to clean.
 * @param $clean_slash
 *   Whether to clean slashes from the alias..
 * @return
 *   The cleaned alias.
 */
function pathauto_clean_alias($alias) {
  $output = $alias;
  $separator = variable_get('pathauto_separator', '-');

  // Trim duplicates and remove trailing and leading separators.
  $output = _pathauto_clean_separators($output, $separator);

  // Give other modules a chance to clean this alias.
  foreach (module_implements('pathauto_clean_alias') as $module) {
    $function = $module .'_pathauto_clean_alias';
    $output = $function($output);
  }

  return $output;
}

/**
 * Apply patterns to create an alias.
 *
 * @param $module
 *   The name of your module (e.g., 'node').
 * @param $op
 *   Operation being performed on the content being aliased
 *   ('insert', 'update', 'return', or 'bulkupdate').
 * @param $placeholders
 *   An array whose keys consist of the translated placeholders
 *   which appear in patterns (e.g., t('[title]')) and values are the
 *   actual values to be substituted into the pattern (e.g., $node->title).
 * @param $source
 *   The "real" URI of the content to be aliased (e.g., "node/$node->nid").
 * @param $entity_id
 *   The entity ID (node ID, user ID, etc.).
 * @param $type
 *   For modules which provided pattern items in hook_pathauto(),
 *   the relevant identifier for the specific item to be aliased
 *   (e.g., $node->type).
 * @param $language
 *   A string specify the path's language.
 * @return
 *   The alias that was created.
 */
function pathauto_create_alias($module, $op, $placeholders, $source, $entity_id, $type = NULL, $language = '') {
  if (($op != 'bulkupdate') and variable_get('pathauto_verbose', FALSE) && user_access('notify of path changes')) {
    $verbose = TRUE;
  }
  else {
    $verbose = FALSE;
  }

  // Retrieve and apply the pattern for this content type
  if (!empty($type)) {
    $pattern = trim(variable_get("pathauto_{$module}_{$type}_{$language}_pattern", ''));
    if (empty($pattern)) {
      $pattern = trim(variable_get("pathauto_{$module}_{$type}_pattern", ''));
    }
  }
  if (empty($pattern)) {
    $pattern = trim(variable_get("pathauto_{$module}_pattern", ''));
  }
  // No pattern? Do nothing (otherwise we may blow away existing aliases...)
  if (empty($pattern)) {
    return '';
  }

  if ($module == 'taxonomy') {
    // Get proper path for term.
    $term_path = taxonomy_term_path(taxonomy_get_term($entity_id));
    if ($term_path != $source) {
      // Quietly alias 'taxonomy/term/[tid]' with proper path for term.
      $update_data = _pathauto_existing_alias_data($source, $language);
      _pathauto_set_alias($source, $term_path, $module, $entity_id, $update_data['pid'], FALSE, $update_data['old_alias'], $language);
      // Set $source as proper path.
      $source = $term_path;
    }
  }

  // Special handling when updating an item which is already aliased.
  $pid = NULL;
  $old_alias = NULL;
  if ($op == 'update' or $op == 'bulkupdate') {
    if (variable_get('pathauto_update_action', 2) == 0) {
      // Do nothing
      return '';
    }
    $update_data = _pathauto_existing_alias_data($source, $language);
    $pid = $update_data['pid'];
    $old_alias = $update_data['old_alias'];
  }

  // Replace the placeholders with the values provided by the module.
  $alias = str_replace($placeholders['tokens'], $placeholders['values'], $pattern);

  // Two or more slashes should be collapsed into one
  $alias = preg_replace('/\/+/', '/', $alias);

  // Trim any leading or trailing slashes
  $alias = preg_replace('/^\/|\/+$/', '', $alias);

  // Clean the alias.
  $alias = pathauto_clean_alias($alias);

  // Enforce the maximum length.
  $separator = variable_get('pathauto_separator', '-');
  $maxlength = min(variable_get('pathauto_max_length', 100), _pathauto_get_schema_alias_maxlength());
  $alias = drupal_substr($alias, 0, $maxlength);

  // If the alias already exists, generate a new, hopefully unique, variant
  if (_pathauto_alias_exists($alias, $source, $language)) {
    $original_alias = $alias;
    for ($i = 0; _pathauto_alias_exists(drupal_substr($alias, 0, $maxlength - strlen($i)) . $separator . $i, $source, $language); $i++) {
    }
    // Make room for the sequence number
    $alias = drupal_substr($alias, 0, $maxlength - drupal_strlen($i));
    $alias = $alias . $separator . $i;
    // If verbose is on, alert the user why this happened
    if ($verbose) {
      drupal_set_message(t('The automatically generated alias %original_alias conflicted with an existing alias. Alias changed to %alias.',
        array('%original_alias' => $original_alias, '%alias' => $alias)));
    }
  }

  // Return the generated alias if requested.
  if ($op == 'return') {
    return $alias;
  }

  // If $pid is NULL, a new alias is created - otherwise, the existing
  // alias for the designated source is replaced.
  _pathauto_set_alias($source, $alias, $module, $entity_id, $pid, $verbose, $old_alias, $language);

  // Also create a related feed alias if requested, and if supported
  // by the module
  $feedappend = trim(variable_get('pathauto_' . $module . '_applytofeeds', ''));
  if (drupal_strlen($feedappend)) {
    // If the alias to generate is not empty, then we can append the feed
    // string. Otherwise we want to keep $alias as an empty string so that
    // an existing alias may be removed.
    if (drupal_strlen($alias)) {
      $alias .= '/feed';
    }

    // For forums and taxonomies, the source doesn't always form the base of the rss feed (ie. image galleries)
    if ($module == 'taxonomy' || $module == 'forum') {
      $update_data = _pathauto_existing_alias_data("taxonomy/term/$entity_id/$feedappend", $language);
      _pathauto_set_alias("taxonomy/term/$entity_id/$feedappend", $alias, $module, $entity_id, $update_data['pid'], $verbose, $update_data['old_alias'], $language);
    }
    else {
      $update_data = _pathauto_existing_alias_data("$source/$feedappend", $language);
      _pathauto_set_alias("$source/$feedappend", $alias, $module, $entity_id, $update_data['pid'], $verbose, $update_data['old_alias'], $language);
    }
  }

  return $alias;
}

/**
 * Verify if the given path is a valid menu callback.
 *
 * Taken from menu_execute_active_handler().
 *
 * @param $path
 *   A string containing a relative path.
 * @return
 *   TRUE if the path already exists.
 */
function _pathauto_path_is_callback($path) {
  $menu = menu_get_item($path);
  if (isset($menu['path']) && $menu['path'] == $path) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Private function for Pathauto to create an alias.
 *
 * @param $source
 *   The internal path.
 * @param $alias
 *   The visible externally used path.
 * @param $entity_type
 *   The entity type (node, user, etc.).
 * @param $entity_id
 *   The entity ID (node ID, user ID, etc.).
 * @param $pid
 *   If the item is currently aliased, the pid for that item.
 * @param $verbose
 *   If the admin has enabled verbose, should be TRUE. Else FALSE or NULL.
 * @param $old_alias
 *   If the item is currently aliased, the existing alias for that item.
 * @param $language
 *   The path's language.
 */
function _pathauto_set_alias($source, $alias, $entity_type, $entity_id, $pid = NULL, $verbose = FALSE, $old_alias = NULL, $language = '') {
  // Alert users that an existing callback cannot be overridden automatically
  if (_pathauto_path_is_callback($alias)) {
    if ($verbose && user_access('notify of path changes')) {
      drupal_set_message(t('Ignoring alias %alias due to existing path conflict.', array('%alias' => $alias)));
    }
    return;
  }
  // Alert users if they are trying to create an alias that is the same as the internal path
  if ($source == $alias) {
    if ($verbose && user_access('notify of path changes')) {
      drupal_set_message(t('Ignoring alias %alias because it is the same as the internal path.', array('%alias' => $alias)));
    }
    return;
  }

  // Skip replacing the current alias with an identical alias
  if ($old_alias != $alias) {
    path_set_alias($source, $alias, $pid, $language);

    if (variable_get('pathauto_update_action', 2) == 3 && function_exists('path_redirect_save')) {
      if (!empty($old_alias)) {
        $redirect = array(
          'source' => $old_alias,
          'redirect' => $source,
        );
        path_redirect_save($redirect);
      }
    }
    if ($verbose && user_access('notify of path changes')) {
      if (!empty($redirect)) {
        drupal_set_message(t('Created new alias %alias for %source, replacing %old_alias. %old_alias now redirects to %alias.', array('%alias' => $alias, '%source' => $source, '%old_alias' => $old_alias)));
      }
      elseif ($pid) {
        drupal_set_message(t('Created new alias %alias for %source, replacing %old_alias.', array('%alias' => $alias, '%source' => $source, '%old_alias' => $old_alias)));
      }
      else {
        drupal_set_message(t('Created new alias %alias for %source.', array('%alias' => $alias, '%source' => $source)));
      }
    }
  }
}

/**
 * Generalized function to get tokens across all Pathauto types.
 *
 * @param $object
 *   A user, node, or category object.
 * @return
 *   Tokens for that object formatted in the way that
 *   Pathauto expects to see them.
 */
function pathauto_get_placeholders($type, $object) {
  if (function_exists('token_get_values')) {
    $full = token_get_values($type, $object, TRUE);
    $tokens = token_prepare_tokens($full->tokens);
    $values = pathauto_clean_token_values($full);
    return array('tokens' => $tokens, 'values' => $values);
  }
  // TODO at some point try removing this and see if install profiles have problems again.
  watchdog('Pathauto', 'It appears that you have installed Pathauto, which depends on token, but token is either not installed or not installed properly.');
  return array('tokens' => array(), 'values' => array());
}

/**
 * Clean tokens so they are URL friendly.
 *
 * @param $full
 *   An array of token values that need to be "cleaned" for use in the URL.
 * @return
 *   An array of the cleaned tokens.
 */
function pathauto_clean_token_values($full) {
  foreach ($full->values as $key => $value) {
    // If it's a "path" or "url friendly" token don't remove the "/" character
    if (drupal_substr($full->tokens[$key], -4, 4) === 'path' || drupal_substr($full->tokens[$key], -8, 8) === 'path-raw' || drupal_substr($full->tokens[$key], -5, 5) === 'alias') {
      $full->values[$key] = pathauto_cleanstring($value, FALSE);
    }
    else {
      $full->values[$key] = pathauto_cleanstring($value);
    }
  }
  return $full->values;
}

/**
 * Return an array of arrays for punctuation values.
 *
 * Returns an array of arrays for punctuation values keyed by a name, including
 * the value and a textual description.
 * Can and should be expanded to include "all" non text punctuation values.
 *
 * @return
 *   An array of arrays for punctuation values keyed by a name, including the
 *   value and a textual description.
 */
function pathauto_punctuation_chars() {
  static $punctuation;

  if (!isset($punctuation)) {
    $punctuation = array();
    $punctuation['double_quotes']      = array('value' => '"', 'name' => t('Double quotes "'));
    $punctuation['quotes']             = array('value' => "'", 'name' => t("Single quotes (apostrophe) '"));
    $punctuation['backtick']           = array('value' => '`', 'name' => t('Back tick `'));
    $punctuation['comma']              = array('value' => ',', 'name' => t('Comma ,'));
    $punctuation['period']             = array('value' => '.', 'name' => t('Period .'));
    $punctuation['hyphen']             = array('value' => '-', 'name' => t('Hyphen -'));
    $punctuation['underscore']         = array('value' => '_', 'name' => t('Underscore _'));
    $punctuation['colon']              = array('value' => ':', 'name' => t('Colon :'));
    $punctuation['semicolon']          = array('value' => ';', 'name' => t('Semicolon ;'));
    $punctuation['pipe']               = array('value' => '|', 'name' => t('Pipe |'));
    $punctuation['left_curly']         = array('value' => '{', 'name' => t('Left curly bracket {'));
    $punctuation['left_square']        = array('value' => '[', 'name' => t('Left square bracket ['));
    $punctuation['right_curly']        = array('value' => '}', 'name' => t('Right curly bracket }'));
    $punctuation['right_square']       = array('value' => ']', 'name' => t('Right square bracket ]'));
    $punctuation['plus']               = array('value' => '+', 'name' => t('Plus +'));
    $punctuation['equal']              = array('value' => '=', 'name' => t('Equal ='));
    $punctuation['asterisk']           = array('value' => '*', 'name' => t('Asterisk *'));
    $punctuation['ampersand']          = array('value' => '&', 'name' => t('Ampersand &'));
    $punctuation['percent']            = array('value' => '%', 'name' => t('Percent %'));
    $punctuation['caret']              = array('value' => '^', 'name' => t('Caret ^'));
    $punctuation['dollar']             = array('value' => '$', 'name' => t('Dollar $'));
    $punctuation['hash']               = array('value' => '#', 'name' => t('Hash #'));
    $punctuation['at']                 = array('value' => '@', 'name' => t('At @'));
    $punctuation['exclamation']        = array('value' => '!', 'name' => t('Exclamation !'));
    $punctuation['tilde']              = array('value' => '~', 'name' => t('Tilde ~'));
    $punctuation['left_parenthesis']   = array('value' => '(', 'name' => t('Left parenthesis ('));
    $punctuation['right_parenthesis']  = array('value' => ')', 'name' => t('Right parenthesis )'));
    $punctuation['question_mark']      = array('value' => '?', 'name' => t('Question mark ?'));
    $punctuation['less_than']          = array('value' => '<', 'name' => t('Less than <'));
    $punctuation['greater_than']       = array('value' => '>', 'name' => t('Greater than >'));
    $punctuation['back_slash']         = array('value' => '\\', 'name' => t('Back slash \\'));
  }

  return $punctuation;
}

/**
 * Fetch the maximum length of the {url_alias}.dst field from the schema.
 *
 * @return
 *   An integer of the maximum url alias length allowed by the database.
 */
function _pathauto_get_schema_alias_maxlength() {
  static $maxlength;
  if (!isset($maxlength)) {
    $schema = drupal_get_schema('url_alias');
    $maxlength = $schema['fields']['dst']['length'];
  }
  return $maxlength;
}
