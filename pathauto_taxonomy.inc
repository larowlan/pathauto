<?php
// $Id$

/*
 * Implementation of hook_pathauto()
 */

function taxonomy_pathauto($op) {
  switch ($op) {
    case 'settings':
      $settings = array();
      $settings['module'] = 'taxonomy';
      $settings['groupheader'] = t('Category path settings');
      $settings['patterndescr'] = t('Default path pattern (applies to all vocabularies with blank patterns below)');
      $settings['patterndefault'] = t('category/[vocab]/[catpath]');
      $patterns = token_get_list('taxonomy');
      foreach ($patterns['taxonomy'] as $pattern => $description) {
        $settings['placeholders'][t('['. $pattern .']')] = t($description);
      }
      $settings['supportsfeeds'] = '0/feed';
      $settings['bulkname'] = t('Bulk generate aliases for categories that are not aliased');
      $settings['bulkdescr'] = t('Generate aliases for all existing categories which do not already have aliases. Note: Bulk Update may not complete on large or slow sites. See the README.txt for more information.');

      $vocabularies = taxonomy_get_vocabularies();
      if (sizeof($vocabularies) > 0) {
        $settings['patternitems'] = array();
        foreach ($vocabularies as $vocab) {
          $vocabname = $vocab->name;
          $fieldlabel = t('Pattern for all %vocab-name paths', array('%vocab-name' => $vocabname));
          $settings['patternitems'][$vocab->vid] = $fieldlabel;
        }
      }
      return (object) $settings;
    default:
      break;
  }
}

/**
 * Implementation of hook_taxonomy().
 */
function pathauto_taxonomy($op, $type, $object = NULL) {
  switch ($type) {
    case 'term':
      switch ($op) {
        case 'insert':
        case 'update':
          /*
          ** Use the category info to automatically create an alias
          */
          $category = (object) $object;
          if ($category->name) {
            $count = _taxonomy_pathauto_alias($category, $op);
          }
          break;
        case 'delete':
          //If the category is deleted, remove the path aliases
          $category = (object) $object;
          path_set_alias('taxonomy/term/'. $category->tid);
          path_set_alias('forum/'.$category->tid);
          path_set_alias('taxonomy/term/'. $category->tid .'/0/feed');
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/**
 * Generate aliases for all categories without aliases
 * 
 */ 
function taxonomy_pathauto_bulkupdate() {
  $query = "SELECT tid, vid, name, src, dst FROM {term_data} LEFT JOIN {url_alias} ON CONCAT('taxonomy/term/', tid) = src OR CONCAT('forum/', tid) = src WHERE src IS NULL";
  $result = db_query_range($query, 0, variable_get('pathauto_max_bulk_update', 50));
  
  $count = 0;
  $placeholders = array();
  while ($category = db_fetch_object($result)) {
    $count = _taxonomy_pathauto_alias($category, 'bulkupdate') + $count;
  }
  
  drupal_set_message(format_plural($count,
    "Bulk generation of terms completed, one alias generated.",
    "Bulk generation of terms completed, @count aliases generated."));
}

/**
 * 
 * Function to create aliases for taxonomy objects
 * 
 * @param object $category a taxonomy object
 * 
 */
function _taxonomy_pathauto_alias($category, $op) {

  $count = 0;

  $placeholders = pathauto_get_placeholders('taxonomy', $category);    
  
  $forum_vid = variable_get('forum_nav_vocabulary', '');
  if ($forum_vid === $category->vid) {
    $src = 'forum/'.$category->tid;
    if ($forumalias = pathauto_create_alias('taxonomy', $op, $placeholders, $src, $forum_vid)) {
      $count++;
    }
  }

  $src = 'taxonomy/term/'.$category->tid;
  if ($alias = pathauto_create_alias('taxonomy', $op, $placeholders, $src, $category->vid)) {
    $count++;
  }
  return $count;
}