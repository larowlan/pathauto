<?php

/**
 * @file
 * Contains \Drupal\pathauto\PathautoManager
 */

namespace Drupal\pathauto;

use \Drupal\Core\Language\Language;
use \Drupal\Component\Utility\Unicode;

class PathautoManager {

  /**
   * Clean up a string segment to be used in an URL alias.
   *
   * Performs the following possible alterations:
   * - Remove all HTML tags.
   * - Process the string through the transliteration module.
   * - Replace or remove punctuation with the separator character.
   * - Remove back-slashes.
   * - Replace non-ascii and non-numeric characters with the separator.
   * - Remove common words.
   * - Replace whitespace with the separator character.
   * - Trim duplicate, leading, and trailing separators.
   * - Convert to lower-case.
   * - Shorten to a desired length and logical position based on word boundaries.
   *
   * This function should *not* be called on URL alias or path strings
   * because it is assumed that they are already clean.
   *
   * @param string $string
   *   A string to clean.
   * @param array $options
   *   (optional) A keyed array of settings and flags to control the Pathauto
   *   clean string replacement process. Supported options are:
   *   - langcode: A language code to be used when translating strings.
   *
   * @return string
   *   The cleaned string.
   */
  public function cleanString($string, array $options = array()) {
    // Use the advanced drupal_static() pattern, since this is called very often.
    static $drupal_static_fast;
    $config = \Drupal::configFactory()->get('pathauto.settings');
    module_load_include('inc', 'pathauto');

    if (!isset($drupal_static_fast)) {
      $drupal_static_fast['cache'] = &drupal_static(__FUNCTION__);
    }
    $cache = &$drupal_static_fast['cache'];

    // Generate and cache variables used in this function so that on the second
    // call to \Drupal::service('pathauto.manager')->cleanString() we focus on processing.
    if (!isset($cache)) {
      $cache = array(
        'separator' => $config->get('separator'),
        'strings' => array(),
        'transliterate' => $config->get('transliterate') && \Drupal::moduleHandler()->moduleExists('transliteration'),
        'punctuation' => array(),
        'reduce_ascii' => (bool) $config->get('reduce_ascii'),
        'ignore_words_regex' => FALSE,
        'lowercase' => (bool) $config->get('case'),
        'maxlength' => min($config->get('max_component_length'), _pathauto_get_schema_alias_maxlength()),
      );

      // Generate and cache the punctuation replacements for strtr().
      $punctuation = pathauto_punctuation_chars();
      foreach ($punctuation as $name => $details) {
        $action = $config->get('punctuation_' . $name);
        switch ($action) {
          case PATHAUTO_PUNCTUATION_REMOVE:
            $cache['punctuation'][$details['value']] = '';
            break;

          case PATHAUTO_PUNCTUATION_REPLACE:
            $cache['punctuation'][$details['value']] = $cache['separator'];
            break;

          case PATHAUTO_PUNCTUATION_DO_NOTHING:
            // Literally do nothing.
            break;
        }
      }

      // Generate and cache the ignored words regular expression.
      $ignore_words = $config->get('ignore_words');
      $ignore_words_regex = preg_replace(array('/^[,\s]+|[,\s]+$/', '/[,\s]+/'), array('', '\b|\b'), $ignore_words);
      if ($ignore_words_regex) {
        $cache['ignore_words_regex'] = '\b' . $ignore_words_regex . '\b';
        if (function_exists('mb_eregi_replace')) {
          $cache['ignore_words_callback'] = 'mb_eregi_replace';
        }
        else {
          $cache['ignore_words_callback'] = 'preg_replace';
          $cache['ignore_words_regex'] = '/' . $cache['ignore_words_regex'] . '/i';
        }
      }
    }

    // Empty strings do not need any proccessing.
    if ($string === '' || $string === NULL) {
      return '';
    }

    $langcode = NULL;
    if (!empty($options['language']->language)) {
      $langcode = $options['language']->language;
    }
    elseif (!empty($options['langcode'])) {
      $langcode = $options['langcode'];
    }

    // Check if the string has already been processed, and if so return the
    // cached result.
    if (isset($cache['strings'][$langcode][$string])) {
      return $cache['strings'][$langcode][$string];
    }

    // Remove all HTML tags from the string.
    $output = strip_tags(decode_entities($string));

    // Optionally transliterate (by running through the Transliteration module).
    if ($cache['transliterate']) {
      // If the reduce strings to letters and numbers is enabled, don't bother
      // replacing unknown characters with a question mark. Use an empty string
      // instead.
      $output = \Drupal::service('transliteration')->transliterate($output, $cache['reduce_ascii'] ? '' : '?', $langcode);
    }

    // Replace or drop punctuation based on user settings.
    $output = strtr($output, $cache['punctuation']);

    // Reduce strings to letters and numbers.
    if ($cache['reduce_ascii']) {
      $output = preg_replace('/[^a-zA-Z0-9\/]+/', $cache['separator'], $output);
    }

    // Get rid of words that are on the ignore list.
    if ($cache['ignore_words_regex']) {
      $words_removed = $cache['ignore_words_callback']($cache['ignore_words_regex'], '', $output);
      if (Unicode::strlen(trim($words_removed)) > 0) {
        $output = $words_removed;
      }
    }

    // Always replace whitespace with the separator.
    $output = preg_replace('/\s+/', $cache['separator'], $output);

    // Trim duplicates and remove trailing and leading separators.
    $output = $this->getCleanSeparators($this->getCleanSeparators($output, $cache['separator']));

    // Optionally convert to lower case.
    if ($cache['lowercase']) {
      $output = Unicode::strtolower($output);
    }

    // Shorten to a logical place based on word boundaries.
    $output = truncate_utf8($output, $cache['maxlength'], TRUE);

    // Cache this result in the static array.
    $cache['strings'][$langcode][$string] = $output;

    return $output;
  }

  /**
   * Trims duplicate, leading, and trailing separators from a string.
   *
   * @param string $string
   *   The string to clean path separators from.
   * @param string $separator
   *   The path separator to use when cleaning.
   *
   * @return string
   *   The cleaned version of the string.
   *
   * @see pathauto_cleanstring()
   * @see pathauto_clean_alias()
   */
  protected function getCleanSeparators($string, $separator = NULL) {
    static $default_separator;
    $config = \Drupal::configFactory()->get('separator');

    if (!isset($separator)) {
      if (!isset($default_separator)) {
        $default_separator = $config->get('separator');
      }
      $separator = $default_separator;
    }

    $output = $string;

    if (strlen($separator)) {
      // Trim any leading or trailing separators.
      $output = trim($output, $separator);

      // Escape the separator for use in regular expressions.
      $seppattern = preg_quote($separator, '/');

      // Replace multiple separators with a single one.
      $output = preg_replace("/$seppattern+/", $separator, $output);

      // Replace trailing separators around slashes.
      if ($separator !== '/') {
        $output = preg_replace("/\/+$seppattern\/+|$seppattern\/+|\/+$seppattern/", "/", $output);
      }
    }

    return $output;
  }

  /**
   * Clean up an URL alias.
   *
   * Performs the following alterations:
   * - Trim duplicate, leading, and trailing back-slashes.
   * - Trim duplicate, leading, and trailing separators.
   * - Shorten to a desired length and logical position based on word boundaries.
   *
   * @param string $alias
   *   A string with the URL alias to clean up.
   *
   * @return string
   *   The cleaned URL alias.
   */
  public function cleanAlias($alias) {
    $cache = &drupal_static(__FUNCTION__);
    $config = \Drupal::configFactory()->get('pathauto.settings');
    module_load_include('inc', 'pathauto');

    if (!isset($cache)) {
      $cache = array(
        'maxlength' => min($config->get('max_length'), _pathauto_get_schema_alias_maxlength()),
      );
    }

    $output = $alias;

    // Trim duplicate, leading, and trailing separators. Do this before cleaning
    // backslashes since a pattern like "[token1]/[token2]-[token3]/[token4]"
    // could end up like "value1/-/value2" and if backslashes were cleaned first
    // this would result in a duplicate blackslash.
    $output = $this->getCleanSeparators($output);

    // Trim duplicate, leading, and trailing backslashes.
    $output = $this->getCleanSeparators($output, '/');

    // Shorten to a logical place based on word boundaries.
    $output = truncate_utf8($output, $cache['maxlength'], TRUE);

    return $output;
  }

  /**
   * Apply patterns to create an alias.
   *
   * @param string $module
   *   The name of your module (e.g., 'node').
   * @param string $op
   *   Operation being performed on the content being aliased
   *   ('insert', 'update', 'return', or 'bulkupdate').
   * @param string $source
   *   An internal Drupal path to be aliased.
   * @param array $data
   *   An array of keyed objects to pass to token_replace(). For simple
   *   replacement scenarios 'node', 'user', and others are common keys, with an
   *   accompanying node or user object being the value. Some token types, like
   *   'site', do not require any explicit information from $data and can be
   *   replaced even if it is empty.
   * @param string $type
   *   For modules which provided pattern items in hook_pathauto(),
   *   the relevant identifier for the specific item to be aliased
   *   (e.g., $node->type).
   * @param string $language
   *   A string specify the path's language.
   *
   * @return array|string
   *   The alias that was created.
   *
   * @see _pathauto_set_alias()
   */
  public function createAlias($module, $op, $source, $data, $type = NULL, $language = Language::LANGCODE_NOT_SPECIFIED) {
    $config = \Drupal::configFactory()->get('pathauto.settings');
    module_load_include('inc', 'pathauto');

    // Retrieve and apply the pattern for this content type.
    $pattern = pathauto_pattern_load_by_entity($module, $type, $language);

    // Allow other modules to alter the pattern.
    $context = array(
      'module' => $module,
      'op' => $op,
      'source' => $source,
      'data' => $data,
      'type' => $type,
      'language' => &$language,
    );
    \Drupal::moduleHandler()->alter('pathauto_pattern', $pattern, $context);

    if (empty($pattern)) {
      // No pattern? Do nothing (otherwise we may blow away existing aliases...)
      return '';
    }

    // Special handling when updating an item which is already aliased.
    $existing_alias = NULL;
    if ($op == 'update' || $op == 'bulkupdate') {
      if ($existing_alias = _pathauto_existing_alias_data($source, $language)) {
        switch ($config->get('update_action')) {
          case PATHAUTO_UPDATE_ACTION_NO_NEW:
            // If an alias already exists,
            // and the update action is set to do nothing,
            // then gosh-darn it, do nothing.
            return '';
        }
      }
    }

    // Replace any tokens in the pattern.
    // Uses callback option to clean replacements. No sanitization.
    $alias = \Drupal::token()->replace($pattern, $data, array(
      'sanitize' => FALSE,
      'clear' => TRUE,
      'callback' => 'pathauto_clean_token_values',
      'language' => (object) array('language' => $language),
      'pathauto' => TRUE,
    ));

    // Check if the token replacement has not actually replaced any values. If
    // that is the case, then stop because we should not generate an alias.
    // @see token_scan()
    $pattern_tokens_removed = preg_replace('/\[[^\s\]:]*:[^\s\]]*\]/', '', $pattern);
    if ($alias === $pattern_tokens_removed) {
      return '';
    }

    $alias = $this->cleanAlias($alias);

    // Allow other modules to alter the alias.
    $context['source'] = &$source;
    $context['pattern'] = $pattern;
    \Drupal::moduleHandler()->alter('pathauto_alias', $alias, $context);

    // If we have arrived at an empty string, discontinue.
    if (!Unicode::strlen($alias)) {
      return '';
    }

    // If the alias already exists, generate a new, hopefully unique, variant.
    $original_alias = $alias;
    pathauto_alias_uniquify($alias, $source, $language);
    if ($original_alias != $alias) {
      // Alert the user why this happened.
      _pathauto_verbose(t('The automatically generated alias %original_alias conflicted with an existing alias. Alias changed to %alias.', array(
        '%original_alias' => $original_alias,
        '%alias' => $alias,
      )), $op);
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
    $path = _pathauto_set_alias($path, $existing_alias, $op);
    return $path;
  }
}
