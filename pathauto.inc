<?php
// $Id$


/**
 * Check to see if there is already an alias pointing to a different item
 * 
 * @param string $alias
 *   A string alias (i.e. dst)
 * @param string $src
 *   A string that is the internal path
 * 
 *
 */
function _pathauto_alias_exists($alias, $src) {
  return db_result(db_query(
    "SELECT COUNT(dst) FROM {url_alias} WHERE dst = '%s' AND src != '%s'",
    $alias, $src));
}

/**
 * Clean up a string value provided by a module, resulting in a
 * string containing only alphanumerics and separators
 * @param $string
 *   A string to clean.
 * 
 */
function pathauto_cleanstring($string) {
  // Default words to ignore
  $ignore_words = array(
    "a", "an", "as", "at", "before", "but", "by", "for", "from",
    "is", "in", "into", "like", "of", "off", "on", "onto", "per",
    "since", "than", "the", "this", "that", "to", "up", "via",
    "with"
  );

  // Replace or drop apostrophes based on user settings
  $separator = variable_get('pathauto_separator', '-');
  $quotes = variable_get('pathauto_quotes', 0);
  $output = str_replace("'", ($quotes ? $separator : ''), $string);
  
  // Get rid of words that are on the ignore list
  $ignore_re = "\b". preg_replace("/,/", "\b|\b", variable_get('pathauto_ignore_words', $ignore_words)) ."\b";
  $output = preg_replace("/$ignore_re/ie", "", $output);

  // Optionally remove accents and transliterate to a subset of ASCII-96 characters.
  if (variable_get('pathauto_transliterate', FALSE)) { 

    static $i18n_loaded = false;
    static $translations = array();
    if (!$i18n_loaded) {
      $path = drupal_get_path('module', 'pathauto');
      if (is_file($path. '/i18n-ascii.txt')) {
        $translations = parse_ini_file($path. '/i18n-ascii.txt');
      }
      $i18n_loaded = true;
    }
  
    $output = strtr($output, $translations);
  
    // Preserve alphanumerics, everything else becomes a separator.
    $pattern = '/[^a-zA-Z0-9]+/ ';
    $output = preg_replace($pattern, $separator, $output);
  }

  // Always replace whitespace with the separator.
  $output = preg_replace("/\s+/", $separator, $output);

  // In preparation for pattern matching, 
  // escape the separator if and only if it is not alphanumeric)
  if (isset($separator)) {
    if (preg_match('/^(\pL|\pN)+$/D', $separator)) {
      $seppattern = $separator;
    } 
    else {
      $seppattern = '\\'. $separator;
    }
    // Trim any leading or trailing separators (note the need to
    $output = preg_replace("/^$seppattern+|$seppattern+$/", "", $output);

    // Replace multiple separators with a single one
    $output = preg_replace("/$seppattern+/", "$separator", $output);

  }


  // Enforce the maximum component length
  $maxlength = min(variable_get('pathauto_max_component_length', 100), 128);
  $output = drupal_substr($output, 0, $maxlength);

  return $output;
}

/**
 * Apply patterns to create an alias
 *
 * @param $module
 *   The name of your module (e.g., 'node')
 * @param $op
 *   Operation being performed on the content being aliased ('insert',
 *   'update', or 'bulkupdate')
 * @param $placeholders
 *   An array whose keys consist of the translated placeholders
 *   which appear in patterns (e.g., t('[title]')) and values are the
 *   actual values to be substituted into the pattern (e.g., $node->title)
 * @param $src
 *   The "real" URI of the content to be aliased (e.g., "node/$node->nid")
 * @param $type
 *   For modules which provided patternitems in hook_pathauto(),
 *   the relevant identifier for the specific item to be aliased (e.g.,
 *   $node->type)
 * @return
 *   The alias that was created
 */
function pathauto_create_alias($module, $op, $placeholders, $src, $type = NULL) {
  if (($op != 'bulkupdate') and variable_get('pathauto_verbose', FALSE)) {
    $verbose = TRUE;
  } 
  else {
    $verbose = FALSE;
  }

  // Retrieve and apply the pattern for this content type
  $pattern = '';
  if ($type) {
    $pattern = drupal_strtolower(variable_get('pathauto_'. $module .'_'. $type .'_pattern', ''));
  }
  if (!trim($pattern)) {
    $pattern = drupal_strtolower(variable_get('pathauto_'. $module .'_pattern', ''));
  }
  
  // No pattern? Do nothing (otherwise we may blow away existing aliases...)
  if (!trim($pattern)) {
    return '';
  }

  // Special handling when updating an item which is already aliased.
  $pid = NULL;
  if ($op == 'update' or $op == 'bulkupdate') {
    $result = db_query("SELECT pid, dst FROM {url_alias} WHERE src='%s'", $src);
    if ($data = db_fetch_object($result)) {
      // The item is already aliased, check what to do...
      switch (variable_get('pathauto_update_action', 2)) {
        // Do nothing
        case 0:
          return '';
        // Add new alias in addition to old one
        case 1:
          $oldalias = $data->dst;
          break;
        // Replace old alias - remember the pid to update
        case 2:
          $pid = $data->pid;
          $oldalias = $data->dst;
          break;
        default:
          break;
      }
    }
  }

  // Replace the placeholders with the values provided by the module,
  // and lower-case the result
  
  $alias = str_replace($placeholders['tokens'], $placeholders['values'], $pattern);
  $alias = drupal_strtolower($alias);
  
  // Two or more slashes should be collapsed into one
  $alias = preg_replace("/\/+/", "/", $alias);
  
  // Trim any leading or trailing slashes
  $alias = preg_replace("/^\/|\/+$/", "", $alias);

  $maxlength = min(variable_get('pathauto_max_length', 100), 128);
  $alias = drupal_substr($alias, 0, $maxlength);

  // If the alias already exists, generate a new variant
  $separator = variable_get('pathauto_separator', '-');
  if (_pathauto_alias_exists($alias, $src)) {
    for ($i = 0; _pathauto_alias_exists($alias . $separator . $i, $src); $i++) {
    }
    // Make room for the sequence number
    $alias = drupal_substr($alias, 0, $maxlength - 1 -($i/10 + 1));
    $alias = $alias . $separator . $i;
  }

  // If $pid is NULL, a new alias is created - otherwise, the existing
  // alias for the designated src is replaced
  _pathauto_set_alias($src, $alias, $pid, $verbose, $oldalias);

  // Also create a related feed alias if requested, and if supported
  // by the module
  if (variable_get('pathauto_'. $module .'_applytofeeds', FALSE)) {
    $feedappend = variable_get('pathauto_'. $module .'_supportsfeeds', '');
    // Handle replace case (get existing pid)
    _pathauto_set_alias("$src/$feedappend", "$alias/feed", NULL, $verbose);
  }

  return $alias;
}

/**
 * Verifies if the given path is a valid menu callback.
 * Taken from menu_execute_active_handler().
 *
 * @param $path
 *   A string containing a relative path.
 *
 * @return
 *   TRUE if the path already exists.
 */
function _pathauto_path_is_callback($path) {
  static $menu = NULL;
  if (is_null($menu)) {
    $menu = menu_get_menu();
  }
  
  // Determine the menu item containing the callback.
  return isset($menu["callbacks"][$path]);
}

/**
 * Private function for pathauto to create an alias
 * 
 * @param $src
 *   The internal path 
 * @param $dst
 *   The visible externally used path. 
 * @param $pid
 *   If the item is currently aliased, the pid for that item.
 * @param $verbose
 *   If the admin has enabled verbose, should be TRUE.  Else FALSE or NULL.
 * @param $oldalias
 *   If the item is currently aliased, the existing alias for that item.
 * 
 */
function _pathauto_set_alias($src, $dst, $pid = NULL, $verbose = FALSE, $oldalias = NULL) {
  // Alert users that an existing callback cannot be overridden automatically
  if (_pathauto_path_is_callback($dst)) {
    if ($verbose and user_access('create url aliases')) {
      drupal_set_message(t('Ignoring alias '). $dst .t(' due to existing path conflict'));
    }
    return;
  }

  // Skip replacing the current alias with an identical alias
  if ($oldalias != $dst) {
    path_set_alias($src, $dst, $pid, 1, 10);
    if ($verbose and user_access('create url aliases')) {
      if ($pid) {
        drupal_set_message(t('Created new alias %dst for %src, replacing %oldalias', array('%dst' => $dst, '%src' => $src, '%oldalias' => $oldalias)));
      } 
      else {
        drupal_set_message(t('Created new alias %dst for %src', array('%dst' => $dst, '%src' => $src)));
      }
    }
  }
}

/**
 * Generalized function to get tokens across all Pathauto types
 * 
 * @param $object
 *   A user, node, or category object.
 * 
 * @return
 *   tokens for that object formatted in the way that Pathauto expects to see them.
 *  
 */
function pathauto_get_placeholders($type, $object) {
  $full = token_get_values($type, $object, TRUE);
  $tokens = token_prepare_tokens($full->tokens);
  $values = pathauto_clean_token_values($full->values);
  
  return array('tokens' => $tokens, 'values' => $values);
}

/**
 * Cleans tokens so they are URL friendly
 * 
 * @param $values
 *   An array of token values that need to be "cleaned" for use in the URL 
 * 
 */
function pathauto_clean_token_values($values) {
  foreach ($values as $key => $value) {
    $values[$key] = pathauto_cleanstring($value);
  }
  return $values;
}
