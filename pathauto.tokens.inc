<?php
// $Id$

/**
 * @file
 * Builds placeholder replacement tokens for pathauto.
 *
 * @ingroup pathauto
 */

/**
 * Implements hook_token_list().
 */
function _pathauto_token_list($type = 'all') {
  $tokens = array();
  if (module_exists('taxonomy')) {
    if ($type == 'taxonomy' || $type == 'all') {
      $tokens['taxonomy']['catpath'] = t('As [cat], but including its supercategories separated by /.');
      $tokens['taxonomy']['catpath-raw'] = t('As [cat-raw], but including its supercategories separated by /.');
      $tokens['taxonomy']['catalias'] = t('URL alias for the term.');
    }
    if ($type == 'node' || $type == 'all') {
      $tokens['node']['termpath'] = t('As [term], but including its supercategories separated by /.');
      $tokens['node']['termpath-raw'] = t('As [term-raw], but including its supercategories separated by /.');
      $tokens['node']['termalias'] = t('The URL alias of the taxonomy term.');
    }
  }
  if (module_exists('book')) {
    if ($type == 'node' || $type == 'all') {
      $tokens['node']['bookpathalias'] = t('The URL alias of the parent book of the node.');
    }
  }
  return $tokens;
}

/**
 * Implements hook_token_values().
 */
function _pathauto_token_values($type, $object = NULL, $options = array(), $label = NULL) {
  $values = array();

  switch ($type) {
    case 'node':
      // Token [bookpathalias].
      if (module_exists('book')) {
        $values['bookpathalias'] = '';
        if (!empty($object->book['plid']) && $parent = book_link_load($object->book['plid'])) {
          $values['bookpathalias'] = drupal_get_path_alias($parent['href']);
        }
      }

      // Tokens [termpath], [termpath-raw], and [termalias].
      if (module_exists('taxonomy')) {
        // Get the lowest-weighted term from the lowest-weighted vocabulary.
        // This query is copied from @taxonomy_node_get_terms()
        $term = db_fetch_object(db_query_range('SELECT t.* FROM {term_node} r INNER JOIN {term_data} t ON r.tid = t.tid INNER JOIN {vocabulary} v ON t.vid = v.vid WHERE r.vid = %d ORDER BY v.weight, t.weight, t.name', $object->vid, 0, 1));
        if ($term) {
          $values = array_merge($values, _pathauto_token_values('taxonomy', $term, $options, 'term'));
        }
        else {
          $values['termpath'] = '';
          $values['termpath-raw'] = '';
          $values['termalias'] = '';
        }
      }
      break;

    case 'taxonomy':
      // In the realm of nodes these are 'terms', in the realm of taxonomy, 'cats'.
      if (!isset($label)) {
        $label = 'cat';
      }

      $values[$label . 'path'] = '';
      $values[$label . 'path-raw'] = '';
      $values[$label . 'alias'] = '';

      // Tokens [catpath] and [catpath-raw].
      if (isset($object->tid)) {
        module_load_include('inc', 'pathauto');
        $parents = taxonomy_get_parents_all($object->tid);
        $catpath = $catpath_raw = array();
        foreach ($parents as $parent) {
          $term_name_cleaned = pathauto_cleanstring($parent->name);
          array_unshift($catpath, check_plain($term_name_cleaned));
          array_unshift($catpath_raw, $term_name_cleaned);
        }
        $values[$label . 'path'] = implode('/', $catpath);
        $values[$label . 'path-raw'] = implode('/', $catpath_raw);

        // Token [catalias].
        $values[$label . 'alias'] = drupal_get_path_alias(taxonomy_term_path($object));
        if (!strncasecmp($values[$label .'alias'], 'taxonomy', 8)) {
          // @todo What is this doing??
          $values[$label . 'alias'] = check_plain($object->name);
        }
      }
      break;
  }

  return $values;
}
