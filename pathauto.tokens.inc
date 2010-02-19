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
function pathauto_token_list($type = 'all') {
  $tokens = array();
  if (module_exists('taxonomy')) {
    if ($type == 'taxonomy' || $type == 'all') {
      $tokens['taxonomy']['catpath'] = t('As [cat], but including its supercategories separated by /.');
      $tokens['taxonomy']['catpath-raw'] = t('As [cat-raw], but including its supercategories separated by /. WARNING - raw user input.');
      $tokens['taxonomy']['catalias'] = t('URL alias for the term.');
    }
    if ($type == 'node' || $type == 'all') {
      $tokens['node']['termpath'] = t('As [term], but including its supercategories separated by /.');
      $tokens['node']['termpath-raw'] = t('As [term-raw], but including its supercategories separated by /. WARNING - raw user input.');
      $tokens['node']['termalias'] = t('URL alias for the term.');
    }
  }
  if (module_exists('book')) {
    if ($type == 'node' || $type == 'all') {
      $tokens['node']['bookpathalias'] = t('URL alias for the parent book.');
    }
  }
  return $tokens;
}

/**
 * Implements hook_token_values().
 */
function pathauto_token_values($type, $object = NULL) {
  $values = array();
  if (module_exists('taxonomy')) {
    if ($type == 'taxonomy' || $type == 'node' || $type == 'all') {
      _pathauto_include();
      switch ($type) {
        case 'node':
          // Get the bookpathalias token
          $values['bookpathalias'] = '';
          // We're on a node and it's a book and it has a parent? Get the book path alias.
          if (module_exists('book') && !empty($object->book['plid']) && $parent = book_link_load($object->book['plid'])) {
            $values['bookpathalias'] = drupal_get_path_alias($parent['href']);
          }

          // Get taxonomy related data.
          $vid = db_result(db_query_range("SELECT t.vid FROM {term_node} r INNER JOIN {term_data} t ON r.tid = t.tid INNER JOIN {vocabulary} v ON t.vid = v.vid WHERE r.vid = %d ORDER BY v.weight, t.weight, t.name", $object->vid, 0, 1));
          $category = db_fetch_object(db_query_range("SELECT t.tid, t.name FROM {term_data} t INNER JOIN {term_node} r ON r.tid = t.tid WHERE t.vid = %d AND r.nid = %d ORDER BY t.weight", $vid, $object->nid, 0, 1));
          $category->vid = $vid;
          // In the realm of nodes these are terms, in the realm of Taxonomy, cats
          $label = 'term';

        case 'taxonomy':
        default:
          if (!isset($category)) {
            $category = $object;
          }
          if (!isset($label)) {
            $label = 'cat';
          }
          if (isset($category->tid)) {
            $separator = variable_get('pathauto_separator', '-');
            $parents = taxonomy_get_parents_all($category->tid);
            array_shift($parents);
            $catpath = '';
            $catpath_raw = '';
            foreach ($parents as $parent) {
              $catpath = pathauto_cleanstring(check_plain($parent->name)) .'/'. $catpath;
              $catpath_raw = pathauto_cleanstring($parent->name) .'/'. $catpath_raw;
            }
            $values[$label .'path'] = $catpath . pathauto_cleanstring(check_plain($category->name));
            $values[$label .'path-raw'] = $catpath_raw . pathauto_cleanstring($category->name);

            $term = taxonomy_get_term($category->tid);
            $values[$label .'alias'] = drupal_get_path_alias(taxonomy_term_path($term));
            if (!strncasecmp($values[$label .'alias'], 'taxonomy', 8)) {
              $values[$label .'alias'] = check_plain($category->name);
            }
          }
          else { // Provide some defaults if they aren't set.
            $values[$label .'path'] = '';
            $values[$label .'path-raw'] = '';
            $values[$label .'alias'] = '';
          }
      }
    }
  }
  return $values;
}
