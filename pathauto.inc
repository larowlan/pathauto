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
 * Fetches an existing URL alias given a path and optional language.
 *
 * @param $source
 *   An internal Drupal path.
 * @param $language
 *   An optional language code to look up the path in.
 * @return
 *   FALSE if no alias was found or an associative array containing the
 *   following keys:
 *   - pid: Unique path alias identifier.
 *   - alias: The URL alias.
 */
function _pathauto_existing_alias_data($source, $language = '') {
  return db_fetch_array(db_query_range("SELECT pid, dst AS alias FROM {url_alias} WHERE src = '%s' AND language IN ('%s', '') ORDER BY language DESC, pid DESC", $source, $language, 0, 1));
}

/**
 * Clean up a string segment to be used in an URL alias.
 *
 * Peforms the following possible alterations:
 * - Process the string through the transliteration module.
 * - Replace or remove punctuation with the separator character.
 * - Remove back-slashes.
 * - Replace non-ascii and non-numeric characters with the separator.
 * - Remove common words.
 * - Replace whitespace with the separator character.
 * - Trim duplicate, leading, and trailing separators.
 * - Convert to lower-case.
 * - Shorten to a desired length and logical position based on word boundries.
 *
 * This function should *not* be called on URL alias or path strings because it
 * is assumed that they are already clean.
 *
 * @param $string
 *   A string to clean.
 * @return
 *   The cleaned string.
 */
function pathauto_cleanstring($string) {
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

  // Reduce strings to letters and numbers
  if (variable_get('pathauto_reduce_ascii', FALSE)) {
    $pattern = '/[^a-zA-Z0-9\/]+/';
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
  $output = _pathauto_clean_separators($output);

  // Optionally convert to lower case.
  if (variable_get('pathauto_case', PATHAUTO_CASE_LOWER)) {
    $output = drupal_strtolower($output);
  }

  // Shorten to a logical place based on word boundries.
  $maxlength = min(variable_get('pathauto_max_component_length', 100), _pathauto_get_schema_alias_maxlength());
  $output = pathauto_truncate_utf8($output, $maxlength, TRUE);

  return $output;
}

/**
 * Trims duplicate, leading, and trailing separators from a string.
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

  if (!isset($separator)) {
    $separator = variable_get('pathauto_separator', '-');
  }

  // Clean duplicate or trailing separators.
  if (strlen($separator)) {
    // Escape the separator.
    $seppattern = preg_quote($separator, '/');

    // Trim any leading or trailing separators.
    $output = preg_replace("/^$seppattern+|$seppattern+$/", '', $output);

    // Replace trailing separators around slashes.
    if ($separator !== '/') {
      $output = preg_replace("/$seppattern+\/|\/$seppattern+/", "/", $output);
    }

    // Replace multiple separators with a single one.
    $output = preg_replace("/$seppattern+/", $separator, $output);
  }

  return $output;
}

/**
 * Clean up an URL alias.
 *
 * Peforms the following alterations:
 * - Trim duplicate, leading, and trailing back-slashes.
 * - Trim duplicate, leading, and trailing separators.
 * - Shorten to a desired length and logical position based on word boundries.
 *
 * @param $alias
 *   A string with the URL alias to clean up.
 * @return
 *   The cleaned URL alias.
 */
function pathauto_clean_alias($alias) {
  $output = $alias;

  // Trim duplicate, leading, and trailing back-slashes.
  $output = _pathauto_clean_separators($output, '/');

  // Trim duplicate, leading, and trailing separators.
  $output = _pathauto_clean_separators($output);

  // Shorten to a logical place based on word boundries.
  $maxlength = min(variable_get('pathauto_max_length', 100), _pathauto_get_schema_alias_maxlength());
  $output = pathauto_truncate_utf8($output, $maxlength, TRUE);

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
 * @param $source
 *   An internal Drupal path to be aliased.
 * @param $data
 *   An array of keyed objects to pass to token_replace(). For simple
 *   replacement scenarios 'node', 'user', and others are common keys, with an
 *   accompanying node or user object being the value.  Only one key/value pair
 *   is supported, otherwise you may pass the results from
 *   pathauto_get_placeholders() here.
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
 *
 * @see _pathauto_set_alias()
 * @see pathauto_get_placeholders()
 */
function pathauto_create_alias($module, $op, $source, $data, $entity_id, $type = NULL, $language = '') {
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

  // Support for when $source and $placeholders were swapped.
  if (is_array($source) && is_string($data)) {
    $placeholders = $source;
    $source = $data;
  }
  elseif (is_array($data) && !isset($data['tokens']) && !isset($data['values'])) {
    $placeholders = pathauto_get_placeholders(key($data), current($data));
  }
  else {
    $placeholders = $data;
  }

  // Special handling when updating an item which is already aliased.
  $existing_alias = NULL;
  if ($op == 'update' || $op == 'bulkupdate') {
    if ($existing_alias = _pathauto_existing_alias_data($source, $language)) {
      switch (variable_get('pathauto_update_action', PATHAUTO_UPDATE_ACTION_DELETE)) {
        case PATHAUTO_UPDATE_ACTION_NO_NEW:
          // If an alias already exists, and the update action is set to do nothing,
          // then gosh-darn it, do nothing.
          return '';
      }
    }
  }

  // Replace the placeholders with the values provided by the module.
  $alias = str_replace($placeholders['tokens'], $placeholders['values'], $pattern);

  // Check if the token replacement has not actually replaced any values. If
  // that is the case, then stop because we should not generate an alias.
  // @see token_scan()
  $pattern_tokens_removed = preg_replace('/\[([^\s]+?)\]/', '', $pattern);
  if ($alias === $pattern_tokens_removed) {
    return '';
  }

  $alias = pathauto_clean_alias($alias);

  // Allow other modules to alter the alias.
  $context = array(
    'module' => $module,
    'op' => $op,
    'source' => $source,
    'entity_id' => $entity_id,
    'type' => $type,
    'language' => $language,
    'pattern' => $pattern,
  );
  drupal_alter('pathauto_alias', $alias, $context);

  // If we have arrived at an empty string, discontinue.
  if (!drupal_strlen($alias)) {
    return '';
  }

  // If the alias already exists, generate a new, hopefully unique, variant
  if (_pathauto_alias_exists($alias, $source, $language)) {
    $maxlength = min(variable_get('pathauto_max_length', 100), _pathauto_get_schema_alias_maxlength());
    $separator = variable_get('pathauto_separator', '-');
    $original_alias = $alias;

    $i = 0;
    do {
      // Append an incrementing numeric suffix until we find a unique alias.
      $unique_suffix = $separator . $i;
      $alias = pathauto_truncate_utf8($original_alias, $maxlength - drupal_strlen($unique_suffix, TRUE)) . $unique_suffix;
      $i++;
    } while (_pathauto_alias_exists($alias, $source, $language));

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

  // Build the new path alias array and send it off to be created.
  $path = array(
    'source' => $source,
    'alias' => $alias,
    'language' => $language,
  );
  _pathauto_set_alias($path, $existing_alias, $verbose);

  // Also create a related feed alias if requested and supported.
  $feedappend = trim(variable_get('pathauto_' . $module . '_applytofeeds', ''));
  if (drupal_strlen($feedappend)) {
    // For forums and taxonomies, the source doesn't always form the base of the rss feed (ie. image galleries)
    if ($module == 'taxonomy' || $module == 'forum' && !empty($entity_id)) {
      $source = "taxonomy/term/{$entity_id}";
    }

    // Build the feed path alias array and send it off to be created.
    $path = array(
      'source' => "$source/$feedappend",
      'alias' => "$alias/feed",
      'language' => $language,
    );
    $existing_alias = _pathauto_existing_alias_data($path['source'], $path['language']);
    _pathauto_set_alias($path, $existing_alias, $verbose);
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
 * @param $path
 *   An associative array containing the following keys:
 *   - source: The internal system path.
 *   - alias: The URL alias.
 *   - pid: (optional) Unique path alias identifier.
 *   - language: (optional) The language of the alias.
 * @param $existing_alias
 *   (optional) An associative array of the existing path alias.
 * @param $verbose
 *   If the admin has enabled verbose, should be TRUE. Else FALSE or NULL.
 * @return
 *   TRUE if the path was saved, or NULL otherwise.
 *
 * @see path_set_alias()
 */
function _pathauto_set_alias($path, $existing_alias = NULL, $verbose = FALSE) {
  // Alert users that an existing callback cannot be overridden automatically
  if (_pathauto_path_is_callback($path['alias'])) {
    if ($verbose && user_access('notify of path changes')) {
      drupal_set_message(t('Ignoring alias %alias due to existing path conflict.', array('%alias' => $path['alias'])));
    }
    return;
  }
  // Alert users if they are trying to create an alias that is the same as the internal path
  if ($path['source'] == $path['alias']) {
    if ($verbose && user_access('notify of path changes')) {
      drupal_set_message(t('Ignoring alias %alias because it is the same as the internal path.', array('%alias' => $path['alias'])));
    }
    return;
  }

  $path += array(
    'pid' => NULL,
    'language' => '',
  );

  // Skip replacing the current alias with an identical alias
  if (empty($existing_alias) || $existing_alias['alias'] != $path['alias']) {
    // If there is already an alias, respect some update actions.
    if (!empty($existing_alias)) {
      switch (variable_get('pathauto_update_action', PATHAUTO_UPDATE_ACTION_DELETE)) {
        case PATHAUTO_UPDATE_ACTION_NO_NEW:
          // Do not create the alias.
          return;
        case PATHAUTO_UPDATE_ACTION_LEAVE:
          // Create a new alias instead of overwriting the existing by leaving
          // $path['pid'] empty.
          break;
        case PATHAUTO_UPDATE_ACTION_REDIRECT:
          // Create a redirect
          if (module_exists('path_redirect') && function_exists('path_redirect_save')) {
            $redirect = array(
              'source' => $existing_alias['alias'],
              'redirect' => $path['source'],
            );
            path_redirect_save($redirect);
          }
          // Intentionally fall through to the next condition since we still
          // want to replace the existing alias.
        case PATHAUTO_UPDATE_ACTION_DELETE:
          // Both the redirect and delete actions should overwrite the existing
          // alias.
          $path['pid'] = $existing_alias['pid'];
          break;
      }
    }

    // Save the path array.
    path_set_alias($path['source'], $path['alias'], $path['pid'], $path['language']);

    if ($verbose && user_access('notify of path changes')) {
      if (!empty($redirect)) {
        drupal_set_message(t('Created new alias %alias for %source, replacing %old_alias. %old_alias now redirects to %alias.', array('%alias' => $path['alias'], '%source' => $path['source'], '%old_alias' => $existing_alias['alias'])));
      }
      elseif (!empty($existing_alias['pid'])) {
        drupal_set_message(t('Created new alias %alias for %source, replacing %old_alias.', array('%alias' => $path['alias'], '%source' => $path['source'], '%old_alias' => $existing_alias['pid'])));
      }
      else {
        drupal_set_message(t('Created new alias %alias for %source.', array('%alias' => $path['alias'], '%source' => $path['source'])));
      }
    }

    return TRUE;
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
  $replacements = array();
  foreach ($tokens as $index => $token) {
    $replacements[$token] = $full->values[$index];
  }
  pathauto_clean_token_values($replacements);
  return array('tokens' => array_keys($replacements), 'values' => array_values($replacements));
}

/**
 * Clean tokens so they are URL friendly.
 *
 * @param $replacements
 *   An array of token replacements that need to be "cleaned" for use in the URL.
 */
function pathauto_clean_token_values(&$replacements) {
  foreach ($replacements as $token => $value) {
    // Only clean non-path tokens.
    if (!preg_match('/(path|alias|url|url-brief)(-raw)?\]$/', $token)) {
      $replacements[$token] = pathauto_cleanstring($value);
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
    $punctuation['slash']              = array('value' => '\/', 'name' => t('Slash /'));
    $punctuation['back_slash']         = array('value' => '\\', 'name' => t('Backslash \\'));
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
