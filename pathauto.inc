<?php
// $Id$

/**
 * @file
 * Miscellaneous functions for Pathauto.
 *
 * This also contains some constants giving human readable names to some numeric
 * settings; they're included here as they're only rarely used outside this file
 * anyway. Use _pathauto_include() if the constants need to be available.
 *
 * @ingroup pathauto
 */

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
 * "Create a new alias. Redirect from old alias."
 *
 * This is only available when the Path Redirect module is.
 */
define('PATHAUTO_UPDATE_ACTION_REDIRECT', 3);

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
 * Matches Unicode characters that are word boundaries.
 *
 * @see http://unicode.org/glossary
 *
 * Characters with the following General_category (gc) property values are used
 * as word boundaries. While this does not fully conform to the Word Boundaries
 * algorithm described in http://unicode.org/reports/tr29, as PCRE does not
 * contain the Word_Break property table, this simpler algorithm has to do.
 * - Cc, Cf, Cn, Co, Cs: Other.
 * - Pc, Pd, Pe, Pf, Pi, Po, Ps: Punctuation.
 * - Sc, Sk, Sm, So: Symbols.
 * - Zl, Zp, Zs: Separators.
 *
 * Non-boundary characters include the following General_category (gc) property
 * values:
 * - Ll, Lm, Lo, Lt, Lu: Letters.
 * - Mc, Me, Mn: Combining Marks.
 * - Nd, Nl, No: Numbers.
 *
 * Note that the PCRE property matcher is not used because we wanted to be
 * compatible with Unicode 5.2.0 regardless of the PCRE version used (and any
 * bugs in PCRE property tables).
 */
define('PATHAUTO_PREG_CLASS_UNICODE_WORD_BOUNDARY',
  '\x{0}-\x{2F}\x{3A}-\x{40}\x{5B}-\x{60}\x{7B}-\x{A9}\x{AB}-\x{B1}\x{B4}' .
  '\x{B6}-\x{B8}\x{BB}\x{BF}\x{D7}\x{F7}\x{2C2}-\x{2C5}\x{2D2}-\x{2DF}' .
  '\x{2E5}-\x{2EB}\x{2ED}\x{2EF}-\x{2FF}\x{375}\x{37E}-\x{385}\x{387}\x{3F6}' .
  '\x{482}\x{55A}-\x{55F}\x{589}-\x{58A}\x{5BE}\x{5C0}\x{5C3}\x{5C6}' .
  '\x{5F3}-\x{60F}\x{61B}-\x{61F}\x{66A}-\x{66D}\x{6D4}\x{6DD}\x{6E9}' .
  '\x{6FD}-\x{6FE}\x{700}-\x{70F}\x{7F6}-\x{7F9}\x{830}-\x{83E}' .
  '\x{964}-\x{965}\x{970}\x{9F2}-\x{9F3}\x{9FA}-\x{9FB}\x{AF1}\x{B70}' .
  '\x{BF3}-\x{BFA}\x{C7F}\x{CF1}-\x{CF2}\x{D79}\x{DF4}\x{E3F}\x{E4F}' .
  '\x{E5A}-\x{E5B}\x{F01}-\x{F17}\x{F1A}-\x{F1F}\x{F34}\x{F36}\x{F38}' .
  '\x{F3A}-\x{F3D}\x{F85}\x{FBE}-\x{FC5}\x{FC7}-\x{FD8}\x{104A}-\x{104F}' .
  '\x{109E}-\x{109F}\x{10FB}\x{1360}-\x{1368}\x{1390}-\x{1399}\x{1400}' .
  '\x{166D}-\x{166E}\x{1680}\x{169B}-\x{169C}\x{16EB}-\x{16ED}' .
  '\x{1735}-\x{1736}\x{17B4}-\x{17B5}\x{17D4}-\x{17D6}\x{17D8}-\x{17DB}' .
  '\x{1800}-\x{180A}\x{180E}\x{1940}-\x{1945}\x{19DE}-\x{19FF}' .
  '\x{1A1E}-\x{1A1F}\x{1AA0}-\x{1AA6}\x{1AA8}-\x{1AAD}\x{1B5A}-\x{1B6A}' .
  '\x{1B74}-\x{1B7C}\x{1C3B}-\x{1C3F}\x{1C7E}-\x{1C7F}\x{1CD3}\x{1FBD}' .
  '\x{1FBF}-\x{1FC1}\x{1FCD}-\x{1FCF}\x{1FDD}-\x{1FDF}\x{1FED}-\x{1FEF}' .
  '\x{1FFD}-\x{206F}\x{207A}-\x{207E}\x{208A}-\x{208E}\x{20A0}-\x{20B8}' .
  '\x{2100}-\x{2101}\x{2103}-\x{2106}\x{2108}-\x{2109}\x{2114}' .
  '\x{2116}-\x{2118}\x{211E}-\x{2123}\x{2125}\x{2127}\x{2129}\x{212E}' .
  '\x{213A}-\x{213B}\x{2140}-\x{2144}\x{214A}-\x{214D}\x{214F}' .
  '\x{2190}-\x{244A}\x{249C}-\x{24E9}\x{2500}-\x{2775}\x{2794}-\x{2B59}' .
  '\x{2CE5}-\x{2CEA}\x{2CF9}-\x{2CFC}\x{2CFE}-\x{2CFF}\x{2E00}-\x{2E2E}' .
  '\x{2E30}-\x{3004}\x{3008}-\x{3020}\x{3030}\x{3036}-\x{3037}' .
  '\x{303D}-\x{303F}\x{309B}-\x{309C}\x{30A0}\x{30FB}\x{3190}-\x{3191}' .
  '\x{3196}-\x{319F}\x{31C0}-\x{31E3}\x{3200}-\x{321E}\x{322A}-\x{3250}' .
  '\x{3260}-\x{327F}\x{328A}-\x{32B0}\x{32C0}-\x{33FF}\x{4DC0}-\x{4DFF}' .
  '\x{A490}-\x{A4C6}\x{A4FE}-\x{A4FF}\x{A60D}-\x{A60F}\x{A673}\x{A67E}' .
  '\x{A6F2}-\x{A716}\x{A720}-\x{A721}\x{A789}-\x{A78A}\x{A828}-\x{A82B}' .
  '\x{A836}-\x{A839}\x{A874}-\x{A877}\x{A8CE}-\x{A8CF}\x{A8F8}-\x{A8FA}' .
  '\x{A92E}-\x{A92F}\x{A95F}\x{A9C1}-\x{A9CD}\x{A9DE}-\x{A9DF}' .
  '\x{AA5C}-\x{AA5F}\x{AA77}-\x{AA79}\x{AADE}-\x{AADF}\x{ABEB}' .
  '\x{D800}-\x{F8FF}\x{FB29}\x{FD3E}-\x{FD3F}\x{FDFC}-\x{FDFD}' .
  '\x{FE10}-\x{FE19}\x{FE30}-\x{FE6B}\x{FEFF}-\x{FF0F}\x{FF1A}-\x{FF20}' .
  '\x{FF3B}-\x{FF40}\x{FF5B}-\x{FF65}\x{FFE0}-\x{FFFD}');

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
  $alias_pid = db_result(db_query_range("SELECT pid FROM {url_alias} WHERE dst = '%s' AND src <> '%s' AND language = '%s'", array($alias, $source, $language), 0, 1));
  if (module_exists('path_redirect') && function_exists('path_redirect_delete_multiple')) {
    // Delete from path_redirect the exact same alias to the same node.
    path_redirect_delete_multiple(NULL, array('source' => $alias, 'redirect' => $source));

    // If there still is this alias used in path_redirect, then create a different alias.
    $redirects = path_redirect_load_multiple(NULL, array('source' => $alias));
  }
  if ($alias_pid || !empty($redirects)) {
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
    switch (variable_get('pathauto_update_action', PATHAUTO_UPDATE_ACTION_DELETE)) {
      case PATHAUTO_UPDATE_ACTION_DELETE:
      case PATHAUTO_UPDATE_ACTION_REDIRECT:
        // Replace old alias - remember the pid to update
        $output['pid'] = $existing->pid;
      case PATHAUTO_UPDATE_ACTION_LEAVE:
        // Add new alias in addition to old one
        $output['old_alias'] = $existing->dst;
        break;
      case PATHAUTO_UPDATE_ACTION_NO_NEW:
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

  // Optionally transliterate (by running through the Transliteration module)
  if (variable_get('pathauto_transliterate', FALSE)) {
    if (module_exists('transliteration')) {
      $output = transliteration_get($output);
    }
    else {
      drupal_set_message(t('Pathauto could not transliterate the path, as the Transliteration module has been disabled.'), 'error');
    }
  }

  // Replace or drop punctuation based on user settings
  $separator = variable_get('pathauto_separator', '-');
  $punctuation = pathauto_punctuation_chars();
  foreach ($punctuation as $name => $details) {
    $action = variable_get('pathauto_punctuation_' . $name, PATHAUTO_PUNCTUATION_REMOVE);
    if ($action != PATHAUTO_PUNCTUATION_DO_NOTHING) {
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
    $ignore_words = variable_get('pathauto_ignore_words', PATHAUTO_IGNORE_WORDS);
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
  $output = pathauto_truncate_utf8($output, $maxlength, TRUE);

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
  if (variable_get('pathauto_case', PATHAUTO_CASE_LOWER)) {
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

  // Retrieve and apply the pattern for this content type.
  $pattern = pathauto_pattern_load_by_entity($module, $type, $language);
  if (empty($pattern)) {
    // No pattern? Do nothing (otherwise we may blow away existing aliases...)
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
    if (variable_get('pathauto_update_action', PATHAUTO_UPDATE_ACTION_DELETE) == PATHAUTO_UPDATE_ACTION_NO_NEW) {
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

  // Shorten to a logical place based on the last separator.
  $separator = variable_get('pathauto_separator', '-');
  $maxlength = min(variable_get('pathauto_max_length', 100), _pathauto_get_schema_alias_maxlength());

  // Make sure we're not ending the alias in the middle of a word.
  $alias = pathauto_truncate_utf8($alias, $maxlength, TRUE);

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

    if (variable_get('pathauto_update_action', PATHAUTO_UPDATE_ACTION_DELETE) == PATHAUTO_UPDATE_ACTION_REDIRECT
        && module_exists('path_redirect')
        && function_exists('path_redirect_save')) {
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
  $full = token_get_values($type, $object, TRUE);
  $tokens = token_prepare_tokens($full->tokens);
  $values = pathauto_clean_token_values($full);
  return array('tokens' => $tokens, 'values' => $values);
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
 * Truncates a UTF-8-encoded string safely to a number of characters.
 *
 * This is a functional backport of Drupal 7's truncate_utf8().
 *
 * @param $string
 *   The string to truncate.
 * @param $max_length
 *   An upper limit on the returned string length, including trailing ellipsis
 *   if $add_ellipsis is TRUE.
 * @param $wordsafe
 *   If TRUE, attempt to truncate on a word boundary. Word boundaries are
 *   spaces, punctuation, and Unicode characters used as word boundaries in
 *   non-Latin languages; see PREG_CLASS_UNICODE_WORD_BOUNDARY for more
 *   information.  If a word boundary cannot be found that would make the length
 *   of the returned string fall within length guidelines (see parameters
 *   $max_return_length and $min_wordsafe_length), word boundaries are ignored.
 * @param $add_ellipsis
 *   If TRUE, add t('...') to the end of the truncated string (defaults to
 *   FALSE). The string length will still fall within $max_return_length.
 * @param $min_wordsafe_length
 *   If $wordsafe is TRUE, the minimum acceptable length for truncation (before
 *   adding an ellipsis, if $add_ellipsis is TRUE). Has no effect if $wordsafe
 *   is FALSE. This can be used to prevent having a very short resulting string
 *   that will not be understandable. For instance, if you are truncating the
 *   string "See myverylongurlexample.com for more information" to a word-safe
 *   return length of 20, the only available word boundary within 20 characters
 *   is after the word "See", which wouldn't leave a very informative string. If
 *   you had set $min_wordsafe_length to 10, though, the function would realise
 *   that "See" alone is too short, and would then just truncate ignoring word
 *   boundaries, giving you "See myverylongurl..." (assuming you had set
 *   $add_ellipses to TRUE).
 *
 * @return
 *   The truncated string.
 */
function pathauto_truncate_utf8($string, $max_length, $wordsafe = FALSE, $add_ellipsis = FALSE, $min_wordsafe_length = 1) {
  $ellipsis = '';
  $max_length = max($max_length, 0);
  $min_wordsafe_length = max($min_wordsafe_length, 0);

  if (drupal_strlen($string) <= $max_length) {
    // No truncation needed, so don't add ellipsis, just return.
    return $string;
  }

  if ($add_ellipsis) {
    // Truncate ellipsis in case $max_length is small.
    $ellipsis = drupal_substr(t('...'), 0, $max_length);
    $max_length -= drupal_strlen($ellipsis);
    $max_length = max($max_length, 0);
  }

  if ($max_length <= $min_wordsafe_length) {
    // Do not attempt word-safe if lengths are bad.
    $wordsafe = FALSE;
  }

  if ($wordsafe) {
    $matches = array();
    // Find the last word boundary, if there is one within $min_wordsafe_length
    // to $max_length characters. preg_match() is always greedy, so it will
    // find the longest string possible.
    $found = preg_match('/^(.{' . $min_wordsafe_length . ',' . $max_length . '})[' . PATHAUTO_PREG_CLASS_UNICODE_WORD_BOUNDARY . ']/u', $string, $matches);
    if ($found) {
      $string = $matches[1];
    }
    else {
      $string = drupal_substr($string, 0, $max_length);
    }
  }
  else {
    $string = drupal_substr($string, 0, $max_length);
  }

  if ($add_ellipsis) {
    $string .= $ellipsis;
  }

  return $string;
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

/**
 * Fetch an array of non-raw tokens that have matching raw tokens.
 *
 * @return
 *   An array of tokens.
 */
function _pathauto_get_raw_tokens() {
  static $raw_tokens;

  if (!isset($raw_tokens)) {
    $raw_tokens = array();

    // Build one big list of tokens.
    foreach (token_get_list('all') as $tokens) {
      $raw_tokens = array_merge($raw_tokens, array_keys($tokens));
    }

    // Filter out any tokens without -raw as a suffix.
    foreach ($raw_tokens as $index => $token) {
      if (substr($token, -4) !== '-raw') {
        unset($raw_tokens[$index]);
      }
    }

    array_unique($raw_tokens);
  }

  return $raw_tokens;
}
